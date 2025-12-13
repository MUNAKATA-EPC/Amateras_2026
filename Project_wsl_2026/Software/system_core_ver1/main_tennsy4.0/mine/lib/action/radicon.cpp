#include "radicon.hpp"

static Vector record_data[360][5];      // パワーと角度を5段階360分割で格納
static Vector temp_record_data[360][5]; // 一時的なデータ保管庫

static bool is_temp_record_data_empty = true; // 一時記録データがからかどうか

// record関連
int disToIndex(float dis);                                        // 距離を5段階に変換する
uint8_t complementData(Vector (&data)[360][5], bool (*range)[5]); // dataの補正　成功した距離段階を2進数で返す 例:段階0,1,3が成功なら0b11010
void record();                                                    // 記録

// replay関連
void replay();

void playRadicon(Radicon::Mode mode)
{
    if (mode == Radicon::Mode::RECORD)
    {
        record();
    }
    else if (mode == Radicon::COMPLEMENT)
    {
        motorsPdProcess(&pd_gyro, bnoDeg(), 0);
        motorsMove(0, 70);
    }
    else if (mode == Radicon::Mode::REPLAY)
    {
        replay();
    }
}

int disToIndex(float dis)
{
    if (dis < 350.0f)
        return 0;
    else if (dis < 450.0f)
        return 1;
    else if (dis < 550.0f)
        return 2;
    else if (dis < 650.0f)
        return 3;

    return 4;
}

uint8_t complementData(Vector (&data)[360][5], bool (*range)[5])
{
    bool is_data_lacking[5] = {false, false, false, false, false}; // データ不足かどうかのフラグ
    Vector current_data[360][5];                                   // 補正したベクトル保存用

    for (int j = 0; j < 5; j++) // current_dataの初期化・範囲外のdataをemptyにする
    {
        for (int i = 0; i < 360; i++)
        {
            if (range[i][j] == false)
                data[i][j] = Vector();

            current_data[i][j] = Vector();
        }
    }

    for (int j = 0; j < 5; j++) // 5回行う
    {
        for (int i = 0; i < 360; i++) // 0~359のチェック
        {
            if (range[i][j] == true) // 範囲内なら
            {
                // 無効なデータが来たら
                if (data[i][j].is_empty())
                {
                    // 正の方向の探索
                    int plus_found_index = (i + 1) % 360;
                    int plus_found_count = 1;

                    while (data[plus_found_index][j].is_empty())
                    {
                        plus_found_index = (plus_found_index + 1) % 360;
                        plus_found_count++;

                        if (plus_found_count > 360) // 一周回ってもデータがないならデータ不足
                        {
                            is_data_lacking[j] = true;
                            break;
                        }
                    }

                    if (is_data_lacking[j]) // データ不足なら抜ける
                        break;

                    // 負の方向の探索
                    int minus_found_index = (i - 1 + 360) % 360;
                    int minus_found_count = 1;

                    while (data[minus_found_index][j].is_empty())
                    {
                        minus_found_index = (minus_found_index - 1 + 360) % 360;
                        minus_found_count++;

                        if (minus_found_count > 360) // 一周回ってもデータがないならデータ不足
                        {
                            is_data_lacking[j] = true;
                            break;
                        }
                    }

                    if (is_data_lacking[j]) // データ不足なら抜ける
                        break;

                    // 値の算出　countの小さなベクトルほど影響力を持つようにしている
                    float weight_plus = 360.0f / (float)plus_found_count;   // プラス方向の重み
                    float weight_minus = 360.0f / (float)minus_found_count; // マイナス方向の重み

                    float total_weight = weight_plus + weight_minus; // 重みの合計

                    Vector plus_found_vec = data[plus_found_index][j];   // プラス方向のベクトル
                    Vector minus_found_vec = data[minus_found_index][j]; // マイナス方向のベクトル

                    // 合力 = (マイナス方向のベクトル * マイナス方向の重み) + (プラス方向のベクトル * プラス方向の重み)
                    Vector combine_found_vec = (minus_found_vec * weight_minus) + (plus_found_vec * weight_plus);

                    current_data[i][j] = combine_found_vec / total_weight; // 補間ベクトル = 重み付き合力 ÷ 合計重み
                }
            }
        }

        if (is_data_lacking[j] == false) // 補正したデータを代入
            for (int i = 0; i < 360; i++)
                if (current_data[i][j].is_empty() == false)
                    data[i][j] = current_data[i][j];
    }

    uint8_t output = 0b00000;
    for (int j = 0; j < 5; j++)
    {
        if (is_data_lacking[j] == false)
        {
            output |= 1 << (4 - j); // 左詰めで書く
        }
    }

    return output;
}

// ●ボタンを押している間は記録
// その後▲ボタンでデータ補正・record_dataに代入
// その後×ボタンでtemp_record_data削除
bool old_triangle_button = false; // 昔のps3の▲ボタン記録用

