#include <Arduino.h>

// action類
#include "attacker.hpp"
#include "defender.hpp"
#include "radicon.hpp"
#include "test.hpp"

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
    String debugMessage = "> setup <\n";

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
    motorsSetMoveSign(1, 1, 1, 1);           // 移動のための符号をセット
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
    uiClear();
    uiPrintDebug(debugMessage.c_str()); // uiにも表示
    uiShow();

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
    while (enterButton.isPushing() || rightButton.isPushing() || leftButton.isPushing() || resetButton.isPushing())
    {
        enterButton.update();
        rightButton.update();
        leftButton.update();
        resetButton.update();
    } // 押しっぱなし防止

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

    // uiを実行・描画
    if (!timer.everCalled() || timer.msTime() > 10) // まだ呼ばれていない場合もタイマーをリセットさせる
    {
        timer.reset();

        uiClear();
        uiDrawMain(enterButton.isReleased(), rightButton.isReleased(), leftButton.isReleased()); // 10msに一回更新

        if (!uiActionDecided())
        {
            switch (uiMeterNumber())
            {
            case 0:
                uiPrint(0, 8, "[ir]\n deg:" + String(irDeg()) + "\n dis:" + String(irDis()));
                uiDrawCircleMeter(92, 32, 20, "deg", irDeg());
                break;
            case 1:
                uiPrint(0, 8, "[ringLine]\n deg:" + String(lineRingDeg()) + "\n dis:" + String(lineRingDis()));
                uiDrawCircleMeter(92, 32, 20, "deg", lineRingDeg());
                break;
            case 2:
                uiPrint(0, 8, "[sideLine]\n deg:" + String(lineSideDeg()));
                uiDrawCircleMeter(92, 32, 20, "deg", lineSideDeg());
                break;
            case 3:
                uiPrint(0, 8, "[bno055]\n deg:" + String(bnoDeg()));
                uiDrawCircleMeter(92, 32, 20, "deg", bnoDeg());
                break;
            case 4:
                uiPrint(0, 8, "[yellowGoal]\n deg:" + String(yellowGoalDeg()) + "\n dis:" + String(yellowGoalDis()));
                uiDrawCircleMeter(92, 32, 20, "deg", yellowGoalDeg());
                break;
            case 5:
                uiPrint(0, 8, "[blueGoal]\n deg:" + String(blueGoalDeg()) + "\n dis:" + String(blueGoalDis()));
                uiDrawCircleMeter(92, 32, 20, "deg", blueGoalDeg());
                break;
            case 6:
                uiPrint(0, 8, "[field]\n deg:" + String(fieldDeg()));
                uiDrawCircleMeter(92, 32, 20, "deg", fieldDeg());
                break;
            case 7:
                uiPrint(0, 8, "[none]");
                break;
            }
        }
        else if (uiModeDecided())
        {
            uiConfigPrintAndGet(1, "goal Range", 5, -10, 10);
            uiConfigPrintAndGet(2, "goal Range", 5, -20, 10);
            uiConfigPrintAndGet(3, "goal Range", 10, -10, 20);
        }

        uiShow();
    }

    // 動作を実行
    if (!uiRunning())
    {
        Serial.print("now sellecting...");
        kicker1.kick(false); // キッカーを動かさない
        motorsStop();        // 動作選択中はモータを止める
    }
    else
    {
        switch (uiActionNumber())
        {
        case Action::ATTACKER:
            playAttacker(Attacker::Mode(uiModeNumber()));
            break;
        case Action::DEFENDER:
            playDefender(Defender::Mode(uiModeNumber()));
            break;
        case Action::TEST:
            playTest();
            break;
        case Action::RADICON:
            playRadicon();
            break;
        }
    }

    Serial.print("\n");
}