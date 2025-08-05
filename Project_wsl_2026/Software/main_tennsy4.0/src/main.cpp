#include <Arduino.h>
// 攻撃用
#include "attacker.hpp"
// 守備用
#include "defender.hpp"
// PCプリント用
#include "print.hpp"
// テスト（動作確認）用
#include "test.hpp"
// LCD用
#include "ui.hpp"

void setup()
{
  Serial.begin(9600); // PCとのシリアル通信を開始

  IRserial_init(&Serial1, 9600); // シリアル1を使いボートレート9600にする

  LINEserial_init(&Serial5, 9600); // シリアル2を使いボートレート9600にする

  DSR1202_set_motormove_togglepin(4); // モータを動かすトグルスイッチのピン番号を設定
  DSR1202_init(&Serial2, 115200);     // シリアル3を使いボートレート115200にする
  motors_init(315, 45, 225, 135);     // モーターの設置角度を定義

  OpenMVserial_init(&Serial3, 115200); // シリアル4を使いボートレート115200にする

  BNO055_set_resetpin(9, INPUT_PULLDOWN); // BNOのリセットピンを定義
  BNO055_init(&Wire, 0x28);               // どのBNOを使うか

  // catchsensor_init(1);     // キャッチセンサーのピンを設定
  kicker_set_fetpin(2, 3); // キッカーのFETピンを設定
  kicker_init(700);       // クールダウン時間の定義

  SSD1306_init(&Wire1, 0x3C, 128, 64);
  ui_set_lcdpin(11, INPUT_PULLDOWN, 10, INPUT_PULLDOWN, 12, INPUT_PULLDOWN); // LCD用のボタンの定義
  ui_init();                                                                 // 定義
}

void loop()
{
  IRserial_update();     // 更新
  LINEserial_update();   // 更新
  OpenMVserial_update(); // 更新
  BNO055_update();       // 更新

  ui_process(); // モードを選ばせるー＞LCDアニメーションの実行

  //kicker_kick(1); // 決定ボタンが押されたら蹴る

  if (is_now_selecting_ui()) // 今選んでる途中なら
  {
    Serial.println("Now selecting");
  }
  else // 今選んだ
  {
    switch (get_selected_ui_action())
    {
    case ACTION_ATTACKER:
      play_pc_print(ALL_CHECK_WITH_PC); // 全て確認する
      switch (get_selected_ui_mode())
      {
      case PD_USE_ONLY_GYRO_MODE:
        play_attacker(false, 80); // ジャイロのみで動かす
        break;
      case PD_USE_CAM_MODE:
        play_attacker(true, 95); // カメラで動かす
        break;
      }

      break;

    case ACTION_DEFENDER:
      play_pc_print(ALL_CHECK_WITH_PC); // 全て確認する
      switch (get_selected_ui_mode())
      {
      case PD_USE_ONLY_GYRO_MODE:
        play_defender(false, 95); // ジャイロのみで動かす
        break;
      case PD_USE_CAM_MODE:
        play_defender(true, 95); // カメラで動かす
        break;
      }

      break;

    case ACTION_TEST:
      play_pc_print(ALL_CHECK_WITH_PC); // 全て確認する
      switch (get_selected_ui_mode())
      {
      case TEST_KICKER_MODE:
        play_test(TEST_KICKER_MODE);
        break;
      case TEST_PD_GYRO_MODE:
        play_test(TEST_PD_GYRO_MODE);
        break;
      case TEST_PD_CAM_MODE:
        play_test(TEST_PD_CAM_MODE);
        break;
      }

      break;

    default:
      Serial.println("error");

      break;
    }
  }
}