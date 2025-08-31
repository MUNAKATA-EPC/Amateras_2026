#ifndef MYSET_H

#define MYSET_H

/*　↓　速度・PID制御の設定　↓　*/
#define FRONTDEG 8 // 前とする角度を定義
#define KP 0.8     // KPの値を定義
#define KI 0.01    // KIの値を定義
#define KD 0.05    // KDの値を定義

/*　↓　タクトスイッチの設定　↓　*/
#define LEFTSWPIN 10                  // 左タクトスイッチのピン番号を指定
#define LEFTSWPINMODE INPUT_PULLDOWN  // 左タクトスイッチのピンモードを指定
#define RIGHTSWPIN 12                 // 右タクトスイッチのピン番号を指定
#define RIGHTSWPINMODE INPUT_PULLDOWN // 右タクトスイッチのピンモードを指定
#define ENTERSWPIN 11                 // Enterタクトスイッチのピン番号を指定
#define ENTERSWPINMODE INPUT_PULLDOWN // Enterタクトスイッチのピンモードを指定

/*　↓　トグルスイッチの設定　↓　*/
#define MOTORTOGGLEPIN 4 // モータのトグルスイッチのピン番号を指定
#define CAMTOGGLEPIN 5   // カメラのトグルスイッチのピン番号を指定

/*　↓　キッカーの設定　↓　*/
#define FET1_KICKPIN 2           // FET1のピンを指定
#define FET2_CHARGEPIN 3         // FET2のピンを指定
#define KICKER_COOLDOWNTIME 1000 // キッカーを一度使った後どのくらい休憩するか

//*　↓　I2C通信で動くものたちの設定　↓　*//
// Wire:SDAが18,SCLが19
// Wire1:SDAが17,SCLが16
// Wire2:SDAが24,SCLが25

/*SSD1306の設定*/
#define SSD1306_WIRE Wire1  // 使うI2Cピンを定義
#define SSD1306_ADRESS 0x3C // I2Cアドレスを指定
#define SSD1306_WIDTH 128   // 横幅を指定
#define SSD1306_HEIGH 64    // 縦幅を指定
/*BNO055の設定*/
#define BNO055_WIRE Wire                     // BNO055用のI2Cピンを定義
#define BNO055_ADRESS 0x28                   // I2Cアドレスを指定
#define BNO055_ID 55                         // センサーのIDを指定(適当な数でいい)
#define BNO055_RESETSWPIN 9                  // BNO055のリセットスイッチのピン番号を指定
#define BNO055_RESETSWPINMODE INPUT_PULLDOWN // BNO055のリセットスイッチのピンモードを指定

//*　↓　シリアル通信で動くものたちの設定　↓　*//
// Serial1:TXが1,RXが0
// Serial2:TXが10,RXが9
// Serial3:TXが8,RXが7
// Serial4:TXが32,RXが31
// Serial5:TXが20or33,RXが21or34
// Serial6:TXが24,RXが25
// Serial7:TXが14,RXが15

/*DSR1202の設定*/
#define DSR1202_SERIAL Serial2     // 使うシリアルピンを定義
#define DSR1202_SERIALSPEED 115200 // シリアル通信の速度を指定

/*IRの設定*/
#define IRXIAO_SERIAL Serial1   // 使うシリアルピンを定義
#define IRXIAO_SERIALSPEED 9600 // シリアル通信の速度を指定

/*LINEの設定*/
#define LINEXIAO_SERIAL Serial5   // 使うシリアルピンを定義
#define LINEXIAO_SERIALSPEED 9600 // シリアル通信の速度を指定

/*M5STAMP-picoの設定*/
// ps3
#define M5STAMPPS3_SERIAL Serial2   // 使うシリアルピンを定義
#define M5STAMPPS3_SERIALSPEED 9600 // シリアル通信の速度を指定
#define L_ADJUSTNUM 7               // 左スティックの調節用の数値
#define R_ADJUSTNUM 7               // 右スティックの調節用の数値
// communicate
#define M5STAMPCOMMUNICATE_SERIAL Serial2   // 使うシリアルピンを定義
#define M5STAMPCOMMUNICATE_SERIALSPEED 9600 // シリアル通信の速度を指定

/*OpenMVの設定*/
#define OPENMV_SERIAL Serial3     // 使うシリアルピンを定義
#define OPENMV_SERIALSPEED 115200 // シリアル通信の速度を指定

#endif