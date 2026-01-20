#include "jam.hpp"

static Vector data[360][5];           // 最終的な回り込みデータ（ラグランジュ補間後）
static Vector data_temp[360][5];      // 生データ
static Vector data_temp_temp[360][5]; // 生データの仮置き場

// 角度からidx1に変える
int degToIdx1(int deg) { return (deg + 360) % 360; }
// 距離からidx1に変える（5段階で分ける）
int disToIdx2(int dis)
{
    if (dis > 650)
    {
        return 4;
    }
    else if (dis > 600)
    {
        return 3;
    }
    else if (dis > 550)
    {
        return 2;
    }
    else if (dis > 400)
    {
        return 1;
    }
    return 0;
}

// 標本化する角度を指定
// 0°~45°      : 左前ブロック
const int hidarimae_num = 5;
const int hidarimae_block[5] = {0, 10, 20, 30, 45};
// 45°~135°    : 左ブロック
const int hidari_num = 5;
const int hidari_block[5] = {45, 65, 85, 105, 135};
// 135°~180°   : 左後ろブロック
const int hidariusiro_num = 5;
const int hidariusiro_block[5] = {135, 145, 155, 165, 180};
// 0°~-45°     : 右前ブロック
const int migimae_num = 5;
const int migimae_block[5] = {0, -10, -20, -30, -45};
// -45°~-135°  : 右ブロック
const int migi_num = 5;
const int migi_block[5] = {-45, -65, -85, -105, -135};
// -135°~-180° : 右後ろブロック
const int migiusiro_num = 5;
const int migiusiro_block[5] = {-135, -145, -155, -165, -180};
// 指定した標本化する角度にデータが入っているか
uint8_t dataBlockCheck(Vector input_data[360][5], int idx2)
{
    uint8_t output = 0b000000;
    int count = 0;

    // 左前ブロック
    count = 0;
    for (int i = 0; i < hidarimae_num; i++)
    {
        int idx1 = degToIdx1(hidarimae_block[i]);
        if (!input_data[idx1][idx2].is_empty())
        {
            count++;
        }
    }
    if (count == hidarimae_num)
    {
        output += 0b100000;
    }

    // 左ブロック
    count = 0;
    for (int i = 0; i < hidari_num; i++)
    {
        int idx1 = degToIdx1(hidari_block[i]);
        if (!input_data[idx1][idx2].is_empty())
        {
            count++;
        }
    }
    if (count == hidari_num)
    {
        output += 0b010000;
    }

    // 左後ろブロック
    count = 0;
    for (int i = 0; i < hidariusiro_num; i++)
    {
        int idx1 = degToIdx1(hidariusiro_block[i]);
        if (!input_data[idx1][idx2].is_empty())
        {
            count++;
        }
    }
    if (count == hidariusiro_num)
    {
        output += 0b001000;
    }

    // 右前ブロック
    count = 0;
    for (int i = 0; i < migimae_num; i++)
    {
        int idx1 = degToIdx1(migimae_block[i]);
        if (!input_data[idx1][idx2].is_empty())
        {
            count++;
        }
    }
    if (count == migimae_num)
    {
        output += 0b000100;
    }

    // 右ブロック
    count = 0;
    for (int i = 0; i < migi_num; i++)
    {
        int idx1 = degToIdx1(migi_block[i]);
        if (!input_data[idx1][idx2].is_empty())
        {
            count++;
        }
    }
    if (count == migi_num)
    {
        output += 0b000010;
    }

    // 右後ろブロック
    count = 0;
    for (int i = 0; i < migiusiro_num; i++)
    {
        int idx1 = degToIdx1(migiusiro_block[i]);
        if (!input_data[idx1][idx2].is_empty())
        {
            count++;
        }
    }
    if (count == migiusiro_num)
    {
        output += 0b000001;
    }

    return output;
}

