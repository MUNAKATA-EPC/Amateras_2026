#include <Arduino.h>
#include <Ps3Controller.h>

/*ps3の情報をシリアル通信で送る*/

const uint8_t head_byte = 0xAA; // 同期ヘッダー格納用

int8_t stick_lx, stick_ly, stick_rx, stick_ry; // それぞれのコントローラの情報が格納用(範囲は-128~127)
uint16_t buttons_data_bit_mask = 0;            // ボタンは14個なので16bitで足りる

// PS3が接続されると呼ばれる関数
void connect_success();
// PS3が切断されると呼ばれる関数
void connect_false();
// PS3のどれが反応すると呼ばれる関数
void data_update();

void setup()
{
  Serial.begin(115200); // 115200bpsでシリアル通信を開始

  Ps3.attach(data_update);               // PS3のどれが反応すると呼ばれる関数
  Ps3.attachOnConnect(connect_success);  // 接続されたときに呼び出す関数
  Ps3.attachOnDisconnect(connect_false); // 切断されたときに呼び出す関数
  Ps3.begin("4C:75:25:C4:14:F2");        // ps3_setupで取得したアドレスを記入
  // kaname_machineに搭載しているm5のアドレス : 4C:75:25:C4:14:F2
  // seidai_machineに搭載しているm5のアドレス : E8:6B:EA:31:0A:3A

  while (!Ps3.isConnected()) // 接続されるまで待つ
    ;z
}

void loop()
{
  Serial.write(head_byte);                  // teensyとの通信開始
  Serial.write(stick_lx);                   // 左ステックのx座標を送信
  Serial.write(stick_ly);                   // 左ステックのy座標を送信
  Serial.write(stick_rx);                   // 右ステックのx座標を送信
  Serial.write(stick_ry);                   // 右ステックのy座標を送信
  Serial.write(buttons_data_bit_mask);      // 2byteのデータなので下位の1byteのみ送信
  Serial.write(buttons_data_bit_mask >> 8); // 8bit分右にシフトして上位の1byteを送信

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

void data_update()
{
  // アナログスティックの情報を取得
  stick_lx = Ps3.data.analog.stick.lx; // 左スティックのX方向
  stick_ly = Ps3.data.analog.stick.ly; // 左スティックのY方向
  stick_rx = Ps3.data.analog.stick.rx; // 右スティックのX方向
  stick_ry = Ps3.data.analog.stick.ry; // 右スティックのY方向

  // ボタンの情報を取得(押されたら加算)
  buttons_data_bit_mask = 0; // 初期化

  if (Ps3.data.button.up) // 上
    buttons_data_bit_mask |= (1 << 0);
  if (Ps3.data.button.down) // 下
    buttons_data_bit_mask |= (1 << 1);
  if (Ps3.data.button.left) // 左
    buttons_data_bit_mask |= (1 << 2);
  if (Ps3.data.button.right) // 右
    buttons_data_bit_mask |= (1 << 3);

  if (Ps3.data.button.triangle) // △
    buttons_data_bit_mask |= (1 << 4);
  if (Ps3.data.button.circle) // ○
    buttons_data_bit_mask |= (1 << 5);
  if (Ps3.data.button.cross) // ×
    buttons_data_bit_mask |= (1 << 6);
  if (Ps3.data.button.square) // □
    buttons_data_bit_mask |= (1 << 7);

  if (Ps3.data.button.l1) // L1
    buttons_data_bit_mask |= (1 << 8);
  if (Ps3.data.button.l2) // L2
    buttons_data_bit_mask |= (1 << 9);
  if (Ps3.data.button.l3) // L3
    buttons_data_bit_mask |= (1 << 10);

  if (Ps3.data.button.r1) // R1
    buttons_data_bit_mask |= (1 << 11);
  if (Ps3.data.button.r2) // R2
    buttons_data_bit_mask |= (1 << 12);
  if (Ps3.data.button.r3) // R3
    buttons_data_bit_mask |= (1 << 13);
}