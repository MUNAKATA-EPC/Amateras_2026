#include <Arduino.h>
#include "USE_LCD.h"
#include "USE_m5pico.h"
#include "USE_DSR1202.h"
#include "USE_Kicker.h"
#include "USE_BNO.h"
#include "USE_L3GD20H.h"
#include "USE_PIDv2.h"
#include "USE_ControlSW.h"
#include "USE_NeoPixel.h"

// App1_PlaySoccerを起動させる関数
void App1_PlaySoccer()
{
    // ロボットの移動に使う変数
    double ControlLevel = 1.0;
    double GyromotorPower = 0;
    double ControlmotorPower[4] = {0};

    // ロボットのXY方向の最大出力
    double ControlPower = 0.0;

    // BNOの角度を保存する
    int BNOdeg = 0;
    int resetdeg = 0;

    // ロボットを動かすかどうか
    bool PlaySoccer = 0;

    // 加速についてつかう変数
    unsigned long OldTime_Accel = millis();

    while (!ControlSW_left && !ControlSW_front)
    {
        M5PICOget();    // M5-picoからの値を更新
        ControlSWget(); // コントロール基板の

        display.drawBitmap(0, 0, HomeView_PlaySoccerApp1_bitmap, 128, 64, SSD1306_WHITE); // とりあえずホーム画面を表示

        // ジャイロ(BNO)を更新
        /*
        int WantGoDeg = RStick_Angle;                          // 今後ロボットの向きたい方向
        int resetWantDeg = (WantGoDeg - resetdeg + 360) % 360; // resetdgeを0度として行きたい方向はどのくらいズレているか
        if (WantGoDeg == -1)
        {
            resetdeg = 0;
        }
        else if (resetWantDeg < 180)
        {
            resetdeg += int(RStick_Angle / 4);

            int resetWantDeg_Now = (WantGoDeg - resetdeg + 360) % 360; // resetdgeを0度として行きたい方向はどのくらいズレているか
            if (resetWantDeg_Now > 180)
            {
                resetdeg = WantGoDeg;
            }
        }
        else
        {
            resetdeg -= int(RStick_Angle / 4);

            int resetWantDeg_Now = (WantGoDeg - resetdeg + 360) % 360; // resetdgeを0度として行きたい方向はどのくらいズレているか
            if (resetWantDeg_Now < 180)
            {
                resetdeg = WantGoDeg;
            }
        }
        resetdeg = (resetdeg + 360) % 360;
        */
        resetdeg = RStick_Angle;
        BNOdeg = int(BNOread() - resetdeg + 360);
        BNOdeg %= 360;

        // BNOの値とPWを表示
        display.setTextSize(1);              // テキストサイズを設定
        display.setTextColor(SSD1306_BLACK); // 白で表示
        display.setCursor(83, 13);           // どこにプリントさせるか
        display.print(BNOdeg);               // 値をプリント
        display.setCursor(83, 24);           // どこにプリントさせるか
        display.print(PIDv2(BNOdeg));        // PWをプリント
        // BNOの矢印を作図
        int BNO_x = -int(14 * sin(radians(BNOdeg))) + 112;
        int BNO_y = -int(14 * cos(radians(BNOdeg))) + 48;
        display.drawLine(112, 48, BNO_x, BNO_y, WHITE);

        // LStickの値とPWを表示
        display.setTextSize(1);                                // テキストサイズを設定
        display.setTextColor(SSD1306_BLACK);                   // 白で表示
        display.setCursor(19, 13);                             // どこにプリントさせるか
        display.print(LStick_Angle);                           // 値をプリント
        display.setCursor(19, 24);                             // どこにプリントさせるか
        display.print(CONTROLMAXPOWER * (LStick_Angle != -1)); // PWをプリント
        // ジョイコンの配置
        int Board_lx = int(left_X / 13 + 12);
        int Board_ly = int(left_Y / 13 + 45);
        display.fillRect(Board_lx, Board_ly, 7, 7, SSD1306_WHITE);

        // ロボットを動かすかどうかどうかの表示
        if (PlaySoccer == 1)
        {

            display.drawBitmap(57, 34, PlayStop_bitmap, 18, 19, SSD1306_WHITE); // プログラム開始
        }
        else
        {
            display.drawBitmap(57, 34, PlayStart_bitmap, 18, 19, SSD1306_WHITE); // プログラム停止
        }

        // コントロール基板上の操作
        if (ControlSW_right)
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
        }

        // キッカー
        int KickDoOrNot = (SW_right1 + SW_left1 + SW_right2 + SW_left2) * (MeterMax == 1);
        CheckKick(KickDoOrNot); // R1、R2、L1、L2のいずれかが押されるとキックする
        if (KickDoOrNot > 0)
        {
            display.drawBitmap(35, 54, KickStart_bitmap, 58, 9, SSD1306_WHITE); // kickしてる
            Meter_NeoLight_Clear();                                             // kickしたので初期化する
        }
        else
        {
            display.drawBitmap(35, 54, KickStop_bitmap, 58, 9, SSD1306_WHITE); // kickしてない
        } // キックしたら表示を変える

        // 加速ボタンのスイッチ
        unsigned long NowTime_Accel = millis() - OldTime_Accel;
        if (SW_circle)
        {
            if (NowTime_Accel < 800)
            {
                ControlLevel = 1.3;
            }
            else
            {
                ControlLevel = 1.6;
            }
        }
        else
        {
            ControlLevel = 1.0;       // ロボットのスピードを変更する変数
            OldTime_Accel = millis(); // 昔の時間を更新
        }

        // ジャイロのモータ出力
        GyromotorPower = PIDv2(BNOdeg); // ジャイロの回転成分

        // アナログステックのモータ出力
        ControlPower = CONTROLMAXPOWER * ControlLevel * LStick_Dist / 128; // モーター(X,Y)を動かす出力値
        ControlmotorPower[0] = -sin(radians(LStick_Angle_Radjust - 60));   // アナログステックのXY成分
        ControlmotorPower[1] = sin(radians(LStick_Angle_Radjust - 300));   // アナログステックのXY成分
        ControlmotorPower[2] = -sin(radians(LStick_Angle_Radjust - 135));  // アナログステックのXY成分
        ControlmotorPower[3] = sin(radians(LStick_Angle_Radjust - 225));   // アナログステックのXY成分
        // 前と後の調整
        if (ControlLevel == 1)
        {
            if (LStick_Angle_Radjust == 0)
            {
                ControlmotorPower[0] = -sin(radians(0 - 60)); // アナログステックのXY成分
                ControlmotorPower[1] = sin(radians(0 - 300));        // アナログステックのXY成分
                ControlmotorPower[2] = -sin(radians(0 - 135));       // アナログステックのXY成分
                ControlmotorPower[3] = sin(radians(0 - 225));        // アナログステックのXY成分
            }
            if (LStick_Angle_Radjust == 180)
            {
                ControlmotorPower[0] = -sin(radians(180 - 60)); // アナログステックのXY成分
                ControlmotorPower[1] = sin(radians(180 - 300));        // アナログステックのXY成分
                ControlmotorPower[2] = -sin(radians(180 - 135));       // アナログステックのXY成分
                ControlmotorPower[3] = sin(radians(180 - 225));        // アナログステックのXY成分
            }
        }
        else
        {
            if (LStick_Angle_Radjust == 0)
            {
                ControlmotorPower[0] = -sin(radians(0 - 60));  // アナログステックのXY成分
                ControlmotorPower[1] = sin(radians(0 - 300));        // アナログステックのXY成分
                ControlmotorPower[2] = -sin(radians(0 - 135));       // アナログステックのXY成分
                ControlmotorPower[3] = sin(radians(0 - 225)); // アナログステックのXY成分
            }
            if (LStick_Angle_Radjust == 180)
            {
                ControlmotorPower[0] = -sin(radians(180 - 60));  // アナログステックのXY成分
                ControlmotorPower[1] = sin(radians(180 - 300));        // アナログステックのXY成分
                ControlmotorPower[2] = -sin(radians(180 - 135));       // アナログステックのXY成分
                ControlmotorPower[3] = sin(radians(180 - 225)); // アナログステックのXY成分
            }
        }
        // 右と左の調整
        if (LStick_Angle_Radjust == 90)
        {
            ControlmotorPower[0] = -sin(radians(90 - 60)) * 1.2; // アナログステックのXY成分
            ControlmotorPower[1] = sin(radians(90 - 300)) * 1.2; // アナログステックのXY成分
            ControlmotorPower[2] = -sin(radians(90 - 135));       // アナログステックのXY成分
            ControlmotorPower[3] = sin(radians(90 - 225));        // アナログステックのXY成分
        }
        if (LStick_Angle_Radjust == 270)
        {
            ControlmotorPower[0] = -sin(radians(270 - 60)) * 1.2; // アナログステックのXY成分
            ControlmotorPower[1] = sin(radians(270 - 300)) * 1.2; // アナログステックのXY成分
            ControlmotorPower[2] = -sin(radians(270 - 135));       // アナログステックのXY成分
            ControlmotorPower[3] = sin(radians(270 - 225));        // アナログステックのXY成分
        }
        // モータ出力をかける
        ControlmotorPower[0] *= (LStick_Angle_Radjust != -1) * ControlLevel * ControlPower;
        ControlmotorPower[1] *= (LStick_Angle_Radjust != -1) * ControlLevel * ControlPower;
        ControlmotorPower[2] *= (LStick_Angle_Radjust != -1) * ControlLevel * ControlPower;
        ControlmotorPower[3] *= (LStick_Angle_Radjust != -1) * ControlLevel * ControlPower;

        // DSRを動かす
        DSR1202move(int(ControlmotorPower[0] - GyromotorPower) * PlaySoccer,
                    int(ControlmotorPower[1] + GyromotorPower) * PlaySoccer,
                    int(ControlmotorPower[2] - GyromotorPower) * PlaySoccer,
                    int(ControlmotorPower[3] + GyromotorPower) * PlaySoccer);

        // NeoPixel・LCDを表示
        LCD_Show(); // LCDを表示
        for (int i = 0; i < 40; i++)
        {
            if (MeterMax == 1)
            {
                Mapping_NeoLight(0, 23, 24, 120, 120, 100);
            }
            else
            {
                Meter_NeoLight(10, 100);
            }

            NeoPixel_Show();  // NeoPixelを表示
            delay(1);         // 1ms待つ
            NeoPixel_Clear(); // NeoPixelをクリア
        }
        LCD_Clear(); // LCDをクリア
    }
}

