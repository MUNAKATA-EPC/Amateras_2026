#include <Arduino.h>

// action類
#include "attacker.hpp"
#include "defender.hpp"
#include "radicon.hpp"

// common類
#include "angleHelper.hpp"
#include "timer.hpp"
#include "vector.hpp"
// driver類
#include "ui.hpp"
#include "dribbler.hpp"
#include "kicker.hpp"
#include "motors.hpp"
// sensor類
#include "bno.hpp"
#include "button.hpp"
#include "digitalSensor.hpp"
#include "ir.hpp"
#include "line.hpp"
#include "ps3.hpp"
#include "openmv.hpp"

void setup()
{
    String debugMessage = "<setup>\n";

    // I2C
    debugMessage += uiInit(&Wire1, 0x3C, 128, 64) ? "ui     : found\n" : "ui     : not found\n";
    debugMessage += bnoInit(&Wire, 0x28) ? "bno    : found\n" : "bno    : not found\n";

    // シリアル
    Serial.begin(9600); // デバッグ用

    debugMessage += irInit(&Serial1, 115200, 0xAA) ? "ir     : found\n" : "ir     : not found\n";
    debugMessage += lineInit(&Serial5, 115200, 0xAA) ? "line   : found\n" : "line   : not found\n";
    debugMessage += openmvInit(&Serial3, 115200, 0xAA) ? "openmv : found\n" : "openmv : not found\n";
    debugMessage += ps3Init(&Serial2, 115200, 0xAA) ? "ps3    : found\n" : "ps3    : not found\n";

    debugMessage += motorsInit(&Serial1, 115200) ? "motors : found\n" : "motors : not found\n";
    motorsSetMoveSign(1, -1, -1, 1);         // 移動のための符号をセット
    motorsSetPdSign(1, 1, 1, 1);             // PD制御のための符号をセット
    motorsSetDegPosition(315, 45, 225, 135); // モータの位置をセット

    // デジタル
    kicker1.init(2, 3, 700);
    catchSensor.init(6); // キッカー用

    enterButton.init(11, INPUT_PULLDOWN); // ui用
    rightButton.init(12, INPUT_PULLDOWN); // ui用
    leftButton.init(10, INPUT_PULLDOWN);  // ui用
    resetButton.init(9, INPUT_PULLDOWN);  // bno用

    // デバッグメッセージの出力
    Serial.println(debugMessage);
    uiPrintDebug(debugMessage.c_str()); // uiにも表示

    // どれかのボタンを押すまで待機
    bool wait = true;
    while (wait)
    {
        enterButton.update();
        rightButton.update();
        leftButton.update();
        resetButton.update();

        wait = !(enterButton.isReleased() || rightButton.isReleased() || leftButton.isReleased() || resetButton.isReleased());
    }

    // ドリブラー
    dribbler1.init(13, 1000, 2000);
    while (!dribbler1.available()) // ドリブラーの初期化が完了していなかったら初期化
        dribbler1.setup();
}

Timer timer; // ui用

void loop()
{
    // ボタン更新
    enterButton.update();
    rightButton.update();
    leftButton.update();
    resetButton.update();

    // センサー類更新
    irUpdate();
    lineUpdate();
    openmvUpdate();
    ps3Update();
    bnoUpdate(resetButton.isReleased()); // bno更新

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