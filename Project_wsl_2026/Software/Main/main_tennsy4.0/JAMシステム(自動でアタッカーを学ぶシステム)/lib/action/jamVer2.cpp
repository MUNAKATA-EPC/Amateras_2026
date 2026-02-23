#include "jamVer2.hpp"

// UIのデバッグ制御関連

static String print_message[4] = {"", "", "", ""}; // デバッグ用の文字列配列
void printMessage(String one, String two, String three, String four)
{
    print_message[0] = one;
    print_message[1] = two;
    print_message[2] = three;
    print_message[3] = four;
}

void printJAMVer2(JAM::Mode mode)
{
    uiPrint(15, 32, print_message[0]);
    uiPrint(15, 42, print_message[1]);
    uiPrint(15, 52, print_message[2]);
    uiPrint(15, 62, print_message[3]);
}

// JAMのメイン制御関連

static int max_power = 50;

struct Data
{
    float deg = 0xFF;   // 角度のデータ
    float power = 0xFF; // モータの出力のデータ

    Data operator+(const Data &other) const
    {
        return {deg + other.deg, power + other.power};
    }
    Data operator*(float val) const
    {
        return {deg * val, power * val};
    }
    Data operator/(float val) const
    {
        // ゼロ除算防止
        if (val == 0)
            return {0, 0};
        return {deg / val, power / val};
    }
    Data &operator+=(const Data &other)
    {
        deg += other.deg;
        power += other.power;
        return *this;
    }
};

void gaussianDataShifter(Data *data, int size, int radius, float sigma)
{
    float *deg_buf = new float[size];
    float *pow_buf = new float[size];

    for (int i = 0; i < size; i++)
    {
        deg_buf[i] = data[i].deg;
        pow_buf[i] = data[i].power;
    }

    gaussianShifter(deg_buf, size, radius, sigma);
    gaussianShifter(pow_buf, size, radius, sigma);

    for (int i = 0; i < size; i++)
    {
        data[i].deg = deg_buf[i];
        data[i].power = pow_buf[i];
    }

    delete[] deg_buf;
    delete[] pow_buf;
}

static Data real_data[181];      // -90度から+90度までのデータを格納する配列
static Data processed_data[181]; // 加工（補完）されたデータを格納する配列
static Data data_list[32][181];
// 30個の学習データを格納する配列（31番目はその平均値）（32番目は平均値をガウシアン非常化した値）
static JAM::Mode last_mode = static_cast<JAM::Mode>(-1); // 前回のモード状態を保持

int degToIdex1(int deg)
{
    if (deg < -90)
        return -1;
    if (deg > 90)
        return -1;

    return 90 - deg; // -90→180、0→90、+90→0
}

