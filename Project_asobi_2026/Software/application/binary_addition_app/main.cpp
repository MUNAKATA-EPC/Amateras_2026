#include <stdio.h>   // プリント関連
#include <windows.h> // これを追加

// 計算時の数値格納用
int first_number = 0;
int second_number = 0;
int result_number = 0;
// スペースを何個入れるか格納用
int space_count;

// 足し算をする関数（負が含まれる計算はできないはずなのでそこはご愛敬）
int compute_addition(int A, int B)
{
    int need_carry_bits = A & B;    // 繰り上げが必要なビット格納用
    int no_need_carry_bits = A ^ B; // 繰り上げの必要ないビット格納用
    int output = 0;                 // 最終的な計算結果格納用

    do
    {
        output = (need_carry_bits << 1) ^ no_need_carry_bits;          // 繰り上げつまり左に1ビット分シフトするということ
        need_carry_bits = (need_carry_bits << 1) & no_need_carry_bits; // これで同じビットにふたつ1があったらもう一度繰り上げ処理がいる
        no_need_carry_bits = output;                                   // 繰り上げの必要のないビットたち
    } while (need_carry_bits != 0); // 繰り上げが必要なら繰り返す（負に対応するため!=にしている）

    return output; // 演算結果を返す
}

// PCから数字を獲得する関数
int get_enter_number()
{
    int output_number;
    bool is_entered_number = false;

    while (!is_entered_number)
    {
        fflush(stdout);
        if (scanf("%d", &output_number) == true)
        {
            // 正しく入力された
            is_entered_number = true;
        }
        else
        {
            // 正しく入力されなかったら再トライ
            int buf;
            while ((buf = getchar()) != '\n' && buf != EOF)
            {
            } // バッファクリア
        }
    }

    return output_number;
}

// 指定した文字を指定した個数指定した周期で置く関数
void printf_loop_put(const char *p, int put_number, int loop_ms)
{
    for (int i = 0; i < put_number; i++)
    {
        fflush(stdout);
        printf("%s", p);
        Sleep(loop_ms);
    }
}

// 数字を2進数で表示する関数（最後改行あり）
void printf_binary(int number)
{
    int started = 0;
    for (int i = sizeof(int) * 8 - 1; i >= 0; i--)
    {
        int bit = (number >> i) & 1;
        if (bit)
            started = 1;
        if (started)
            printf("%d", bit);
    }
    if (!started)
        printf("0");
    printf(" (2)\n");
}

// 指定した個数スペースを空ける関数
void printf_space(int put_number)
{
    for (int i = 0; i < put_number; i++)
    {
        printf(" ");
    }
}

// メインループ
int main(void)
{
    space_count = 0; // 最初は0個

    // アプリ起動アニメーション実行
    printf("* -- Addition Calculation Apps -- *");
    printf("\n");

    Sleep(1000);

    int select_number = 0;
    bool continue_flag = true;
    bool restart_flag = true;
    do
    {
        if (restart_flag)
        {
            // 加算で使う一つ目の値入力
            printf_space(space_count);
            printf("First number  : ");
            first_number = get_enter_number();
            printf_space(space_count);
            printf("-> ");
            printf_binary(first_number);
        }
        else if (continue_flag)
        {
            // continueなので昔のresult_numberがfirst_numberとなる
            printf_space(space_count);
            printf("First number  : %d (Old result number)", result_number);
            printf("\n");
            printf_space(space_count);
            printf("-> ");
            printf_binary(result_number);
        }
        else
        {
            return 0; // ありえない場合だが一応
        }

        // 加算で使う二つ目の値入力
        printf_space(space_count);
        printf("Second number : ");
        second_number = get_enter_number();
        printf_space(space_count);
        printf("-> ");
        printf_binary(second_number);

        Sleep(1000);

        // 計算
        result_number = compute_addition(first_number, second_number); // 2進数によって計算
        printf("\n");
        printf_space(space_count);
        printf("[ %d + %d = %d ]", first_number, second_number, result_number);
        printf("\n\n");

        Sleep(1000);

        // 計算結果を表示
        printf_space(space_count);
        printf("Result number : %d\n", result_number);
        printf_space(space_count);
        printf("-> ");
        printf_binary(result_number);

        Sleep(1000);
        space_count += 4; // 4個空ける

        // 計算を続けるかどうか
        printf("\n");
        printf_loop_put("=", space_count, 100);
        printf("Select number(continue:0,restart:1,close app:2) : ");
        select_number = (abs(get_enter_number()) + 3) % 3; // 強制的に0~3になる
        printf_space(space_count);
        printf("-> ");
        printf_binary(select_number);

        Sleep(1000);

        // 選ばれた番号の動きをする
        printf("\n");
        switch (select_number)
        {
        case 0:
            printf_space(space_count);
            printf("[ continue ]");
            first_number = result_number;

            continue_flag = true;
            restart_flag = false;
            break;

        case 1:
            printf_space(space_count);
            printf("[ restart ]");

            continue_flag = false;
            restart_flag = true;
            break;

        case 2:
            printf_space(space_count);
            printf("[ close app ]");

            continue_flag = false;
            restart_flag = false;
            break;
        }
        printf("\n\n");

        Sleep(1000); // 1000ms待機
    } while (continue_flag || restart_flag);

    return 0;
}