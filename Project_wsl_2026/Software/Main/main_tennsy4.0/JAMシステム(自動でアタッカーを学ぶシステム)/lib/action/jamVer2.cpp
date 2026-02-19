#include "jamVer2.hpp"

int degToIdex1(int deg)
{
    if (abs(deg) > 45)
        return -1;   // 範囲外
    return 45 - deg; // -45→90、0→45、+45→0
}

// 変数の定義
static JAM::Mode last_mode;
static String print_jam_message = "";

void playJAM(JAM::Mode mode)
{
    // モード切り替えの瞬間を検知
    bool once = (mode != last_mode);

    // MANABU処理：学習モード
    if (mode == JAM::Mode::MANABU)
    {
        if (once || ps3ButtonIsPushing(Ps3Button::CROSS))
        {
            print_jam_message = "reset data";
            for (int i = 0; i < 91; i++)
            {
                reliable_subdata_counter[i] = 0;
                for (int j = 0; j < 5; j++)
                {
                    deg_subdata[i][j] = 0;
                    power_subdata[i][j] = 0;
                }
            }
        }
        else
        {
            print_jam_message = "collecting data";
        }

        // 基本姿勢制御
        motorsPdProcess(&pd_gyro, bnoDeg(), 0);

        if (ps3LeftStickDetected())
        {
            int move_deg = ps3LeftStickDeg();
            int move_pow = 100.0f * ps3LeftStickDis() / 128.0f;
            motorsMove(move_deg, move_pow);

            int ir_deg = irDeg();
            int idx1 = degToIdex1(ir_deg);

            // 赤外線検知中かつCIRCLEボタン押下で学習
            if (irDetected() && idx1 != -1 && ps3ButtonIsPushing(Ps3Button::CIRCLE))
            {
                fullColorLed1.rgbLightUp(0, 10, 10);

                if (reliable_subdata_counter[idx1] == 5) // 6個目のデータが入った場合
                {
                    int temp_deg[6], temp_pow[6];
                    long sum_deg = move_deg;
                    temp_deg[5] = move_deg;
                    temp_pow[5] = move_pow;

                    for (int i = 0; i < 5; i++)
                    {
                        temp_deg[i] = deg_subdata[idx1][i];
                        temp_pow[i] = power_subdata[idx1][i];
                        sum_deg += temp_deg[i];
                    }

                    int ave_deg = sum_deg / 6;
                    int outlier_idx = 0;
                    int max_diff = -1;

                    // 平均から最も遠い値（外れ値）を特定
                    for (int i = 0; i < 6; i++)
                    {
                        int diff = abs(temp_deg[i] - ave_deg);
                        if (diff > max_diff)
                        {
                            max_diff = diff;
                            outlier_idx = i;
                        }
                    }

                    // 外れ値を除いた5つを保存
                    int k = 0;
                    for (int i = 0; i < 6; i++)
                    {
                        if (i == outlier_idx)
                            continue;
                        deg_subdata[idx1][k] = temp_deg[i];
                        power_subdata[idx1][k] = temp_pow[i];
                        k++;
                    }
                }
                else
                {
                    // カウンターが5未満ならそのまま追加
                    int current_cnt = reliable_subdata_counter[idx1];
                    deg_subdata[idx1][current_cnt] = move_deg;
                    power_subdata[idx1][current_cnt] = move_pow;
                    reliable_subdata_counter[idx1]++;
                }
            }
        }
        else
        {
            motorsPdMove();
        }
    }
    // KEISAN処理：データ整理と線形補完
    else if (mode == JAM::Mode::KEISAN)
    {
        if (once)
        {
            print_jam_message = "keisan kaishi";

            // ソートと転送
            for (int i = 0; i < 91; i++)
            {
                int count = reliable_subdata_counter[i];
                if (count > 0)
                {
                    long s_deg = 0;
                    for (int j = 0; j < count; j++)
                        s_deg += deg_subdata[i][j];
                    int avg_deg = s_deg / count;

                    int temp_indices[5] = {0, 1, 2, 3, 4};
                    // 選択ソート（平均に近い順）
                    for (int m = 0; m < count - 1; m++)
                    {
                        for (int n = m + 1; n < count; n++)
                        {
                            if (abs(deg_subdata[i][temp_indices[n]] - avg_deg) < abs(deg_subdata[i][temp_indices[m]] - avg_deg))
                            {
                                int tmp = temp_indices[m];
                                temp_indices[m] = temp_indices[n];
                                temp_indices[n] = tmp;
                            }
                        }
                    }
                    // 確定データ配列へ格納
                    for (int j = 0; j < 5; j++)
                    {
                        int target = (j < count) ? temp_indices[j] : temp_indices[0];
                        deg_data[i][j] = deg_subdata[i][target];
                        power_data[i][j] = power_subdata[i][target];
                    }
                }
            }

            // 線形補完
            for (int i = 0; i < 91; i++)
            {
                if (reliable_subdata_counter[i] == 0)
                {
                    int prev = -1, next = -1;
                    for (int p = i - 1; p >= 0; p--)
                    {
                        if (reliable_subdata_counter[p] > 0)
                        {
                            prev = p;
                            break;
                        }
                    }
                    for (int n = i + 1; n < 91; n++)
                    {
                        if (reliable_subdata_counter[n] > 0)
                        {
                            next = n;
                            break;
                        }
                    }

                    if (prev != -1 && next != -1) // 中間地点
                    {
                        for (int j = 0; j < 5; j++)
                        {
                            deg_data[i][j] = deg_data[prev][j] + (deg_data[next][j] - deg_data[prev][j]) * (i - prev) / (next - prev);
                            power_data[i][j] = power_data[prev][j] + (power_data[next][j] - power_data[prev][j]) * (i - prev) / (next - prev);
                        }
                    }
                    else if (prev != -1 || next != -1) // 端点
                    {
                        int src = (prev != -1) ? prev : next;
                        for (int j = 0; j < 5; j++)
                        {
                            deg_data[i][j] = deg_data[src][j];
                            power_data[i][j] = power_data[src][j];
                        }
                    }
                }
            }
            print_jam_message = "keisan kanryo";
        }
    }
    // KAKUNIN処理：学習済みデータで自律走行
    else if (mode == JAM::Mode::KAKUNIN)
    {
        print_jam_message = "kakunin running";
        motorsPdProcess(&pd_gyro, bnoDeg(), 0);

        if (irDetected())
        {
            int ir_deg = irDeg();
            int idx = degToIdex1(ir_deg);

            if (idx != -1) // ±45度以内なら学習データを使用
            {
                fullColorLed1.rgbLightUp(0, 20, 0);
                motorsMove(deg_data[idx][0], power_data[idx][0]);
            }
            else // 範囲外なら計算されたパワーで旋回
            {
                const int add_deg = 55;
                int base_idx = (ir_deg > 0) ? degToIdex1(45) : degToIdex1(-45);
                int target_deg = (ir_deg > 0) ? ir_deg + add_deg : ir_deg - add_deg;
                motorsMove(target_deg, power_data[base_idx][0]);
            }
        }
        else
        {
            motorsPdMove();
        }
    }
    // KAKIKOMU処理
    else if (mode == JAM::Mode::KAKIKOMU)
    {
        print_jam_message = "kakikomu";
    }
    // ATTACKER処理
    else if (mode == JAM::Mode::ATTACKER)
    {
        print_jam_message = "attacker";
    }

    last_mode = mode;
}

void printJAM(JAM::Mode mode)
{
    uiPrint(15, 40, print_jam_message);
}