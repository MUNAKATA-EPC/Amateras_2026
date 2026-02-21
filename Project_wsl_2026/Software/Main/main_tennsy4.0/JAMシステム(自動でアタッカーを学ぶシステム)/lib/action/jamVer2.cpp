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
    uiPrint(15, 40, print_message[0]);
    uiPrint(15, 50, print_message[1]);
    uiPrint(15, 60, print_message[2]);
    uiPrint(15, 70, print_message[3]);
}

// JAMのメイン制御関連

struct Data
{
    int deg = 0xFF;   // 角度のデータ
    int power = 0xFF; // モータの出力のデータ

    Data operator+(const Data &other) const
    {
        return {deg + other.deg, power + other.power};
    }
    Data operator*(int val) const
    {
        return {deg * val, power * val};
    }
    Data operator/(int val) const
    {
        return {deg / val, power / val};
    }
    Data &operator+=(const Data &other)
    {
        deg += other.deg;
        power += other.power;
        return *this;
    }
};

static Data real_data[91];                               // -45度から+45度までのデータを格納する配列
static Data processed_data[91];                          // 加工（補完）されたデータを格納する配列
static Data data_list[31][91];                           // 30個の学習データを格納する配列（31番目はその平均値）
static JAM::Mode last_mode = static_cast<JAM::Mode>(-1); // 前回のモード状態を保持

int degToIdex1(int deg)
{
    if (abs(deg) > 45)
        return -1;   // 範囲外
    return 45 - deg; // -45→90、0→45、+45→0
}

void playJAMVer2(JAM::Mode mode)
{
    // モード切り替えの瞬間を検知
    bool once = (mode != last_mode);

    // MANABU処理：学習モード（real_data）
    if (mode == JAM::Mode::MANABU)
    {
        static Timer stop_for_print_timer; // ユーザーにprintが見えるように処理を一時停止するためのタイマー
        static Data sum_data[91];          // 学習データの合計値を格納する配列
        static unsigned int count[91];     // 何個学習したか
        static bool mananda_flag = false;  // 学習したかどうか

        motorsPdProcess(&pd_gyro, bnoDeg(), 0);

        if (stop_for_print_timer.everReset() && stop_for_print_timer.msTime() < 1000)
        {
            motorsPdMove(); // 停止
        }
        else if (once || ps3ButtonIsPressed(Ps3Button::CROSS)) // ×ボタンが押されたとき：データ（real_dataも）削除
        {
            printMessage("reset data", "", "", "");

            for (int i = 0; i < 91; i++) // 学習データの初期化
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
            int power = 80.0f * ps3LeftStickDis() / 128.0f;

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

                    if (index1 != -1 && power > 10)
                    {
                        printMessage("mananderu...", "", "", "");

                        sum_data[index1] += {deg, power};
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

                for (int i = 0; i < 91; i++)
                {
                    if (count[i] != 0)
                    {
                        real_data[i] = sum_data[i] / count[i]; // 平均を代入
                    }
                    else
                    {
                        real_data[i] = {0xFF, 0xFF}; // 未測定のところは0xFFを代入
                    }

                    sum_data[i] = {0, 0};
                    count[i] = 0;
                }

                mananda_flag = false;

                stop_for_print_timer.reset(); // 1000ms停止したい
            }
            else if (ps3ButtonIsPressed(Ps3Button::R1)) // R1ボタンが押されたとき：計測したsum_data,countを全削除
            {
                printMessage("cancel data", "", "", "");

                for (int i = 0; i < 91; i++)
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
                    printMessage("L1(save) or R1(cancel)", "", "", "");
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
            shortage_data_flag = false; // ★修正：毎回リセットしないと一度trueになったら機能しなくなる

            for (int i = 0; i < 91; i++) // real_dataをprocessed_dataに変換する（線形補間）
            {
                if (real_data[i].deg != 0xFF) // データが問題なくある場合：そのまま代入
                {
                    processed_data[i] = real_data[i];
                }
                else // データがない場合：線形補間
                {
                    bool min_found = false;
                    int min_index = i;
                    for (int a = i; a >= 0; a--)
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
                    for (int b = i; b <= 90; b++)
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

                        processed_data[i] =
                            (real_data[min_index] * max_k + real_data[max_index] * min_k) / (min_k + max_k);
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
                for (int i = 0; i < 91; i++)
                {
                    data_list[list_current_index][i] = processed_data[i];
                }

                list_current_index = (list_current_index + 1) % 30; // 0 ~ 29の周期で回す

                if (list_filled_count < 30) // ★修正：30未満の時だけ増やす論理に整理
                {
                    list_filled_count++;
                }

                if (list_filled_count > 0) // ★修正：ゼロ割り算によるクラッシュを防ぐ安全対策
                {
                    for (int i = 0; i < 91; i++)
                    {
                        Data sum = {0, 0};
                        for (int j = 0; j < list_filled_count; j++)
                        {
                            sum += data_list[j][i];
                        }

                        data_list[30][i] = sum / list_filled_count; // 平均値を格納
                    }
                }
            }

            if (shortage_data_flag)
            {
                printMessage("shortage of data", "", "", "");
            }
            else
            {
                // list_current_indexは既に次に進んでいるため、保存したのは「1つ前」のインデックス
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
                printMessage(String(saved_index) + "th data saved", "", "", "");
            }
        }
    }
    // KAKUNIN処理：processed_dataをもとに回り込みを再現
    else if (mode == JAM::Mode::KAKUNIN)
    {
        motorsPdProcess(&pd_gyro, bnoDeg(), 0);

        static int list_index = 0;
        if (ps3ButtonIsPressed(Ps3Button::UP))
        {
            list_index = (list_index + 1) % 31;
        }
        else if (ps3ButtonIsPressed(Ps3Button::DOWN))
        {
            list_index = (list_index - 1 + 31) % 31;
        }

        if (list_index < 30)
        {
            printMessage(String(list_index) + "th data load", "", "", "");
        }
        else
        {
            printMessage("ave data load", "", "", "");
        }

        if (irDetected())
        {
            int index1 = degToIdex1(irDeg());

            if (index1 != -1)
            {
                int deg = data_list[list_index][index1].deg;
                int power = data_list[list_index][index1].power;

                if (deg != 0xFF)
                {
                    motorsMove(deg, power);
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
        else
        {
            motorsPdMove();
        }
    }
    // KAKIKOMU処理：データをSDカードに書き込む
    else if (mode == JAM::Mode::KAKIKOMU)
    {
        printMessage("", "", "", "");
    }
    // ATTACKER処理：未定
    else if (mode == JAM::Mode::ATTACKER)
    {
        printMessage("", "", "", "");
    }

    last_mode = mode;
}