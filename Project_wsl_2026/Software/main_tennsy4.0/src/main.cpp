#include <Arduino.h>
// 攻撃用
#include "attacker.hpp"
// 守備用
#include "defender.hpp"
// テスト（動作確認）用
#include "test.hpp"
// ラジコン用
#include "radicon.hpp"
// PCプリント用
#include "print.hpp"
// LCD用
#include "ui.hpp"

void setup()
{
  Serial.begin(9600); // PCとのシリアル通信を開始

  IR_init(&Serial1, 115200); // シリアル1を使いボートレート115200にする

  LINE_init(&Serial5, 115200); // シリアル5を使いボートレート115200にする

  DSR1202_set_motormove_togglepin(5); // モータを動かすトグルスイッチのピン番号を設定
  DSR1202_init(&Serial1, 115200);     // シリアル1を使いボートレート115200にする
  motors_init(315, 45, 225, 135);     // モーターの設置角度を定義

  Camera_init(&Serial3, 115200); // シリアル3を使いボートレート115200にする

  BNO055_set_resetpin(9, INPUT_PULLDOWN); // BNOのリセットピンを定義
  BNO055_init(&Wire, 0x28);               // どのBNOを使うか

  catchsensor_init(A6);    // キャッチセンサーのピンを設定
  kicker_set_fetpin(2, 3); // キッカーのFETピンを設定
  kicker_init(1800);       // クールダウン時間の定義

  Ps3_set_stick_adjust(10, 10, 10, 10);
  Ps3_init(&Serial2, 115200); // シリアル2を使いボートレート115200にする

  SSD1306_init(&Wire1, 0x3C, 128, 64);
  SSD1306_clear();
  SSD1306_write(1, 0, 0, "Hello", false);
  SSD1306_show();
  ui_set_lcdpin(11, INPUT_PULLDOWN, 10, INPUT_PULLDOWN, 12, INPUT_PULLDOWN); // LCD用のボタンの定義
  ui_init();                                                                 // 定義
}

void loop()
{
  IR_update();           // 更新
  LINE_serial_update();  // 更新1
  LINE_compute_update(); // 更新2
  Camera_update();       // 更新
  BNO055_update();       // 更新
  Ps3_serial_update();   // 更新1
  Ps3_compute_update();  // 更新2

  ui_process(); // uiを実行

  if (is_now_selecting_ui()) // 今選んでる途中なら
  {
    Serial.println("Now selecting UI");

    motors_break(); // モータは停止させる
  }
  else // 今選んだ
  {
    // play_pc_print(PS3_CHECK_WITH_PC); // 全て確認する

    switch (get_selected_ui_action())
    {
    case ACTION_ATTACKER:
      switch (get_selected_ui_mode())
      {
      case PD_USE_ONLY_GYRO_MODE:
        play_attacker(false, false, 95); // ジャイロのみで動かす
        break;
      case PD_USE_YELLOW_CAM_MODE:
        play_attacker(true, false, 95); // カメラで動かす
        break;
      case PD_USE_BLUE_CAM_MODE:
        play_attacker(false, true, 95); // カメラで動かす
        break;
      }

      break;

    case ACTION_DEFENDER:
      switch (get_selected_ui_mode())
      {
      case PD_USE_ONLY_GYRO_MODE:
        play_defender(false, false, 95); // ジャイロのみで動かす
        break;
      case PD_USE_YELLOW_CAM_MODE:
        play_defender(true, false, 95); // カメラで動かす
        break;
      case PD_USE_BLUE_CAM_MODE:
        play_defender(false, true, 95); // カメラで動かす
        break;
      }

      break;

    case ACTION_TEST:
      switch (get_selected_ui_mode())
      {
      case TEST_KICKER_MODE:
        play_test(TEST_KICKER_MODE);
        break;
      case TEST_MONITOR_MODE:
        play_test(TEST_MONITOR_MODE);
        break;
      case TEST_PD_MODE:
        play_test(TEST_PD_MODE);
        break;
      }

      break;

    case ACTION_RADICON:
      switch (get_selected_ui_mode())
      {
      case RADICON_50cc_MODE:
        play_radicon(35);
        break;
      case RADICON_100cc_MODE:
        play_radicon(55);
        break;
      case RADICON_150cc_MODE:
        play_radicon(75);
        break;
      case RADICON_200cc_MODE:
        play_radicon(95);
        break;
      }

      break;

    default:
      Serial.println("error");

      break;
    }
  }
}