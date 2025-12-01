#include "radicon.hpp"
static PD pdGyro(0.8f, 0.1f); // ジャイロ用のPD調節値

static Vector data[360][5];      // パワーと角度を5段階360分割で格納
static Vector temp_data[360][5]; // 一時的なデータ保管庫

bool is_temp_data_empty = true;

/*距離を5段階に変換する*/
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

/*temp_dataの補正　temp_dataが極端に少ない場合0b10000を返し、成功した距離段階を2進数で返す 例:段階0,1,3が成功➡0b0110100*/
uint8_t complement_temp_data()
{
    Vector combined_vec[5] = {Vector()}; // 全有効データの合力
    int search_start_deg[5];             // 合力の反対方向 (最もデータが少ない方向)

    // ベクトルの合計と検索開始点の特定
    for (int j = 0; j < 5; j++)
    {
        for (int i = 0; i < 360; i++)
            if (temp_data[i][j].is_empty() == false)
                combined_vec[j] = combined_vec[j] + temp_data[i][j];

        if (combined_vec[j].is_empty())
            // データが一つもない場合、-1 を設定してこの段階の処理をスキップ
            search_start_deg[j] = -1;
        else
            // 合力の反対側を、最もデータが少ない（切れ目の中心に近い）方向と見なす
            search_start_deg[j] = (combined_vec[j].deg() + 180) % 360;
    }

    // 全て空の場合、エラーコードを返す
    bool all_empty = true;
    for (int j = 0; j < 5; j++)
    {
        if (search_start_deg[j] != -1)
        {
            all_empty = false;
            break;
        }
    }
    if (all_empty)
        return 0b10000;

    int extra1[5]; // 反時計回り方向（+）の先端
    int extra2[5]; // 時計回り方向（-）の先端

    // 先端の特定 (extra1, extra2)
    for (int j = 0; j < 5; j++)
    {
        if (search_start_deg[j] == -1)
            continue;

        // 探索カウント　360を超えたらデータ不足でエラーとする（制約解除後の安全措置）
        int count;

        // プラス方向の先端 (extra1) を探索
        int current_i = search_start_deg[j];
        count = 0;

        // 有効なデータが見つかるまで探索を続ける (最大360回)
        while (temp_data[current_i][j].is_empty())
        {
            current_i = (current_i + 1) % 360;
            count++;

            // 一周回ってしまったら、データが極端に少ない（1個しか見つからないなど）
            if (count > 360)
            {
                return 0b10000;
            }
        }
        extra1[j] = current_i; // 見つかった有効データが先端1 (補間開始点)

        // マイナス方向の先端 (extra2) を探索
        current_i = search_start_deg[j];
        count = 0;

        // 有効なデータが見つかるまで探索を続ける (最大360回)
        while (temp_data[current_i][j].is_empty())
        {
            current_i = (current_i - 1 + 360) % 360;
            count++;

            // 一周回ってしまったら、データが極端に少ない
            if (count > 360)
            {
                return 0b10000;
            }
        }
        extra2[j] = current_i; // 見つかった有効データが先端2 (補間終了点)
    }

    // 補間実行
    bool is_temp_data_lacking[5] = {false};
    Vector current_temp_data[360][5];
    for (int i = 0; i < 360; i++)
        for (int j = 0; j < 5; j++)
            current_temp_data[i][j] = temp_data[i][j]; // 元のデータをコピー

    for (int j = 0; j < 5; j++)
    {
        if (search_start_deg[j] == -1)
        {
            is_temp_data_lacking[j] = true;
            continue;
        }

        // 補間ループの開始点と終了点を設定
        // extra1 から始まり、反時計回り（+）に進んで extra2 で終わるループ。
        // これにより、最も大きな切れ目（extra2からextra1への間）を避ける。

        int start_i = extra1[j];
        int end_i = extra2[j];

        // 補間範囲のループ。start_iから始まり、end_iまで（end_iを含む）
        int current_i = start_i;
        do
        {
            // 補間対象のセルが空で、かつそれが有効なデータの先端でない場合のみ補間
            if (current_temp_data[current_i][j].is_empty())
            {
                // 正の方向の探索
                int plus_found_index = (current_i + 1) % 360;
                int plus_found_count = 1;

                while (current_temp_data[plus_found_index][j].is_empty())
                {
                    plus_found_index = (plus_found_index + 1) % 360;
                    plus_found_count++;

                    // 【境界チェック】探索が補間範囲の終端 (end_i) を超えたら中止し、補間しない
                    // 1個のデータしかなく extra1 == extra2 の場合は、このチェックでスキップされる
                    if (plus_found_index == end_i)
                    {
                        goto next_step;
                    }
                }

                // 負の方向の探索
                int minus_found_index = (current_i - 1 + 360) % 360;
                int minus_found_count = 1;

                while (current_temp_data[minus_found_index][j].is_empty())
                {
                    minus_found_index = (minus_found_index - 1 + 360) % 360;
                    minus_found_count++;

                    // 境界チェック　探索が補間範囲の始端 (start_i) を超えたら中止し、補間しない
                    if (minus_found_index == start_i)
                    {
                        goto next_step;
                    }
                }

                // 値の算出 (重み付きベクトル補間)
                float total_weight = (float)plus_found_count + minus_found_count;
                Vector plus_found_vec = current_temp_data[plus_found_index][j];
                Vector minus_found_vec = current_temp_data[minus_found_index][j];

                Vector combine_found_vec = (minus_found_vec * (float)plus_found_count) + (plus_found_vec * (float)minus_found_count);

                current_temp_data[current_i][j] = combine_found_vec / total_weight;
            }

        next_step:;
            current_i = (current_i + 1) % 360;

        } while (current_i != (end_i + 1) % 360); // end_iを通過するまでループ

        // 書き写し
        if (is_temp_data_lacking[j] == false)
        {
            for (int i = 0; i < 360; i++)
                if (!current_temp_data[i][j].is_empty())
                    temp_data[i][j] = current_temp_data[i][j];
        }

        // is_temp_data_lacking[j] はここでは設定されないため、このラベルは不要だが、
        // 念のため前のロジックの意図（エラーフラグを立てた場合のスキップ）を考慮し、
        // is_temp_data_lacking[j] の設定を削除した上で、継続。
    }

    // フェーズ 5: バイナリ出力
    uint8_t output = 0;
    bool any_success = false;
    for (int j = 0; j < 5; j++)
    {
        if (search_start_deg[j] != -1) // データが全く無かった段階(-1)は失敗と見なす
        {
            output |= (1 << (4 - j)); // 成功した段階のビットを1にセット
            any_success = true;
        }
    }

    if (!any_success)
        return 0b10000;

    return output;
}