void playJAMVer2(JAM::Mode mode)
{
    // モード切り替えの瞬間を検知
    bool once = (mode != last_mode);

    // MANABU処理：学習モード（real_data）
    if (mode == JAM::Mode::MANABU)
    {
        static Timer stop_for_print_timer; // ユーザーにprintが見えるように処理を一時停止するためのタイマー
        static Data sum_data[181];         // 学習データの合計値を格納する配列
        static unsigned int count[181];    // 何個学習したか
        static bool mananda_flag = false;  // 学習したかどうか

        motorsPdProcess(&pd_gyro, bnoDeg(), 0);

        if (stop_for_print_timer.everReset() && stop_for_print_timer.msTime() < 1000)
        {
            motorsPdMove(); // 停止
        }
        else if (once || ps3ButtonIsPressed(Ps3Button::CROSS)) // ×ボタンが押されたとき：データ（real_dataも）削除
        {
            printMessage("reset data", "", "", "");

            for (int i = 0; i < 181; i++) // 学習データの初期化
            {
                real_data[i] = {0xFF, 0xFF};
                sum_data[i] = {0, 0};
                count[i] = 0;
            }
            mananda_flag = false;

            motorsPdMove();

            stop_for_print_timer.reset(); // 1000ms停止したい
        }
        else
        {
            bool is_move = ps3LeftStickDetected();
            int deg = ps3LeftStickDeg();
            int power = max_power * ps3LeftStickDis() / 128.0f;

            if (is_move)
            {
                motorsMove(deg, power);
            }
            else
            {
                motorsPdMove();
            }

            if (ps3ButtonIsPushing(Ps3Button::CIRCLE)) // 〇ボタンが押されたとき：sum_data,countに計測データを代入（加算）
            {
                if (irDetected())
                {
                    int index1 = degToIdex1(irDeg());

                    if (index1 != -1 && is_move && catchSensor.read() == LOW)
                    {
                        printMessage("mananderu...", "", "", "");

                        sum_data[index1] += {(float)deg, (float)power};
                        count[index1]++;
                        mananda_flag = true;
                    }
                    else
                    {
                        printMessage("manaben!", "", "", "");
                    }
                }
                else
                {
                    printMessage("manaben!", "", "", "");
                }
            }
            else if (ps3ButtonIsPressed(Ps3Button::L1)) // L1ボタンが押されたとき：計測したsum_dataから平均を出してreal_dataに代入
            {
                printMessage("save data", "", "", "");

                for (int i = 0; i < 181; i++)
                {
                    if (count[i] != 0)
                    {
                        real_data[i] = sum_data[i] / count[i]; // 平均を代入
                    }
                    // else { real_data[i]は更新せず前回の値を保持、もしくは0xFF }

                    sum_data[i] = {0, 0};
                    count[i] = 0;
                }

                mananda_flag = false;

                stop_for_print_timer.reset(); // 1000ms停止したい
            }
            else if (ps3ButtonIsPressed(Ps3Button::R1)) // R1ボタンが押されたとき：計測したsum_data,countを全削除
            {
                printMessage("cancel data", "", "", "");

                for (int i = 0; i < 181; i++)
                {
                    sum_data[i] = {0, 0};
                    count[i] = 0;
                }

                mananda_flag = false;

                stop_for_print_timer.reset(); // 1000ms停止したい
            }
            else
            {
                if (mananda_flag)
                {
                    printMessage("L1(save)", "R1(cancel)", "", "");
                }
                else
                {
                    printMessage("CIRCLE(manabu)", "", "", "");
                }
            }
        }
    }
    // KEISAN処理：データ整理と線形補完（processed_data）
    else if (mode == JAM::Mode::KEISAN)
    {
        motorsPdProcess(&pd_gyro, bnoDeg(), 0);
        motorsPdMove();

        static bool shortage_data_flag = false; // データ不足用フラグ
        static int list_filled_count = 0;       // リストの中にデータが何個溜まっているか（最大30個）
        static int list_current_index = 0;      // 現在のリスト番号（何個目のデータか）

        if (once)
        {
            shortage_data_flag = false; // 毎回リセットしないと一度trueになったら機能しなくなる

            for (int i = 0; i < 181; i++) // real_dataをprocessed_dataに変換する（線形補間）
            {
                if (real_data[i].deg != 0xFF) // データが問題なくある場合：そのまま代入
                {
                    processed_data[i] = real_data[i];
                }
                else // データがない場合：線形補間
                {
                    bool min_found = false;
                    int min_index = i;
                    for (int a = i - 1; a >= 0; a--) // i自身を含まないよう修正
                    {
                        if (real_data[a].deg != 0xFF)
                        {
                            min_found = true;
                            min_index = a;
                            break;
                        }
                    }

                    bool max_found = false;
                    int max_index = i;
                    for (int b = i + 1; b <= 180; b++) // i自身を含まないよう修正
                    {
                        if (real_data[b].deg != 0xFF)
                        {
                            max_found = true;
                            max_index = b;
                            break;
                        }
                    }

                    if (min_found && max_found) // 線形補間
                    {
                        int min_k = i - min_index;
                        int max_k = max_index - i;
                        int total_k = min_k + max_k;

                        if (total_k > 0)
                        {
                            processed_data[i] = (real_data[min_index] * max_k + real_data[max_index] * min_k) / total_k;
                        }
                        else
                        {
                            processed_data[i] = real_data[min_index];
                        }
                    }
                    else if (min_found) // min_indexのデータを代入
                    {
                        processed_data[i] = real_data[min_index];
                    }
                    else if (max_found) // max_indexのデータをを代入
                    {
                        processed_data[i] = real_data[max_index];
                    }
                    else // min_found == false && max_found == false
                    {
                        processed_data[i] = {0xFF, 0xFF};
                        shortage_data_flag = true;
                    }
                }
            }

            if (shortage_data_flag == false) // データ不足ではない場合：リストにprocessed_dataを追加する
            {
                for (int i = 0; i < 181; i++)
                {
                    data_list[list_current_index][i] = processed_data[i];
                }

                list_current_index = (list_current_index + 1) % 30; // 0 ~ 29の周期で回す

                if (list_filled_count < 30) // 30未満の時だけ増やす論理に整理
                {
                    list_filled_count++;
                }

                if (list_filled_count > 0) // ゼロ割り算によるクラッシュを防ぐ安全対策
                {
                    for (int i = 0; i < 181; i++)
                    {
                        Data sum = {0.0f, 0.0f};
                        for (int j = 0; j < list_filled_count; j++)
                        {
                            sum += data_list[j][i];
                        }

                        data_list[30][i] = sum / list_filled_count; // 平均値を格納
                    }
                }

                Data temp_data[181];
                for (int i = 0; i < 181; i++)
                {
                    temp_data[i] = data_list[30][i];
                }
                gaussianDataShifter(temp_data, 181, 5, 1.0f); // ガウシアン平滑
                for (int i = 0; i < 181; i++)
                {
                    data_list[31][i] = temp_data[i];
                }
            }

            if (shortage_data_flag)
            {
                printMessage("shortage of data", "", "", "");
            }
            else
            {
                int saved_index = (list_current_index - 1 + 30) % 30;
                printMessage(String(saved_index) + "th data saved", "", "", "");
            }
        }
        else
        {
            if (shortage_data_flag)
            {
                printMessage("shortage of data", "", "", "");
            }
            else
            {
                int saved_index = (list_current_index - 1 + 30) % 30;

                static int check_deg = -90;
                if (ps3ButtonIsPressed(Ps3Button::UP))
                {
                    check_deg++;
                }
                else if (ps3ButtonIsPressed(Ps3Button::DOWN))
                {
                    check_deg--;
                }
                check_deg = constrain(check_deg, -90, 90);
                int check_index = degToIdex1(check_deg);

                printMessage(String(saved_index) + "th data saved",
                             "idx:" + String(check_deg),
                             "deg:" + String(data_list[saved_index][check_index].deg) + ",pow:" + String(data_list[saved_index][check_index].power),
                             "");
            }
        }
    }
    // KAKUNIN処理：processed_dataをもとに回り込みを再現
    else if (mode == JAM::Mode::KAKUNIN)
    {
        motorsPdProcess(&pd_gyro, bnoDeg(), 0);

        static int list_index = 0;
        if (once)
        {
            list_index = 0;
        }
        if (ps3ButtonIsPressed(Ps3Button::UP))
        {
            list_index = (list_index + 1) % 32;
        }
        else if (ps3ButtonIsPressed(Ps3Button::DOWN))
        {
            list_index = (list_index - 1 + 32) % 32;
        }

        static int part_index = 1;
        if (once)
        {
            part_index = 1;
        }
        if (ps3ButtonIsPressed(Ps3Button::LEFT))
        {
            part_index = (part_index - 1 + 3) % 3;
        }
        else if (ps3ButtonIsPressed(Ps3Button::RIGHT))
        {
            part_index = (part_index + 1) % 3;
        }
        String part_str = (part_index == 1)   ? "whole_part"
                          : (part_index == 0) ? "left_part"
                                              : "right_part";

        if (list_index < 30) // プリントする
        {
            printMessage(String(list_index) + "th data load", part_str, "", "");
        }
        else if (list_index == 30)
        {
            printMessage("ave data load", part_str, "", "");
        }
        else if (list_index == 31)
        {
            printMessage("gauss data load", part_str, "", "");
        }

        if (ps3ButtonIsPushing(Ps3Button::CIRCLE))
        {
            if (irDetected())
            {
                int ir_deg = irDeg();
                int index1 = degToIdex1(ir_deg);

                if (index1 != -1)
                {
                    fullColorLed1.rgbLightUp(0, 10, 0);

                    int deg = (int)data_list[list_index][index1].deg;
                    int power = (int)data_list[list_index][index1].power;

                    if (part_index == 0) // left_part: 左側のデータのみを使い、右側は反転で補う
                    {
                        if (ir_deg > 0) // 右側にIRがある場合
                        {
                            int mirror_idx = degToIdex1(-ir_deg);
                            if (data_list[list_index][mirror_idx].deg != 0xFF)
                            {
                                deg = -(int)data_list[list_index][mirror_idx].deg;
                                power = (int)data_list[list_index][mirror_idx].power;
                            }
                        }
                    }
                    else if (part_index == 2) // right_part: 右側のデータのみを使い、左側は反転で補う
                    {
                        if (ir_deg < 0) // 左側にIRがある場合
                        {
                            int mirror_idx = degToIdex1(-ir_deg);
                            if (data_list[list_index][mirror_idx].deg != 0xFF)
                            {
                                deg = -(int)data_list[list_index][mirror_idx].deg;
                                power = (int)data_list[list_index][mirror_idx].power;
                            }
                        }
                    }

                    // 0xFF保護
                    if (deg != 0xFF && power != 0xFF)
                    {
                        float power_ratio = 80.0f / max_power;
                        power = power * power_ratio;

                        // 安全範囲制限
                        deg = constrain(deg, -180, 180);
                        power = constrain(power, -100, 100);

                        motorsMove(deg, power);
                    }
                    else
                    {
                        motorsPdMove();
                    }
                }
                else
                {
                    fullColorLed1.rgbLightUp(0, 0, 10);

                    int ir_deg_fallback = irDeg();

                    if (irDis() > 550)
                    {
                        motorsMove(ir_deg_fallback, max_power);
                    }
                    else
                    {
                        int add_deg = (ir_deg_fallback < 0) ? -60 : 60;
                        motorsMove(ir_deg_fallback + add_deg, max_power);
                    }
                }
            }
            else
            {
                motorsPdMove();
            }
        }
        else
        {
            motorsPdMove();
        }
    }
    // KAKIKOMU処理：SDカードに書き込む
    else if (mode == JAM::Mode::KAKIKOMU)
    {
        printMessage("", "", "", "");
        motorsPdProcess(&pd_gyro, bnoDeg(), 0);
        motorsPdMove();
    }
    // ATTACKER処理：未定
    else if (mode == JAM::Mode::ATTACKER)
    {
        printMessage("", "", "", "");
        motorsPdProcess(&pd_gyro, bnoDeg(), 0);

        if (irDetected())
        {
            int add_deg = (irDeg() < 0) ? -45 : 45;
            motorsMove(irDeg() + add_deg, max_power);
        }
        else
        {
            motorsPdMove();
        }
    }

    last_mode = mode;
}