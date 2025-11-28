#include "radicon.hpp"
static PD pdGyro(0.8f, 0.1f); // ジャイロ用のPD調節値
static PD pdCam(0.6f, 0.1f);  // カメラ用のPD調節値

static int record[360][10] = {0xFF}; // 0~359度、距離10段階の記録用配列

void radiconRecord()
{
    // キッカー
    bool on = (catchSensor.read() == HIGH) || ps3ButtonIsPushing(ButtonDataType::L1) || ps3ButtonIsPushing(ButtonDataType::R1);
    kicker1.kick(on);

    // PD制御
    int pdDeg = bnoDeg();
    if (ps3RightStickDetected())
    {
        motorsPdProcess(&pdGyro, pdDeg, ps3RightStickDeg()); // PD成分計算(右ステック補正付き)
    }
    else
    {
        motorsPdProcess(&pdGyro, pdDeg, 0); // PD成分計算
    }

    // パワー
    int power = 85;

    // 制御
    if (ps3LeftStickDetected())
    {
        int moveDeg = normalizeDeg(ps3LeftStickDeg() + bnoDeg()); // 移動方向
        float scale = ps3LeftStickDis() / 128.0f;                 // 速度のスケール計算

        motorsMove(moveDeg, (int)power * scale);

        // 記録処理
        if (irDetected())
        {
            int ir_deg_record_index = (irDeg() + 360) % 360;                        // 0~359度に変換
            int ir_dis_record_index = static_cast<int>(irDis() * 100.0f / 1023.0f); // 距離を10段階にする

            record[ir_deg_record_index][ir_dis_record_index] = moveDeg; // 記録
        }
    }
    else
    {
        motorsPdMove();
    }
}

// 角度インデックスを 0-359 の範囲に正規化
int wrapArrayIndex(int index)
{
    const int SIZE = 360; // DEG_MAXの代わりに360を使用
    // C++の%演算子は負の値に対して直感的でないため、調整
    int mod = index % SIZE;
    if (mod < 0)
    {
        mod += SIZE;
    }
    return mod;
}

const int DEG_MAX = 360;
const int DIS_MAX = 10;
const int UNRECORDED_VALUE = 0xFF;

void radiconComplement()
{
    // 角度インデックスの探索順序（0, 359, 1, 358, ...）のためのループカウンター
    for (int i = 0; i < DEG_MAX; ++i)
    {

        // 中心から外側への探索順序を計算
        int current_deg_index;
        if (i % 2 == 0)
        {
            current_deg_index = i / 2;
        }
        else
        {
            current_deg_index = DEG_MAX - (i / 2) - 1;
        }

        // --- Step 1: 同じ角度 (current_deg_index) 内で補間を試みる (水平補間) ---

        // まず、この角度のいずれかの距離に有効なデータがあるかチェック
        bool all_dis_unrecorded = true;
        for (int y = 0; y < DIS_MAX; ++y)
        {
            if (record[current_deg_index][y] != UNRECORDED_VALUE)
            {
                all_dis_unrecorded = false;
                break;
            }
        }

        if (!all_dis_unrecorded)
        {
            // 同じ角度内に有効なデータがある場合、それを使い未記録セルを補間
            for (int y = 0; y < DIS_MAX; ++y)
            {
                if (record[current_deg_index][y] == UNRECORDED_VALUE)
                {
                    // 最も近い距離にある有効なデータを探す (最近傍法)
                    int nearest_valid_val = UNRECORDED_VALUE;
                    int min_dis_diff = DIS_MAX; // 最大の差より大きく設定

                    for (int y_near = 0; y_near < DIS_MAX; ++y_near)
                    {
                        if (record[current_deg_index][y_near] != UNRECORDED_VALUE)
                        {
                            int dis_diff = abs(y - y_near); // Arduinoでは標準でabs()が利用可能
                            if (dis_diff < min_dis_diff)
                            {
                                min_dis_diff = dis_diff;
                                nearest_valid_val = record[current_deg_index][y_near];
                            }
                            if (min_dis_diff == 0)
                                break;
                        }
                    }

                    if (nearest_valid_val != UNRECORDED_VALUE)
                    {
                        record[current_deg_index][y] = nearest_valid_val;
                    }
                }
            }
        }

        // --- Step 2: 垂直補間（同じ角度の全ての距離が未記録の場合） ---

        // Step 1の後、再度この角度の全データが0xFFか確認
        // Step 1で補間された場合は、all_dis_unrecordedはfalseになる
        all_dis_unrecorded = true;
        for (int y = 0; y < DIS_MAX; ++y)
        {
            if (record[current_deg_index][y] != UNRECORDED_VALUE)
            {
                all_dis_unrecorded = false;
                break;
            }
        }

        if (all_dis_unrecorded)
        {
            // 近隣の角度を探索 (x+1, x-1, x+2, x-2, ...)
            for (int offset = 1; offset < DEG_MAX / 2 + 1; ++offset)
            {
                int near_deg_p = wrapArrayIndex(current_deg_index + offset);
                int near_deg_m = wrapArrayIndex(current_deg_index - offset);

                bool found = false;

                // +offset の角度をチェック
                for (int y = 0; y < DIS_MAX; ++y)
                {
                    if (record[near_deg_p][y] != UNRECORDED_VALUE)
                    {
                        // 有効なデータが見つかった場合、その角度の有効データで現在の角度の未記録セルを埋める
                        for (int fill_y = 0; fill_y < DIS_MAX; ++fill_y)
                        {
                            // 近隣の角度の有効データをコピー
                            if (record[near_deg_p][fill_y] != UNRECORDED_VALUE)
                            {
                                record[current_deg_index][fill_y] = record[near_deg_p][fill_y];
                            }
                            // 近隣角度で最初に有効データが見つかった場所[y]の値で、
                            // 自身の未記録の距離も埋める (このロジックは前回と同じものを維持)
                            else if (record[current_deg_index][fill_y] == UNRECORDED_VALUE)
                            {
                                record[current_deg_index][fill_y] = record[near_deg_p][y];
                            }
                        }
                        found = true;
                        break;
                    }
                }

                if (found)
                    break;

                // -offset の角度をチェック（+offsetで見つからなかった場合のみ）
                for (int y = 0; y < DIS_MAX; ++y)
                {
                    if (record[near_deg_m][y] != UNRECORDED_VALUE)
                    {
                        // 有効なデータが見つかった場合、その角度の有効データで現在の角度の未記録セルを埋める
                        for (int fill_y = 0; fill_y < DIS_MAX; ++fill_y)
                        {
                            if (record[near_deg_m][fill_y] != UNRECORDED_VALUE)
                            {
                                record[current_deg_index][fill_y] = record[near_deg_m][fill_y];
                            }
                            else if (record[current_deg_index][fill_y] == UNRECORDED_VALUE)
                            {
                                record[current_deg_index][fill_y] = record[near_deg_m][y];
                            }
                        }
                        found = true;
                        break;
                    }
                }

                if (found)
                    break;
            }
        }
    }
}

void radiconReplay()
{
    // ここに再生用コードを書く
}



void playRadicon()
{
    Radicon::Mode mode = (Radicon::Mode)uiModeNumber();

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