// App2_PlaySoccerを起動させる関数
void App2_PlaySoccer()
{
    // ロボットの移動に使う変数
    double ControlLevel = 1.0;
    double GyromotorPower = 0;
    double ControlmotorPower[4] = {0};

    // ロボットのXY方向の最大出力
    double ControlPower = 0.0;

    // BNOの角度を保存する
    int L3GDdeg = 0;
    // int resetdeg = 0;

    // ロボットを動かすかどうか
    bool PlaySoccer = 0;

    while (!ControlSW_left && !ControlSW_front)
    {
        M5PICOget();    // M5-picoからの値を更新
        ControlSWget(); // コントロール基板の

        display.drawBitmap(0, 0, HomeView_PlaySoccerApp2_bitmap, 128, 64, SSD1306_WHITE); // とりあえずホーム画面を表示

        // ジャイロ(L3GD)を更新
        L3GDdeg = L3GD20Hread();

        // L3GD20Hの値とPWを表示
        display.setTextSize(1);              // テキストサイズを設定
        display.setTextColor(SSD1306_BLACK); // 白で表示
        display.setCursor(83, 13);           // どこにプリントさせるか
        if (L3GDdeg != -1)
        {
            display.print(L3GDdeg); // 値をプリント
        }
        else
        {
            display.print("Cal..."); // 文字をプリント
        }
        display.setCursor(83, 24);     // どこにプリントさせるか
        display.print(PIDv2(L3GDdeg)); // PWをプリント
        // L3GD20Hの矢印を作図
        int BNO_x = -int(14 * sin(radians(L3GDdeg))) + 112;
        int BNO_y = -int(14 * cos(radians(L3GDdeg))) + 48;
        display.drawLine(112, 48, BNO_x, BNO_y, WHITE);

        // LStickの値とPWを表示
        display.setTextSize(1);                                // テキストサイズを設定
        display.setTextColor(SSD1306_BLACK);                   // 白で表示
        display.setCursor(19, 13);                             // どこにプリントさせるか
        display.print(LStick_Angle);                           // 値をプリント
        display.setCursor(19, 24);                             // どこにプリントさせるか
        display.print(CONTROLMAXPOWER * (LStick_Angle != -1)); // PWをプリント
        // ジョイコンの配置
        int Board_lx = int(left_X / 13 + 12);
        int Board_ly = int(left_Y / 13 + 45);
        display.fillRect(Board_lx, Board_ly, 7, 7, SSD1306_WHITE);

        // コントロール基板上の操作
        if (ControlSW_right)
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
            // 加速ボタンのスイッチ
            if (SW_circle)
            {
                ControlLevel = 1.4; // ◎ボタンが押されたら加速する
            }
            else
            {
                ControlLevel = 1.0; // ロボットのスピードを変更する変数
            }

            // ジャイロのモータ出力
            GyromotorPower = PIDv2(L3GDdeg) * 1.85; // ジャイロの回転成分

            // アナログステックのモータ出力
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
                ControlmotorPower[0] = -sin(radians(90 - 60)) * 1.35; // アナログステックのXY成分
                ControlmotorPower[1] = sin(radians(90 - 300)) * 1.35; // アナログステックのXY成分
                ControlmotorPower[2] = -sin(radians(90 - 135));       // アナログステックのXY成分
                ControlmotorPower[3] = sin(radians(90 - 225));        // アナログステックのXY成分
            }
            if (LStick_Angle == 270)
            {
                ControlmotorPower[0] = -sin(radians(270 - 60)) * 1.35; // アナログステックのXY成分
                ControlmotorPower[1] = sin(radians(270 - 300)) * 1.35; // アナログステックのXY成分
                ControlmotorPower[2] = -sin(radians(270 - 135));       // アナログステックのXY成分
                ControlmotorPower[3] = sin(radians(270 - 225));        // アナログステックのXY成分
            }
            // モータ出力をかける
            ControlmotorPower[0] *= (LStick_Angle != -1) * ControlLevel * ControlPower;
            ControlmotorPower[1] *= (LStick_Angle != -1) * ControlLevel * ControlPower;
            ControlmotorPower[2] *= (LStick_Angle != -1) * ControlLevel * ControlPower;
            ControlmotorPower[3] *= (LStick_Angle != -1) * ControlLevel * ControlPower;

            // DSRを動かす
            DSR1202move(int(ControlmotorPower[0] - GyromotorPower),
                        int(ControlmotorPower[1] + GyromotorPower),
                        int(ControlmotorPower[2] - GyromotorPower),
                        int(ControlmotorPower[3] + GyromotorPower));

            display.drawBitmap(57, 34, PlayStop_bitmap, 18, 19, SSD1306_WHITE); // プログラム停止
        }
        else
        {
            DSR1202move(0, 0, 0, 0); // モータは停止

            display.drawBitmap(57, 34, PlayStart_bitmap, 18, 19, SSD1306_WHITE); // プログラム開始
        }

        // NeoPixel・LCDを表示
        LCD_Show(); // LCDを表示
        for (int i = 0; i < 40; i++)
        {
            SpeedTurn_NeoLight(10, 100);

            NeoPixel_Show();  // NeoPixelを表示
            delay(1);         // 1ms待つ
            NeoPixel_Clear(); // NeoPixelをクリア
        }
        LCD_Clear(); // LCDをクリア
    }
}