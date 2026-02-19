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
    // 計測中の一時データを保持する変数（静的変数として定義）
    static bool is_measuring = false;
    static long measure_sum_deg = 0;
    static long measure_sum_pow = 0;
    static int measure_count = 0;
    static int target_idx = -1;        // 計測開始時のインデックスを保持
    static bool last_l1_state = false; // L1の連続押し防止用

    if (mode == JAM::Mode::MANABU)
    {
        // L1ボタンの現在の状態を取得
        bool current_l1_state = ps3ButtonIsPushing(Ps3Button::L1);

        // ×ボタン：全データの初期化
        if (once || ps3ButtonIsPushing(Ps3Button::CROSS))
        {
            print_jam_message = "reset all data";
            is_measuring = false;
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

        // 基本姿勢制御
        motorsPdProcess(&pd_gyro, bnoDeg(), 0);

        if (ps3LeftStickDetected())
        {
            int move_deg = ps3LeftStickDeg();
            int move_pow = 100.0f * ps3LeftStickDis() / 128.0f;
            motorsMove(move_deg, move_pow);

            int ir_deg = irDeg();
            int idx1 = degToIdex1(ir_deg);

            // 1. ◎ボタンを押し続ける：一時的な計測（平均を取る準備）
            if (irDetected() && idx1 != -1 && ps3ButtonIsPushing(Ps3Button::CIRCLE))
            {
                // 計測開始の瞬間にインデックスを固定
                if (!is_measuring)
                {
                    target_idx = idx1;
                }
                is_measuring = true;
                measure_sum_deg += move_deg;
                measure_sum_pow += move_pow;
                measure_count++;
                print_jam_message = "measuring...";
                fullColorLed1.rgbLightUp(10, 10, 0); // 黄色：計測中
            }
            else
            {
                // ボタンを離したとき、計測状態を維持（L1/R1待ち）
                if (is_measuring)
                {
                    print_jam_message = "wait L1(save) or R1(cancel)";
                }
                else
                {
                    print_jam_message = "collecting data";
                }
            }

            // 2. L1ボタン：計測データを確定して subdata に格納
            if (is_measuring && current_l1_state && !last_l1_state && measure_count > 0 && target_idx != -1)
            {
                int avg_move_deg = measure_sum_deg / measure_count;
                int avg_move_pow = measure_sum_pow / measure_count;

                // 外れ値除去ロジックを用いて保存
                if (reliable_subdata_counter[target_idx] == 5)
                {
                    int temp_deg[6], temp_pow[6];
                    temp_deg[5] = avg_move_deg;
                    temp_pow[5] = avg_move_pow;
                    long sum_deg_for_outlier = avg_move_deg;

                    for (int i = 0; i < 5; i++)
                    {
                        temp_deg[i] = deg_subdata[target_idx][i];
                        temp_pow[i] = power_subdata[target_idx][i];
                        sum_deg_for_outlier += temp_deg[i];
                    }

                    int ave_deg_tmp = sum_deg_for_outlier / 6;
                    int outlier_idx = 0;
                    int max_diff = -1;
                    for (int i = 0; i < 6; i++)
                    {
                        int diff = abs(temp_deg[i] - ave_deg_tmp);
                        if (diff > max_diff)
                        {
                            max_diff = diff;
                            outlier_idx = i;
                        }
                    }

                    int k = 0;
                    for (int i = 0; i < 6; i++)
                    {
                        if (i == outlier_idx)
                            continue;
                        deg_subdata[target_idx][k] = temp_deg[i];
                        power_subdata[target_idx][k] = temp_pow[i];
                        k++;
                    }
                }
                else
                {
                    int current_cnt = reliable_subdata_counter[target_idx];
                    deg_subdata[target_idx][current_cnt] = avg_move_deg;
                    power_subdata[target_idx][current_cnt] = avg_move_pow;
                    reliable_subdata_counter[target_idx]++;
                }

                // 計測リセット
                is_measuring = false;
                measure_sum_deg = 0;
                measure_sum_pow = 0;
                measure_count = 0;
                target_idx = -1;
                print_jam_message = "measure saved";
                fullColorLed1.rgbLightUp(0, 10, 10); // シアン：保存完了
            }

            // 3. R1ボタン：現在の計測データを破棄
            if (is_measuring && ps3ButtonIsPushing(Ps3Button::R1))
            {
                is_measuring = false;
                measure_sum_deg = 0;
                measure_sum_pow = 0;
                measure_count = 0;
                target_idx = -1;
                print_jam_message = "measure canceled";
                fullColorLed1.rgbLightUp(10, 0, 0); // 赤色：キャンセル
            }
        }
        else
        {
            motorsPdMove();
            // スティックを離したときも計測中ならメッセージ維持
            if (is_measuring)
                print_jam_message = "wait L1/R1";
        }

        // L1の状態を保存（次回ループでの比較用）
        last_l1_state = current_l1_state;
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