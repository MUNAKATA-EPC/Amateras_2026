#include <Arduino.h>
#include "USE_LCD.h"
#include "USE_m5pico.h"
#include "USE_DSR1202.h"
#include "USE_Kicker.h"
#include "USE_BNO.h"
#include "PID.h"

// App1_PlaySoccerを起動させる関数
void App1_PlaySoccer()
{
    double ControlLevel = 1.0;
    double GyroMotorVal = 0;
    double ControlmotorPower[4] = {0};

    double ControlPower = 0.0;

    bool PlaySoccer = 0;

    while (!SW_cross || PlaySoccer == 1)
    {
        M5PICOget(); // M5-picoからの値を更新

        display.drawBitmap(0, 0, HomeView_PlaySoccerApp1_bitmap, 128, 64, SSD1306_WHITE); // とりあえずホーム画面を表示

        // BNOの値とPWを表示
        display.setTextSize(1);              // テキストサイズを設定
        display.setTextColor(SSD1306_BLACK); // 白で表示
        display.setCursor(83, 13);           // どこにプリントさせるか
        display.print(BNOread());            // 値をプリント
        display.setCursor(83, 24);           // どこにプリントさせるか
        display.print(PID(BNOread()));       // PWをプリント
        // BNOの矢印を作図
        int BNO_x = -int(14 * sin(radians(BNOread()))) + 112;
        int BNO_y = -int(14 * cos(radians(BNOread()))) + 48;
        display.drawLine(112, 48, BNO_x, BNO_y, WHITE);

        // LStickの値とPWを表示
        display.setTextSize(1);                                            // テキストサイズを設定
        display.setTextColor(SSD1306_BLACK);                               // 白で表示
        display.setCursor(19, 13);                                         // どこにプリントさせるか
        display.print(LStick_Angle);                                       // 値をプリント
        display.setCursor(19, 24);                                         // どこにプリントさせるか
        display.print(ControlPower * (LStick_Angle != -1) * ControlLevel); // PWをプリント
        // ジョイコンの配置
        int Board_lx = int(left_X / 13 + 12);
        int Board_ly = int(left_Y / 13 + 45);
        display.fillRect(Board_lx, Board_ly, 7, 7, SSD1306_WHITE);

        // コントロール基板上の操作
        if (digitalRead(11))
        {
            // もともと0なら1を1なら0を代入
            if (PlaySoccer == 0)
            {
                PlaySoccer = 1;
            }
            else
            {
                PlaySoccer = 0;
            }

            // 押されなくなるまで待つ
            while (digitalRead(11))
                ;
        }

        // 1ならロボットを動かす、0なら待つ
        CheckKick(SW_right1 + SW_left1 + SW_right2 + SW_left2); // R1、R2、L1、L2のいずれかが押されるとキックする
        if (SW_right1 + SW_left1 + SW_right2 + SW_left2 > 0)
        {
            display.drawBitmap(35, 54, KickStart_bitmap, 58, 9, SSD1306_WHITE); // kickしてる
        }
        else
        {
            display.drawBitmap(35, 54, KickStop_bitmap, 58, 9, SSD1306_WHITE); // kickしてない
        } // キックしたら表示を変える

        if (PlaySoccer == 1)
        {
            // DSRを動かす
            ControlLevel = 1.0; // ロボットのスピードを変更する変数
            if (SW_circle)
                ControlLevel = 1.8; // ◎ボタンが押されたら加速する

            GyroMotorVal = PID(BNOread()); // ジャイロの回転成分

            Serial.println(LStick_Dist / 128);
            ControlPower = CONTROLMAXPOWER * ControlLevel * LStick_Dist / 128; // モーター(X,Y)を動かす出力値
            ControlmotorPower[0] = -sin(radians(LStick_Angle - 60));           // アナログステックのXY成分
            ControlmotorPower[1] = sin(radians(LStick_Angle - 300));           // アナログステックのXY成分
            ControlmotorPower[2] = -sin(radians(LStick_Angle - 135));          // アナログステックのXY成分
            ControlmotorPower[3] = sin(radians(LStick_Angle - 225));           // アナログステックのXY成分

            // 前と後の調整
            if (LStick_Angle == 0)
            {
                ControlmotorPower[0] = -sin(radians(0 - 60));  // アナログステックのXY成分
                ControlmotorPower[1] = sin(radians(0 - 300));  // アナログステックのXY成分
                ControlmotorPower[2] = -sin(radians(0 - 135)); // アナログステックのXY成分
                ControlmotorPower[3] = sin(radians(0 - 225));  // アナログステックのXY成分
            }
            if (LStick_Angle == 180)
            {
                ControlmotorPower[0] = -sin(radians(180 - 60));  // アナログステックのXY成分
                ControlmotorPower[1] = sin(radians(180 - 300));  // アナログステックのXY成分
                ControlmotorPower[2] = -sin(radians(180 - 135)); // アナログステックのXY成分
                ControlmotorPower[3] = sin(radians(180 - 225));  // アナログステックのXY成分
            }

            // 右と左の調整
            if (LStick_Angle == 90)
            {
                ControlmotorPower[0] = -sin(radians(90 - 60));  // アナログステックのXY成分
                ControlmotorPower[1] = sin(radians(90 - 300));  // アナログステックのXY成分
                ControlmotorPower[2] = -sin(radians(90 - 135)); // アナログステックのXY成分
                ControlmotorPower[3] = sin(radians(90 - 225));  // アナログステックのXY成分
            }
            if (LStick_Angle == 270)
            {
                ControlmotorPower[0] = -sin(radians(270 - 60));  // アナログステックのXY成分
                ControlmotorPower[1] = sin(radians(270 - 300));  // アナログステックのXY成分
                ControlmotorPower[2] = -sin(radians(270 - 135)); // アナログステックのXY成分
                ControlmotorPower[3] = sin(radians(270 - 225));  // アナログステックのXY成分
            }

            ControlmotorPower[0] *= (LStick_Angle != -1) * ControlLevel * ControlPower;
            ControlmotorPower[1] *= (LStick_Angle != -1) * ControlLevel * ControlPower;
            ControlmotorPower[2] *= (LStick_Angle != -1) * ControlLevel * ControlPower;
            ControlmotorPower[3] *= (LStick_Angle != -1) * ControlLevel * ControlPower;

            DSR1202move(int(ControlmotorPower[0] + GyroMotorVal),
                        int(ControlmotorPower[1] - GyroMotorVal),
                        int(ControlmotorPower[2] + GyroMotorVal),
                        int(ControlmotorPower[3]) - GyroMotorVal); // モータを動かす

            display.drawBitmap(57, 34, PlayStart_bitmap, 18, 19, SSD1306_WHITE); // プログラム開始
        }
        else
        {
            DSR1202move(0, 0, 0, 0); // モータは停止

            display.drawBitmap(57, 34, PlayStop_bitmap, 18, 19, SSD1306_WHITE); // プログラム停止
        }

        display.display(); // 表示
        delay(50);
        display.clearDisplay(); // クリア
    }
}

// App2_PlaySoccerを起動させる関数
void App2_PlaySoccer()
{
    while (!SW_cross)
    {
        M5PICOget(); // M5-picoからの値を更新

        display.drawBitmap(0, 0, NoImage_bitmap, 128, 64, SSD1306_WHITE);

        display.display(); // 表示
        delay(50);
        display.clearDisplay(); // クリア
    }
}