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
    Serial.begin(9600L); // デバッグ用

    debugMessage += irInit(&Serial1, 115200, 0xAA) ? "ir     : found\n" : "ir     : not found\n";
    debugMessage += lineInit(&Serial5, 115200, 0xAA) ? "line   : found\n" : "line   : not found\n";
    debugMessage += openmvInit(&Serial3, 115200, 0xAA) ? "openmv : found\n" : "openmv : not found\n";
    debugMessage += ps3Init(&Serial7, 115200, 0xAA) ? "ps3    : found\n" : "ps3    : not found\n";
    ps3StickAdjust(20.0f, 20.0f);

    debugMessage += motorsInit(&Serial1, 115200) ? "motors : found\n" : "motors : not found\n";
    motorsSetMoveSign(1, 1, 1, 1);           // 移動のための符号をセット
    motorsSetPdSign(1, 1, 1, 1);             // PD制御のための符号をセット
    motorsSetDegPosition(315, 45, 225, 135); // モータの位置をセット

    // デジタル
    kicker1.init(2, 3, 1500);
    catchSensor.init(6); // キッカー用

    enterButton.init(10, INPUT_PULLDOWN); // ui用
    backButton.init(11, INPUT_PULLDOWN);  // ui用
    rightButton.init(33, INPUT_PULLDOWN); // ui用
    leftButton.init(12, INPUT_PULLDOWN);  // ui用
    resetButton.init(37, INPUT_PULLDOWN); // bno用

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
        backButton.update();
        rightButton.update();
        leftButton.update();

        wait = !(enterButton.isReleased() || backButton.isReleased() || rightButton.isReleased() || leftButton.isReleased());
    }
    while (enterButton.isPushing() || backButton.isPushing() || rightButton.isPushing() || leftButton.isPushing())
    {
        enterButton.update();
        backButton.update();
        rightButton.update();
        leftButton.update();
    } // 押しっぱなし防止

    // ドリブラー
    dribbler1.init(13, 1000, 2000);
    while (!dribbler1.available()) // ドリブラーの初期化が完了していなかったら初期化
        dribbler1.setup();
}

Timer timer; // ui用
bool old_running_flag = false;

void loop()
{
    // ボタン更新
    enterButton.update();
    backButton.update();
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
    uiButtonUpdate(enterButton.isReleased(), backButton.isReleased(), rightButton.isReleased(), leftButton.isReleased()); // ボタンの更新

    // 動作を実行
    if (!uiRunning())
    {
        old_running_flag = false;

        Serial.print("now sellecting...");
        kicker1.kick(false); // キッカーを動かさない
        motorsStop();        // 動作選択中はモータを止める

        if (!timer.everCalled() || timer.msTime() > 10) // runningではないので10msに一回描画
        {
            timer.reset();

            uiClear();
            uiDrawMain(); // 10msに一回更新

            if (!uiActionDecided()) // センサーモニター表示
            {
                switch (uiMeterNumber())
                {
                case 0:
                    uiPrint(0, 8, "[ir]\n deg:" + String(irDeg()) + "\n dis:" + String(irDis()) + "\n x:" + String(irX()) + "\n y:" + String(irY()));
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
                    uiPrint(0, 8, "[field]\n deg:" + String(fieldDeg()) + "\n\n[catch]\n react:" + String(catchSensor.read()));
                    uiDrawCircleMeter(92, 32, 20, "deg", fieldDeg());
                    break;
                case 7:
                    uiPrint(0, 8, "[ps3_left]\n x:" + String(ps3LeftStickX()) + "\n y:" + String(ps3LeftStickY()) + "\n deg:" + String(ps3LeftStickDeg()) + "\n dis:" + String(ps3LeftStickDis()));
                    uiDrawCircleMeter(92, 32, 20, "deg", ps3LeftStickDeg());
                    break;
                case 8:
                    uiPrint(0, 8, "[ps3_right]\n x:" + String(ps3RightStickX()) + "\n y:" + String(ps3RightStickY()) + "\n deg:" + String(ps3RightStickDeg()) + "\n dis:" + String(ps3RightStickDis()));
                    uiDrawCircleMeter(92, 32, 20, "deg", ps3RightStickDeg());
                    break;
                case 9:
                    ButtonDataType btns[] = {UP, DOWN, LEFT, RIGHT, TRIANGLE, CIRCLE, CROSS, SQUARE, L1, L2, L3, R1, R2, R3};
                    String message = "";
                    for (int i = 0; i < 14; i++)
                    {
                        message += String((ps3ButtonIsPushing(btns[i])) ? "1" : "0");
                    }
                    uiPrint(0, 8, "[ps3_button]\n" + message);
                }
            }

            uiShow();
        }
    }
    else
    {
        if (old_running_flag == false)
        {
            uiClear();
            uiDrawMain();
            uiShow();
        }
        old_running_flag = true;

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