//// manabu処理：松浦に操作させ動きを記録させる　注意：uiConfigData(1)で選んだ距離インデックスの確認ができる ////
void manabu(bool once)
{
    if (once) // 初期化
    {
        for (int i = 0; i < 360; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                data_temp[i][j] = Vector(0, 0.0f);
                data_temp_temp[i][j] = Vector(0, 0.0f);
            }
        }
    }

    motorsPdProcess(&pd_gyro, bnoDeg(), 0);

    int deg = ps3LeftStickDeg();
    int power = ps3LeftStickDis() * 100.0f / 128.0f; // 最大100

    if (irDetected())
    {
        bool stop = (power <= 10.0f); // 停止判定

        if (ps3ButtonIsPushing(Ps3Button::R1) && !stop)
        {
            fullColorLed1.rgbLightUp(0, 0, 10);

            int idx1 = degToIdx1(irDeg());
            int idx2 = disToIdx2(irDis());

            if (data_temp_temp[idx1][idx2].is_empty())
            {
                data_temp_temp[idx1][idx2] = Vector(deg, power);
            }
            else
            {
                data_temp_temp[idx1][idx2] = (Vector(deg, power) + data_temp_temp[idx1][idx2]) / 2; // 合力とする
            }
        }
    }

    if (ps3ButtonIsPushing(Ps3Button::CIRCLE)) // 採用
    {
        for (int i = 0; i < 360; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                if (!data_temp_temp[i][j].is_empty())
                {
                    data_temp[i][j] = data_temp_temp[i][j];
                }
            }
        }
    }

    if (ps3ButtonIsPushing(Ps3Button::CROSS)) // 却下
    {
        for (int i = 0; i < 360; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                data_temp_temp[i][j] = Vector(0, 0.0f);
            }
        }
    }

    if (ps3ButtonIsPushing(Ps3Button::SQUARE)) // 確認
    {
        if (uiConfigData(1) < 5) // 0~4ならチェックする
        {
            uint8_t output = dataBlockCheck(data_temp, uiConfigData(1));

            if (output == 0b111111)
            {
                fullColorLed1.rgbLightUp(0, 10, 0);
            }
        }
    }

    motorsMove(deg, power);
}

//// keisan処理：ラグランジュ補間を用いて実測データをきれいに整える　注意：uiConfigData(1)で選んだ距離インデックスのみ補完する ////
void keisan(bool once)
{
    if (once)
    {
    }
}

//// kakunin処理：回り込みを実際にリプレイする　注意：uiConfigData(1)で選んだ距離インデックスのみの回り込みをする ////
void kakunin(bool once)
{
    motorsPdProcess(&pd_gyro, bnoDeg(), 0);

    if (irDetected())
    {
        fullColorLed1.rgbLightUp(0, 0, 10);

        int idx1 = degToIdx1(irDeg());
        int idx2 = disToIdx2(irDis());
        if (uiConfigData(1) < 5) // 0~4ならそれのみのチェックができるようにする
        {
            idx2 = uiConfigData(1);
        }

        Vector vec = data[idx1][idx2];
        motorsVectorMove(&vec);
    }
    else
    {
        motorsPdMove();
    }
}

//// kakikomu処理：SDカードに書き込み保存する ////
void kakikomu(bool once) {}

//// attacker処理：実際にアタッカーをする ////
void attacker(bool once) {}

//// メイン ////
static JAM::Mode last_mode;
void playJAM(JAM::Mode mode)
{
    bool once = (mode != last_mode); // 選択時一回だけtrueになる

    if (mode == JAM::Mode::MANABU)
    {
        manabu(once);
    }
    else if (mode == JAM::Mode::KEISAN)
    {
        keisan(once);
    }
    else if (mode == JAM::Mode::KAKUNIN)
    {
        kakunin(once);
    }
    else if (mode == JAM::Mode::KAKIKOMU)
    {
        kakikomu(once);
    }
    else if (mode == JAM::Mode::ATTACKER)
    {
        attacker(once);
    }

    last_mode = mode;
}
void printJAM(JAM::Mode mode)
{
    if (mode == JAM::Mode::MANABU)
    {
        if (uiConfigData(1) < 5) // 0~4の時のみチェックする
        {
            uint8_t progress = dataBlockCheck(data_temp, uiConfigData(1));
            String msg = "progress:";
            for (int i = 0; i < 6; i++)
            {
                if (progress & (1UL << i))
                {
                    msg += "1";
                }
                else
                {
                    msg += "0";
                }
            }
            uiPrint(15, 40, msg);
        }
        else
        {
            uiPrint(15, 40, "error");
        }
    }
    else if (mode == JAM::Mode::KEISAN)
    {
        uiPrint(15, 40, "hello");
    }
    else if (mode == JAM::Mode::KAKUNIN)
    {
        uiPrint(15, 40, "hello");
    }
    else if (mode == JAM::Mode::KAKIKOMU)
    {
        uiPrint(15, 40, "hello");
    }
    else if (mode == JAM::Mode::ATTACKER)
    {
        uiPrint(15, 40, "hello");
    }
}