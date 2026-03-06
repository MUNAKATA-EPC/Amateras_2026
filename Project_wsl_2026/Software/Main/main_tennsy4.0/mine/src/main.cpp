#include <Arduino.h>

// action類
#include "attacker.hpp"
#include "defenderVer2.hpp"
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
#include "buzzer.hpp"
#include "digitalSensor.hpp"
#include "ir.hpp"
#include "line.hpp"
#include "ps3.hpp"
#include "openmv.hpp"
#include "uss.hpp"

void setup()
{
    // I2C
    bool ssd_success = uiInit(&Wire1, 0x3C, 128, 64);
    bool bno_success = bnoInit(&Wire, 0x28);

    // シリアル
    Serial.begin(9600L); // デバッグ用

    irInit(&Serial1, 115200);
    lineInit(&Serial5, 115200);
    openmvInit(&Serial3, 115200);
    ussInit(&Serial2, 115200);
    ps3Init(&Serial7, 115200);
    ps3StickAdjust(20.0f, 20.0f);

    motorsInit(&Serial1, 115200);
    // motorsSetTogglePin(4, INPUT_PULLDOWN);   // モータの起動トグルスイッチのピン設定
    motorsSetMoveSign(1, 1, 1, 1);           // 移動のための符号をセット
    motorsSetPdSign(1, 1, 1, 1);             // PD制御のための符号をセット
    motorsSetDegPosition(315, 45, 225, 135); // モータの位置をセット
    motorsStop();                            // 停止させておく

    // デジタル
    kicker1.init(2, 3, 1500);
    catchSensor.init(6); // キッカー用

    enterButton.init(10, INPUT_PULLDOWN); // ui用
    backButton.init(11, INPUT_PULLDOWN);  // ui用
    rightButton.init(33, INPUT_PULLDOWN); // ui用
    leftButton.init(12, INPUT_PULLDOWN);  // ui用
    resetButton.init(37, INPUT_PULLDOWN); // bno用

    fullColorLed1.init(36, 35, 34); // デバッグ用

    // デバッグメッセージの出力(ui)
    String message = "> setup <\n";
    message += "ssd : " + String((ssd_success) ? "found" : "not found") + "\n";
    message += "bno : " + String((bno_success) ? "found" : "not found") + "\n";
    // message += "-onigiri.com-";

    uiClear();
    uiPrintDebug(message.c_str());
    uiShow();

    // どれかのボタンを押すまで待機
    int music_type = -1;
    while (true)
    {
        enterButton.update();
        backButton.update();
        rightButton.update();
        leftButton.update();
        resetButton.update();

        if (resetButton.isReleased())
        {
            anyrtttl::nonblocking::stop(); // 曲の演奏停止
            delay(300);

            music_type = (music_type + 1) % 4; // 次の曲へ

            if (music_type == 0)
                anyrtttl::nonblocking::begin(BUZZER_PIN, zenzenzense); // 前前前世演奏開始
            else if (music_type == 1)
                anyrtttl::nonblocking::begin(BUZZER_PIN, shokei_part3); // 処刑用BGM演奏開始
            else if (music_type == 2)
                anyrtttl::nonblocking::begin(BUZZER_PIN, mission_imp); // ミッションインポッシブル
            else if (music_type == 3)
                anyrtttl::nonblocking::begin(BUZZER_PIN, mario_bro); // マリオ
        }

        anyrtttl::nonblocking::play(); // 演奏中

        if (enterButton.isReleased() || backButton.isReleased() || rightButton.isReleased() || leftButton.isReleased())
        {
            break;
        }
    }
    anyrtttl::nonblocking::stop(); // 曲の演奏停止

    tone(BUZZER_PIN, BuzzerPitches::MyE7, 100); // 決定音
    // anyrtttl::blocking::play(BUZZER_PIN, startup2); // 起動音2

    delay(250);
}

Timer btn_timer; // btn用
Timer ui_timer;  // ui用
bool old_running_flag = false;

