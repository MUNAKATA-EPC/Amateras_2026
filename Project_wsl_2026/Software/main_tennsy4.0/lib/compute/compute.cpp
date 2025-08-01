#include "compute.hpp"

// char型からint型に変換する関数
int char_to_int(const char *char_data, int start_index, int end_index)
{
    if (start_index < 0 || end_index < start_index) // 不正なデータ入力ではないか
    {
        return 0; // エラー
    }

    int copy_length = end_index - start_index + 1;

    // バッファサイズを限定
    if (copy_length > 10)
        copy_length = 10; // 適当に最大桁数を制限

    char char_data_temp[11]; // 最大10文字+終端

    memcpy(char_data_temp, &char_data[start_index], copy_length);
    char_data_temp[copy_length] = '\0';

    return atoi(char_data_temp);
}
