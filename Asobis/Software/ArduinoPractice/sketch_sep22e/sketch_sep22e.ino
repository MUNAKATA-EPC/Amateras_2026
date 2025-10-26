#include <Sprite.h>
#include <Matrix.h>
Matrix myMatrix = Matrix(11, 13, 1);  //マトリクスLEDを使うときのオマジナイ

const int SW0 = 10; // ボタンのピン番号
unsigned long Last_Time = 0; // デバウンス処理のための時間
unsigned long Click_delay = 50; // デバウンスの遅延時間
unsigned long TapTime = 0; // タップの時間
unsigned long TapDelay = 350; // ダブルタップを検出するための遅延時間
bool buttonValue = HIGH; // ボタンの現在の状態
bool lastButtonValue = HIGH; // ボタンの前回の状態
bool singleTapPending = false; // シングルタップの保留フラグ

void setup() {
  myMatrix.setBrightness(3);        //マトリクスLEDの明るさを設定(0−8)

  pinMode(SW0, INPUT_PULLUP); // ボタンのピンを入力プルアップに設定
  //Serial.begin(9600); // シリアル通信を開始
}

void loop() {
  int result = checkButton(); // ボタンの状態をチェック
  if(result != 0){
    myMatrix.putd2(0,0,result);
    delay(500);
  }
  else{
    myMatrix.putd2(0,0,result);
  }
}

int checkButton() {
  int now_SW0 = digitalRead(SW0); // ボタンの状態を読み取る

  // ボタンの状態が変わった場合
  if (now_SW0 != lastButtonValue) {
    Last_Time = millis(); // 時間を更新
  }

  // デバウンス処理
  if ((millis() - Last_Time) > Click_delay) {
    // ボタンが押された場合
    if (now_SW0 == LOW && buttonValue == HIGH) {
      // 前回のタップからの時間が短い場合はダブルタップと判定
      if ((millis() - TapTime) < TapDelay) {
        buttonValue = now_SW0; // ボタンの状態を更新
        lastButtonValue = now_SW0; // ボタンの前回の状態を更新
        TapTime = millis(); // タップの時間を更新
        singleTapPending = false; // シングルタップの保留をリセット
        return 2; // ダブルタップ
      } 
      else {
        singleTapPending = true; // シングルタップの保留を設定
        TapTime = millis(); // タップの時間を更新
      }
    }
    buttonValue = now_SW0; // ボタンの状態を更新
  }

  // シングルタップの保留が設定されている場合
  if (singleTapPending && (millis() - TapTime) > TapDelay) {
    singleTapPending = false; // シングルタップの保留をリセット
    return 1; // シングルタップ
  }

  lastButtonValue = now_SW0; // ボタンの前回の状態を更新
  return 0; // 何も押されていない
}