/*dataの補正　成功した距離段階を2進数で返す 例:段階0,1,3が成功➡0b110100*/
uint8_t complement_data()
{
    bool is_data_lacking[5] = {false, false, false, false, false}; // データ不足かどうかのフラグ
    Vector current_data[360][5];                                   // 補正したベクトル保存用
    for (int i = 0; i < 360; i++)
        for (int j = 0; j < 5; j++)
            current_data[i][j] = Vector();

    for (int j = 0; j < 5; j++) // 5回行う
    {
        for (int i = 0; i < 360; i++) // 0~359のチェック
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

                    if (plus_found_count > 90) // 90度以上離れていたらデータ不足
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

                    if (minus_found_count > 90) // 90度以上離れていたらデータ不足
                    {
                        is_data_lacking[j] = true;
                        break;
                    }
                }

                if (is_data_lacking[j]) // データ不足なら抜ける
                    break;

                // 値の算出
                float total_weight = (float)plus_found_count + minus_found_count; // 重みの合計

                Vector plus_found_vec = data[plus_found_index][j];   // プラス方向のベクト
                Vector minus_found_vec = data[minus_found_index][j]; // マイナス方向のベクトル

                Vector combine_found_vec = (minus_found_vec * (float)plus_found_count) + (plus_found_vec * (float)minus_found_count); // 合力

                current_data[i][j] = combine_found_vec / total_weight; // 補間ベクトル = 重み付き合力÷合計重み
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

/*●ボタンを押している間は記録*/
/*➡その後▲ボタンでデータ補正・dataに代入*/
/*➡その後×ボタンでtemp_data削除*/
void record()
{
    motorsPdProcess(&pdGyro, bnoDeg(), 0);

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

            temp_data[deg_index][dis_index] = getVec(move_deg, move_power); // 記録

            is_temp_data_empty = false;
        }
    }
    else if (ps3ButtonIsPushing(ButtonDataType::TRIANGLE) && is_temp_data_empty == false)
    {
        /*temp_data補正*/
        complement_temp_data();

        /*dataに書き込み*/
        for (int i = 0; i < 360; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                if (temp_data[i][j].is_empty() == false)
                    data[i][j] = temp_data[i][j]; // 空データではないなら代入
            }
        }

        /*dataの補正*/
        complement_data();
    }
    else if (ps3ButtonIsPushing(ButtonDataType::CROSS))
    {
        /*temp_dataの内容を削除*/
        for (int i = 0; i < 360; i++)
            for (int j = 0; j < 5; j++)
                temp_data[i][j] = Vector();

        is_temp_data_empty = true;
    }

    /*モータ制御*/
    if (move_deg != 0xFF)
        motorsMove(move_deg, move_power);
    else
        motorsPdMove();
}

/*記録したdataをもとに回り込みをする*/
void replay()
{
    if (irDetected())
    {
        int deg_index = (irDeg() + 360) % 360; // 角度を0~359の配列用に変換
        int dis_index = disToIndex(irDis());   // 距離を0~4の配列用に変換

        Vector move_vec = data[deg_index][dis_index];

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

static bool first_call_flag = false; // 配列初期化用

void playRadicon()
{
    Radicon::Mode mode = (Radicon::Mode)uiModeNumber();

    if (first_call_flag == false) // data・temp_dataの初期化
    {
        for (int i = 0; i < 360; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                data[i][j] = Vector();
                temp_data[i][j] = Vector();
            }
        }

        first_call_flag = true;
    }

    switch (mode)
    {
    case Radicon::Mode::RECORD:
        record();
        break;
    case Radicon::Mode::REPLAY:
        replay();
        break;
    default:
        motorsStop();
        break;
    }
}