void record()
{
    motorsPdProcess(&pd_gyro, bnoDeg(), 0);

    /*ps3からの読み取り　移動方向の計算*/
    int move_deg = ps3RightStickDeg();
    int move_power = (int)constrain(80.0f * ps3RightStickDis() / 128.0f, 0.0f, 80.0f);
    if (!ps3RightStickDetected())
    {
        move_deg = 0xFF;
        move_power = 0xFF;
    }

    if (ps3ButtonIsPushing(ButtonDataType::CIRCLE))
    {
        /*データの記録*/
        if (move_deg != 0xFF && irDetected())
        {
            int deg_index = (irDeg() + 360) % 360; // 角度を0~359の配列用に変換
            int dis_index = disToIndex(irDis());   // 距離を0~4の配列用に変換

            temp_record_data[deg_index][dis_index] = getVec(move_deg, move_power); // 記録

            is_temp_record_data_empty = false;
        }
    }
    else if (ps3ButtonIsPushing(ButtonDataType::TRIANGLE) && old_triangle_button == false && is_temp_record_data_empty == false) // ▲ボタンが押された一回だけ起動
    {
        bool range[360][5]; // 範囲（共通で使う）

        /*temp_record_data補正*/
        Vector middle_vec[5] = {Vector(), Vector(), Vector(), Vector(), Vector()};
        for (int j = 0; j < 5; j++)
        { // 記録したデータの合力を出す
            for (int i = 0; i < 360; i++)
                if (temp_record_data[i][j].is_empty() == false)
                    middle_vec[j] = middle_vec[j] + temp_record_data[i][j];
            middle_vec[j] = middle_vec[j] * -1.0f; // 反転させる（emptyが多く分布した配列の中心番号になるはず）
        }

        // middle_vec[j].deg()から正方向と負方向に探索しdataがあるまでrange[i][j]をfalseにする
        for (int j = 0; j < 5; j++) // とりあえず全範囲を有効にする
            for (int i = 0; i < 360; i++)
                range[i][j] = true;

        for (int j = 0; j < 5; j++)
        {
            int middle_deg = (middle_vec[j].deg() + 360) % 360;
            bool find_data_flag[2] = {false, false}; // emptyならfalseにしていく
            int count = 0;
            while (find_data_flag[0] != true || find_data_flag[1] != true) // どちらも見つけるまで探索を続ける
            {
                int plus_index = (middle_deg + count + 1) % 360; // 正方向の探索
                if (temp_record_data[plus_index][j].is_empty())
                {
                    range[plus_index][j] = false;
                }
                else
                {
                    range[plus_index][j] = true;
                    find_data_flag[0] = true;
                }

                int minus_index = (middle_deg + count - 1 + 360) % 360; // 負方向の探索
                if (temp_record_data[minus_index][j].is_empty())
                {
                    range[minus_index][j] = false;
                }
                else
                {
                    range[minus_index][j] = true;
                    find_data_flag[1] = true;
                }

                if (count > 180) // 安全対策：180度以上探索しても見つからなかったら見つかったとしてループを抜ける
                {
                    find_data_flag[0] = true;
                    find_data_flag[1] = true;
                }

                count++;
            }
        }

        complementData(temp_record_data, range); // 補正する

        /*record_dataに書き込み*/
        for (int j = 0; j < 5; j++)
        {
            for (int i = 0; i < 360; i++)
            {
                if (temp_record_data[i][j].is_empty() == false)
                    record_data[i][j] = temp_record_data[i][j]; // 空データではないなら代入
            }
        }

        /*record_dataの補正*/
        for (int j = 0; j < 5; j++) // 範囲はすべての範囲
            for (int i = 0; i < 360; i++)
                range[i][j] = true;

        complementData(record_data, range);
    }
    else if (ps3ButtonIsPushing(ButtonDataType::CROSS))
    {
        /*temp_record_dataの内容を削除*/
        for (int j = 0; j < 5; j++)
            for (int i = 0; i < 360; i++)
                temp_record_data[i][j] = Vector();

        is_temp_record_data_empty = true;
    }

    /*モータ制御*/
    if (move_deg != 0xFF)
        motorsMove(move_deg, move_power);
    else
        motorsPdMove();

    old_triangle_button = ps3ButtonIsPushing(ButtonDataType::TRIANGLE); // 記録
}

// 記録したrecord_dataをもとに回り込みをする
void replay()
{
    if (irDetected())
    {
        int deg_index = (irDeg() + 360) % 360; // 角度を0~359の配列用に変換
        int dis_index = disToIndex(irDis());   // 距離を0~4の配列用に変換

        Vector move_vec = record_data[deg_index][dis_index];

        if (move_vec.is_empty()) // ベクターが空なら
        {
            fullColorLed1.rgbLightUp(10, 0, 0); // 異常なら赤色
            motorsStop();
        }
        else
        {
            fullColorLed1.rgbLightUp(0, 10, 10); // 正常なら青色
            motorsVectorMove(&move_vec);
        }
    }
    else
    {
        motorsStop();
    }
}