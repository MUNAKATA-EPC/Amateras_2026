#include <Arduino.h>

// action類
#include "attacker.hpp"
#include "defender.hpp"
#include "radicon.hpp"

// common類
#include "angleComputer.hpp"
#include "timer.hpp"
#include "vector.hpp"
// driver類
#include "ui/ui.hpp"
#include "dribbler/dribbler.hpp"
#include "kicker/kicker.hpp"
#include "motors/motors.hpp"
// sensor類
#include "bno.hpp"
#include "button.hpp"
#include "digitalSensor.hpp"
#include "ir.hpp"
#include "line.hpp"
#include "m5stamp.hpp"
#include "openmv.hpp"

void setup()
{
    // I2C
    uiInit(&Wire1, 0x3C, 128, 64);
    bnoInit(&Wire, 0x28);

    // シリアル
    irInit(&Serial1, 115200, 0xAA);
    lineInit(&Serial5, 115200, 0xAA);
    openmvInit(&Serial3, 115200, 0xAA);

    motorsInit(&Serial1, 115200);
    motorsSetMoveSign(1, -1, -1, 1);         // 移動のための符号をセット
    motorsSetPdSign(1, 1, 1, 1);             // PD制御のための符号をセット
    motorsSetDegPosition(315, 45, 225, 135); // モータの位置をセット

    // デジタル
    dribbler1.init(13, 1000, 2000);
    while (!dribbler1.available()) // ドリブラーの初期化が完了していなかったら初期化
        dribbler1.setup();

    kicker1.init(2, 3, 700);
    catchSensor.init(6); // キッカー用

    enterButton.init(11, INPUT_PULLDOWN); // ui用
    rightButton.init(12, INPUT_PULLDOWN); // ui用
    leftButton.init(10, INPUT_PULLDOWN);  // ui用
    resetButton.init(9, INPUT_PULLDOWN);  // bno用
}

Timer timer; // ui用

void loop()
{
    // ドリブラーを動かす
    dribbler1.move(1500);

    // uiを実行
    if (!timer.everCalled() || timer.msTime() > 10) // まだ呼ばれていない場合もタイマーをリセットさせる
    {
        timer.reset();
        bool show = !uiRunning();                                                                     // runningなら表示しない
        uiProcess(show, enterButton.isReleased(), rightButton.isReleased(), leftButton.isReleased()); // 10msに一回更新
    }

    // 動作を実行
    if (!uiRunning())
    {
        Serial.println("now sellecting...");
        kicker1.kick(false); // キッカーを動かさない
        motorsStop();        // 動作選択中はモータを止める
    }
    else
    {
        // playPrint(PrintMode::LINE);
        switch (uiActionNumber())
        {
        case Action::ATTACKER:
            playAttacker();
            break;
        case Action::DEFENDER:
            playDefender();
            break;
        case Action::TEST:
            break;
        case Action::RADICON:
            playRadicon();
            break;
        }
    }
}