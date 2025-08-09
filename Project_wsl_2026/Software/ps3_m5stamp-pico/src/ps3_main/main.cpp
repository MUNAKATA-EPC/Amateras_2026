#include <Arduino.h>
#include <Ps3Controller.h>

int stick_lx, stick_ly, stick_rx, stick_ry; // それぞれのコントローラの情報が格納用(範囲は-128~127らしい)
unsigned int button_data_10 = 0;            // SWの情報を10進数で格納用

// PS3が接続されると呼ばれる関数
void connect_success();
// PS3が切断されると呼ばれる関数
void connect_false();
// PS3のどれが反応すると呼ばれる関数
void survey_data();

void setup()
{
  Serial.begin(115200); // 115200bpsでシリアル通信を開始

  Ps3.attach(survey_data);               // PS3のどれが反応すると呼ばれる関数
  Ps3.attachOnConnect(connect_success);  // 接続されたときに呼び出す関数
  Ps3.attachOnDisconnect(connect_false); // 切断されたときに呼び出す関数
  Ps3.begin("E8:6B:EA:31:0A:3A");        // ps3_setupで取得したアドレスを記入

  while (!Ps3.isConnected()) // 接続されるまで待つ
    ;
}

void loop()
{
  Serial.println(stick_lx); // 左スティックのX方向
  Serial.print("a");
  Serial.println(stick_ly); // 左スティックのY方向
  Serial.print("b");
  Serial.println(stick_rx); // 右スティックのX方向
  Serial.print("c");
  Serial.println(stick_ry); // 右スティックのY方向
  Serial.print("d");
  Serial.println(button_data_10); // buttonの情報を送信
  Serial.print("e");

  delay(10); // 10ms待機
}

// それぞれの関数のプログラム

void connect_success()
{
  Ps3.setRumble(1, 1000); // 振動1を1000msする
}

void connect_false()
{
  while (!Ps3.isConnected()) // 接続されるまで待つ
    ;
}

void survey_data()
{
  // アナログスティックの情報を取得
  stick_lx = Ps3.data.analog.stick.lx + 128; // 左スティックのX方向(範囲を0~255にする)
  stick_ly = Ps3.data.analog.stick.ly + 128; // 左スティックのY方向(範囲を0~255にする)
  stick_rx = Ps3.data.analog.stick.rx + 128; // 右スティックのX方向(範囲を0~255にする)
  stick_ry = Ps3.data.analog.stick.ry + 128; // 右スティックのY方向(範囲を0~255にする)

  // ボタンの情報を取得(押されたら加算)
  button_data_10 = 0; // 初期化

  if (Ps3.data.button.up) // 上
    button_data_10 += pow(2, 0);
  if (Ps3.data.button.down) // 下
    button_data_10 += pow(2, 1);
  if (Ps3.data.button.left) // 左
    button_data_10 += pow(2, 2);
  if (Ps3.data.button.right) // 右
    button_data_10 += pow(2, 3);

  if (Ps3.data.button.triangle) // △
    button_data_10 += pow(2, 4);
  if (Ps3.data.button.circle) // ○
    button_data_10 += pow(2, 5);
  if (Ps3.data.button.cross) // ×
    button_data_10 += pow(2, 6);
  if (Ps3.data.button.square) // □
    button_data_10 += pow(2, 7);

  if (Ps3.data.button.l1) // L1
    button_data_10 += pow(2, 8);
  if (Ps3.data.button.l2) // L2
    button_data_10 += pow(2, 9);
  if (Ps3.data.button.l3) // L3
    button_data_10 += pow(2, 10);

  if (Ps3.data.button.r1) // R1
    button_data_10 += pow(2, 11);
  if (Ps3.data.button.r2) // R2
    button_data_10 += pow(2, 12);
  if (Ps3.data.button.r3) // R3
    button_data_10 += pow(2, 13);
}