void loop()
{
    // ボタン類更新
    if (!btn_timer.everReset() || btn_timer.msTime() > 50UL)
    {
        btn_timer.reset();

        enterButton.update();
        rightButton.update();
        leftButton.update();
        backButton.update();
        resetButton.update();

        // uiを実行・描画 // ボタンの更新
        uiButtonUpdate(enterButton.isReleased(), backButton.isReleased(), rightButton.isReleased(), leftButton.isReleased());
    }
    // カラーLEDクリア
    fullColorLed1.rgbLightUp(0, 0, 0);

    // センサー類更新
    irUpdate();
    lineUpdate();
    openmvUpdate();
    // ps3Update();
    ussUpdate();
    bnoUpdate(resetButton.isReleased(), ussRightSpeed(), ussLeftSpeed()); // bno更新

    // 動作を実行
    if (uiRunning() == false)
    {
        old_running_flag = false;

        motorsStop(); // 動作選択中はモータを止める

        if (!ui_timer.everReset() || ui_timer.msTime() > 10UL) // runningではないので10msに一回描画
        {
            ui_timer.reset();

            uiClear();
            uiDrawMain(); // 10msに一回更新

            if (!uiActionDecided()) // センサーモニター表示
            {
                switch (uiMeterNumber())
                {
                case 0:
                {
                    uiPrint(0, 8, "[ir]\n deg:" + String(irDeg()) + "\n dis:" + String(irDis()) + "\n y:" + String(irY()) + "\n s_y:" + String(irYSpeed()));
                    uiDrawCircleMeter(92, 32, 20, "deg", irDeg());
                    break;
                }
                case 1:
                {
                    if (lineIsAdjusting()) // 調整中ならセンサーの値を表示
                    {
                        uiPrint(0, 8, "[line_adjust(0~7)]");
                        for (int i = 0; i < 4; i++)
                        {
                            uiPrint(0, 16 + i * 8, " " + String(i * 2) + ":" + String(lineValueToAdjust(i)));
                            uiPrint(63, 16 + i * 8, " " + String(i * 2 + 1) + ":" + String(lineValueToAdjust(i * 2 + 1)));
                        }
                    }
                    else // 通常表示
                    {
                        uiPrint(0, 8, "[ringLine]\n deg:" + String(lineRingDeg()) + "\n dis:" + String(lineRingDis()));
                        uiDrawCircleMeter(92, 32, 20, "deg", lineRingDeg());
                    }
                    break;
                }
                case 2:
                {
                    if (lineIsAdjusting()) // 調整中ならセンサーの値を表示
                    {
                        uiPrint(0, 8, "[line_adjust(8~15)]");
                        for (int i = 4; i < 8; i++)
                        {
                            uiPrint(0, 16 + (i - 4) * 8, " " + String(i * 2) + ":" + String(lineValueToAdjust(i)));
                            uiPrint(63, 16 + (i - 4) * 8, " " + String(i * 2 + 1) + ":" + String(lineValueToAdjust(i * 2 + 1)));
                        }
                    }
                    else // 通常表示
                    {
                        uiPrint(0, 8, "[sideLine]\n deg:" + String(lineSideDeg()) + "\n dis:" + String(lineSideDeg()));
                        uiDrawCircleMeter(92, 32, 20, "deg", lineSideDeg());
                    }
                    break;
                }
                case 3:
                {
                    uiPrint(0, 8, "[bno055]\n deg:" + String(bnoDeg()) + "\n x_speed:" + String(bnoSpeedX()));
                    uiDrawCircleMeter(92, 32, 20, "deg", bnoDeg());
                    break;
                }
                case 4:
                {
                    uiPrint(0, 8, "[yellowGoal]\n deg:" + String(yellowGoalDeg()) + "\n dis:" + String(yellowGoalDis()));
                    uiDrawCircleMeter(92, 32, 20, "deg", yellowGoalDeg());
                    break;
                }
                case 5:
                {
                    uiPrint(0, 8, "[blueGoal]\n deg:" + String(blueGoalDeg()) + "\n dis:" + String(blueGoalDis()));
                    uiDrawCircleMeter(92, 32, 20, "deg", blueGoalDeg());
                    break;
                }
                case 6:
                {
                    uiPrint(0, 8, "[field]\n deg:" + String(fieldDeg()) + "\n\n[catch]\n react:" + String(catchSensor.read()));
                    uiDrawCircleMeter(92, 32, 20, "deg", fieldDeg());
                    break;
                }
                case 7:
                {
                    uiPrint(0, 8, "[uss]\n l_dis:" + String(ussLeftDis()) + "cm" + "\n l_speed:" + String(ussLeftSpeed()) + "cm/s");
                    uiPrint(0, 32, "[uss]\n r_dis:" + String(ussRightDis()) + "cm" + "\n r_speed:" + String(ussRightSpeed()) + "cm/s");
                    break;
                }
                case 8:
                {
                    uiPrint(0, 8, "[ps3_left]\n x:" + String(ps3LeftStickX()) + "\n y:" + String(ps3LeftStickY()) + "\n deg:" + String(ps3LeftStickDeg()) + "\n dis:" + String(ps3LeftStickDis()));
                    uiDrawCircleMeter(92, 32, 20, "deg", ps3LeftStickDeg());
                    break;
                }
                case 9:
                {
                    uiPrint(0, 8, "[ps3_right]\n x:" + String(ps3RightStickX()) + "\n y:" + String(ps3RightStickY()) + "\n deg:" + String(ps3RightStickDeg()) + "\n dis:" + String(ps3RightStickDis()));
                    uiDrawCircleMeter(92, 32, 20, "deg", ps3RightStickDeg());
                    break;
                }
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

            old_running_flag = true;
        }

        switch (uiActionNumber())
        {
        case Action::ATTACKER:
        {
            playAttacker(Attacker::Mode(uiModeNumber()));
            break;
        }
        case Action::DEFENDER:
        {
            playDefenderVer2(Defender::Mode(uiModeNumber()));
            break;
        }
        case Action::TEST:
        {
            playTest(Test::Mode(uiModeNumber()));
            break;
        }
        case Action::RADICON:
        {
            playRadicon(Radicon::Mode(uiModeNumber()));
            break;
        }
        }
    }
}