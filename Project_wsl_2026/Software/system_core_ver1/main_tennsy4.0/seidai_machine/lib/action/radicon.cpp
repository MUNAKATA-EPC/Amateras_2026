#include "radicon.hpp"
static PD pdGyro(0.8f, 0.1f); // ジャイロ用のPD調節値
static PD pdCam(0.6f, 0.1f);  // カメラ用のPD調節値

#define MOVE_DEG_INDEX 0
#define MOVE_POWER_INDEX 1
static int data[360][2]; // 角度と距離で360*5段階で記録　移動角度とパワー記録用配列

/*　ボールの位置によってロボットの進むべき方向(ステックの角度)とパワーを記録する　*/
/*　ボールがあって◎ボタンが押されている：データ記録　*/
/*　×ボタンが押される：データ削除　*/
void radiconRecord()
{
}

/*　補正成功：LEDが青緑色に光る　*/
/*　補正失敗：LEDが赤色に光る　　*/
void radiconComplement()
{
    bool is_data_lacking = false; // データ不足かどうかのフラグ
    int current_data[360][2];     // 補正した角度保存用
    for (int i = 0; i < 360; i++)
    {
        current_data[i][MOVE_DEG_INDEX] = 0xFF;
        current_data[i][MOVE_POWER_INDEX] = 0xFF;
    }

    for (int i = 0; i < 360; i++)
    {
        // 無効なデータが来たら
        if (data[i][MOVE_DEG_INDEX] == 0xFF)
        {
            // 正の方向の探索
            int plus_found_index = (i + 1) % 360;
            int plus_found_count = 1;

            while (data[plus_found_index][MOVE_DEG_INDEX] == 0xFF)
            {
                plus_found_index = (plus_found_index + 1) % 360;
                plus_found_count++;

                if (plus_found_count > 90) // 90度以上離れていたらデータ不足
                {
                    is_data_lacking = true;
                    break;
                }
            }

            if (is_data_lacking) // データ不足なら抜ける
                break;

            // 負の方向の探索
            int minus_found_index = (i - 1 + 360) % 360;
            int minus_found_count = 1;

            while (data[minus_found_index][MOVE_DEG_INDEX] == 0xFF)
            {
                minus_found_index = (minus_found_index - 1 + 360) % 360;
                minus_found_count++;

                if (minus_found_count > 90) // 90度以上離れていたらデータ不足
                {
                    is_data_lacking = true;
                    break;
                }
            }

            if (is_data_lacking) // データ不足なら抜ける
                break;

            // 重み（距離）算出 (countの小さなデータほど大きな影響を持つようにcountの逆数を重みとする)
            float plus_found_weight = 360.0f / plus_found_count;
            float minus_found_weight = 360.0f / minus_found_count;

            // 補正角度を合力で算出
            Vector plus_found_vec(data[plus_found_index][MOVE_DEG_INDEX], plus_found_weight);
            Vector minus_found_vec(data[minus_found_index][MOVE_DEG_INDEX], minus_found_weight);
            Vector current_vec = plus_found_vec + minus_found_vec;
            // パワーを重みで求める
            float current_power = (plus_found_weight * data[plus_found_index][MOVE_POWER_INDEX] + minus_found_weight * data[minus_found_index][MOVE_POWER_INDEX]) / (plus_found_weight + minus_found_weight);

            // 代入
            current_data[i][MOVE_DEG_INDEX] = current_vec.deg();
            current_data[i][MOVE_POWER_INDEX] = (int)current_power;
        }
    }

    if (is_data_lacking)
    {
        fullColorLed1.rgbLightUp(10, 0, 0); // 警告
        Serial.print("lack_of_data");
    }
    else
    {
        fullColorLed1.rgbLightUp(0, 10, 10); // 補正成功
        Serial.print("success_complement");

        for (int i = 0; i < 360; i++)
        {
            if (current_data[i][MOVE_DEG_INDEX] != 0xFF)
            {
                data[i][MOVE_DEG_INDEX] = current_data[i][MOVE_DEG_INDEX];     // 補正したデータを代入
                data[i][MOVE_POWER_INDEX] = current_data[i][MOVE_POWER_INDEX]; // 補正したデータを代入
            }
        }
    }
}

/*　RecordとComplementを動かしてから行うこと　*/
/*　読み取り成功：LEDが青緑色に光る　*/
/*　読み取り失敗：LEDが赤色に光る　　*/
void radiconReplay()
{
    motorsPdProcess(&pdGyro, bnoDeg(), 0); // PD成分計算

    if (irDetected()) // ボールが見えたら読み込み処理
    {
        int index = (irDeg() + 360) % 360; // 0~359度に変換
        int move_deg = data[index][MOVE_DEG_INDEX];
        int move_power = data[index][MOVE_POWER_INDEX];

        if (move_deg == 0xFF)
        {
            fullColorLed1.rgbLightUp(10, 0, 0); // 警告
            motorsPdMove();
        }
        else
        {
            fullColorLed1.rgbLightUp(0, 10, 10); // 読み取り成功
            motorsMove(move_deg, move_power);
        }
    }
    else
    {
        motorsPdMove();
    }
}

static bool first_call_flag = false;

void playRadicon()
{
    Radicon::Mode mode = (Radicon::Mode)uiModeNumber();

    if (first_call_flag == false) // dataの初期化
    {
        for (int i = 0; i < 360; i++)
        {
            data[i][MOVE_DEG_INDEX] = 0xFF;
            data[i][MOVE_POWER_INDEX] = 0xFF;
        }

        first_call_flag = true;
    }

    switch (mode)
    {
    case Radicon::Mode::RECORD:
        radiconRecord();
        break;
    case Radicon::Mode::COMPLEMENT:
        radiconComplement();
        break;
    case Radicon::Mode::REPLAY:
        radiconReplay();
        break;
    default:
        motorsStop();
        break;
    }
}