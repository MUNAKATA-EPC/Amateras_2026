#include "toggle.hpp"

/*トグルスイッチが使いやすくなる関数*/

/*トグルを定義*/
Toggle motormove_toggle;  // モータを動かす選択用
Toggle goal_select_toggle; // ゴール選択用

void Toggle::set_pin(int pin)
{
    _toggle_pin = pin;
}

void Toggle::init()
{
    pinMode(_toggle_pin, INPUT); // ピンを設定する
}

bool Toggle::is_turn_on()
{
    return digitalRead(_toggle_pin); // トグルスイッチの状況を返す
}