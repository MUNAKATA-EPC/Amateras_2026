#include "DSR1202.hpp"

/*DSR1202とシリアル通信し、DSR1202を制御する*/

HardwareSerial *_serial; // とりあえず定義
uint32_t _baudrate;           // ボートレート格納用

uint8_t DSR1202_motormove_toggle_pin = 0; // モータを動かすかどうかを決定するトグルスイッチのピン格納用

void DSR1202_set_motormove_togglepin(uint8_t pin)
{
    DSR1202_motormove_toggle_pin = pin;

    motormove_toggle.set_pin(DSR1202_motormove_toggle_pin); // トグルスイッチのピン番号を設定
}

void DSR1202_init(HardwareSerial *serial, uint32_t baudrate)
{
    motormove_toggle.init(); // トグルスイッチのピンを設定

    _serial = serial;
    _baudrate = baudrate;
    (*_serial).begin(_baudrate); // ボートレート定義
    (*_serial).setTimeout(50);   // 50msでタイムアウトとする
}

void DSR1202_break()
{
    (*_serial).println("1R0002R0003R0004R000"); // モータを停止させる
}

void DSR1202_move(uint8_t value_1ch, uint8_t value_2ch, uint8_t value_3ch, uint8_t value_4ch)
{
    /*トグルスイッチがオフならば*/

    if (!motormove_toggle.is_turn_on())
    {
        DSR1202_break(); // モータを停止させる
        return;          // 終わり
    }

    /*トグルスイッチがオンならそれぞれのチャンネルを動かす*/

    // 1chについて
    value_1ch = constrain(value_1ch, -100, 100);       // -100~100の範囲に収める
    String str_abs_value_1ch = String(abs(value_1ch)); // 符号を除いた値をString型に変換
    if (abs(value_1ch) < 10)                           // もし値が1桁なら
        str_abs_value_1ch = "00" + str_abs_value_1ch;
    else if (abs(value_1ch) < 100) // もし値が2桁なら
        str_abs_value_1ch = "0" + str_abs_value_1ch;

    String data_value_1ch;                         // 最終的なデータを格納
    if (value_1ch >= 0)                            // 0または値が正ならば
        data_value_1ch = "1F" + str_abs_value_1ch; // 1chで正回転なので1F
    else
        data_value_1ch = "1R" + str_abs_value_1ch; // 1chで逆回転なので1R

    // 2chについて
    value_2ch = constrain(value_2ch, -100, 100);       // -100~100の範囲に収める
    String str_abs_value_2ch = String(abs(value_2ch)); // 符号を除いた値をString型に変換
    if (abs(value_2ch) < 10)                           // もし値が1桁なら
        str_abs_value_2ch = "00" + str_abs_value_2ch;
    else if (abs(value_2ch) < 100) // もし値が2桁なら
        str_abs_value_2ch = "0" + str_abs_value_2ch;

    String data_value_2ch;                         // 最終的なデータを格納
    if (value_2ch >= 0)                            // 0または値が正ならば
        data_value_2ch = "2F" + str_abs_value_2ch; // 2chで正回転なので2F
    else
        data_value_2ch = "2R" + str_abs_value_2ch; // 2chで逆回転なので2R

    // 3chについて
    value_3ch = constrain(value_3ch, -100, 100);       // -100~100の範囲に収める
    String str_abs_value_3ch = String(abs(value_3ch)); // 符号を除いた値をString型に変換
    if (abs(value_3ch) < 10)                           // もし値が1桁なら
        str_abs_value_3ch = "00" + str_abs_value_3ch;
    else if (abs(value_3ch) < 100) // もし値が2桁なら
        str_abs_value_3ch = "0" + str_abs_value_3ch;

    String data_value_3ch;                         // 最終的なデータを格納
    if (value_3ch >= 0)                            // 0または値が正ならば
        data_value_3ch = "3F" + str_abs_value_3ch; // 3chで正回転なので3F
    else
        data_value_3ch = "3R" + str_abs_value_3ch; // 3chで逆回転なので3R

    // 4chについて
    value_4ch = constrain(value_4ch, -100, 100);       // -100~100の範囲に収める
    String str_abs_value_4ch = String(abs(value_4ch)); // 符号を除いた値をString型に変換
    if (abs(value_4ch) < 10)                           // もし値が1桁なら
        str_abs_value_4ch = "00" + str_abs_value_4ch;
    else if (abs(value_4ch) < 100) // もし値が2桁なら
        str_abs_value_4ch = "0" + str_abs_value_4ch;

    String data_value_4ch;                         // 最終的なデータを格納
    if (value_4ch >= 0)                            // 0または値が正ならば
        data_value_4ch = "4F" + str_abs_value_4ch; // 4chで正回転なので4F
    else
        data_value_4ch = "4R" + str_abs_value_4ch; // 4chで逆回転なので4R

    (*_serial).println(data_value_1ch + data_value_2ch + data_value_3ch + data_value_4ch); // 最終的に\0付きで送信
}