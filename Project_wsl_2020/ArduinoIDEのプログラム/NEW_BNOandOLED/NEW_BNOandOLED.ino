/*OLED・BNO以外のライブラリ*/
#include <math.h>
#include <Arduino.h>

/*定数定義*/
#define SSD1306_ADDRESS_NUM 0x3C  //SSD1306のアドレス番号
#define BNO055_ADDRESS_NUM 0x29   //BNO055のアドレス番号
#define GY_RESETPIN_NUM 2         //ジャイロリセットボタンのピン番号
#define RESETPIN_PUSH_NUM 1900    //どのくらいの時間リセットピンが押されたらキャリブレーションモードに移るか
#define AVERAGE_COUNT_NUM 400     //キャリブレーション値を出すために何回値を取得するか
#define LOOP_DELAY_NUM 10         //どんくらいの間隔でプログラムを回すか

/*変数定義*/
int GY_deg;                                                              //計算したジャイロの値を格納
int reset_deg;                                                           //リセットボタンが押されたときのジャイロの値を格納
float accel_deg;                                                         //加速度を元にどの方向に移動したか求める
int Touch_keep_num;                                                      //どのくらいの時間リセットボタンが押され続けたか記録する
float calibration_accel_xx, calibration_accel_yy, calibration_accel_zz;  //x,y,z方向の加速度のキャリブレーション値

/*関数定義*/
void bno_init();                                                                              //BNO055を初期化する
int bno_process();                                                                            //BNO055からの値を計算して返す
void accel_calibration();                                                                     //すべての方向の加速度のキャリブレーション値を更新する
float get_accel(char accel_type[]);                                                           //加速度を取得する　get_accel(どの方向の加速度？)
void put_words(int CursorX_num, int CursorY_num, int Size_num, char Color[], char Words[]);   //ディスプレイに文字を表示させる　put_words(何列目に？, 何行目に？, サイズは？, 何色？, 何という文字？);
void put_number(int CursorX_num, int CursorY_num, int Size_num, char Color[], float number);  //ディスプレイに数字を表示させる　put_words(何列目に？, 何行目に？, サイズは？, 何色？, 何という数字？);
void put_circle_meter(int ori_X, int ori_Y, int meter_R, float meter_deg);                    //ディスプレイに円盤のメーターを表示させる　put_circle_meter(メーター原点X座標, メーター原点Y座標, メーター半径, メーターに表示させる角度);

/*SSD1306のセットアップ*/
#include <Wire.h>
#include <Adafruit_GFX.h>  //ディスプレイのAdarfuitのライブラリーを読み込む
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display(128, 64, &Wire);  // ディスプレイ用のI2Cを指定

/*BNO055のセットアップ*/
#include <Adafruit_BNO055.h>  //BNOのAdarfuitのライブラリーを読み込む
#include <Adafruit_Sensor.h>
#include <utility/imumaths.h>
#include <SPI.h>
Adafruit_BNO055 bno = Adafruit_BNO055(55, BNO055_ADDRESS_NUM, &Wire);  // BNO用のI2Cを指定

//プログラム

void setup() {
  Wire.begin();                                              //I2C通信を開始
  display.begin(SSD1306_SWITCHCAPVCC, SSD1306_ADDRESS_NUM);  // ディスプレイを開始
  display.clearDisplay();
  pinMode(GY_RESETPIN_NUM, INPUT_PULLUP);  //ジャイロのリセットピンの定義
  Serial.begin(9600);                      //シリアルモニターの定義
  bno_init();                              //BNO055の開始
}
void loop() {
  /*角度についてのプログラム*/
  GY_deg = bno_process();                     //GY_degにBNOの値を代入
  put_words(0, 0, 1, "WHITE", "BNO:");        //ディスプレイにBNOの値を表示させる
  put_number(24, 0, 1, "WHITE", GY_deg);      //ディスプレイにBNOの値を表示させる
  put_circle_meter(32, 38, 25, GY_deg + 90);  //BNOの値をメーターに表示させる

  /*移動方向についてのプログラム*/

  /*シリアルプリントについてのプログラム*/
  Serial.print(GY_deg);             //ジャイロの表示
  Serial.print("\t");               //タブスペースを空ける
  Serial.print(accel_deg);          //移動した角度の表示
  Serial.print("\t");               //タブスペースを空ける
  Serial.print(get_accel("xx"));    //X方向の加速度の表示
  Serial.print("\t");               //タブスペースを空ける
  Serial.print(get_accel("yy"));    //Y方向の加速度の表示
  Serial.print("\t");               //タブスペースを空ける
  Serial.println(get_accel("zz"));  //Z方向の加速度の表示

  /*キャリブレーション・ジャイロリセット*/  //キャリブレーション中のアニメーションはaccel_calibration()にあり
  if (digitalRead(GY_RESETPIN_NUM) == LOW) {
    imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);  //BNO055から値を取得
    reset_deg = euler.x();                                                //ジャイロ補正のために値を代入
    accel_calibration();                                                  //キャリブレーション開始
  }                                                                       //リセットボタンが押された場合

  /*ディスプレイに表示*/
  display.display();       //最終的にディスプレイに表示させる
  delay(LOOP_DELAY_NUM);   //LOOP_DELAY_NUM分だけ待つ
  display.clearDisplay();  //画面クリア
}

//関数（サブプログラム）

void bno_init() {
  delay(250);

  if (!bno.begin()) {
    Serial.println("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    put_words(0, 0, 1, "WHITE", "Check Your BNO");
    while (!bno.begin()) {
    }
  }  //BNOが動いていなかった場合に報告する

  bno.setExtCrystalUse(true);  //BNO開始
}

int bno_process() {
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);  //BNO055から値を取得
  GY_deg = euler.x();
  int new_deg = (int)(GY_deg - reset_deg + 360) % 360;  //ジャイロの値を計算

  return new_deg;  //計算した値を返す
}

void accel_calibration() {
  float average_accel_xx = 0.000;  //加速度のキャリブレーション値を格納
  float average_accel_yy = 0.000;
  float average_accel_zz = 0.000;
  for (int i = 0; i < AVERAGE_COUNT_NUM; i++) {
    imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
    sensors_event_t event;  //加速度データを取得

    average_accel_xx += accel.x();  //値を格納する
    average_accel_yy += accel.y();
    average_accel_zz += accel.z();
    delay(1);  //0.001秒待つ
  }
  calibration_accel_xx = float(average_accel_xx / AVERAGE_COUNT_NUM);  //平均値を出す
  calibration_accel_yy = float(average_accel_yy / AVERAGE_COUNT_NUM);
  calibration_accel_zz = float(average_accel_zz / AVERAGE_COUNT_NUM);
}

float get_accel(char accel_type[]) {
  imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  sensors_event_t event;  //加速度データを取得

  float accel_value = 0.0;  //加速度をこの変数に格納
  if (accel_type == "xx") {
    accel_value = accel.x() - calibration_accel_xx;
  } else if (accel_type == "yy") {
    accel_value = accel.y() - calibration_accel_yy;
  } else {
    accel_value = accel.z() - calibration_accel_zz;
  }

  //if (abs(accel_value) < 0.05) accel_value = 0;
  return accel_value;
}

void put_words(int CursorX_num, int CursorY_num, int Size_num, char Color[], char Words[]) {
  display.setCursor(CursorX_num, CursorY_num);  //何列目の何行目に表示させるか
  if (Color == "WHITE") {
    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);  // 白の文字、黒の背景を設定
  } else {
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);  // 黒の文字、白の背景を設定
  }
  display.setTextSize(Size_num);  //サイズを指定
  display.print(Words);           //文字を表示
}

void put_number(int CursorX_num, int CursorY_num, int Size_num, char Color[], float number) {
  display.setCursor(CursorX_num, CursorY_num);  //何列目の何行目に表示させるか
  if (Color == "WHITE") {
    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);  // 白の文字、黒の背景を設定
  } else {
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);  // 黒の文字、白の背景を設定
  }
  display.setTextSize(Size_num);   //サイズを指定
  int int_type_num = (int)number;  //表示する値が小数を含むのか含まないのか判断
  if (number == int_type_num) {
    display.print(int(number));
  } else {
    display.print(float(number));
  }  //小数を含まないならint型で表示、含むならfloat型で表示
}

void put_circle_meter(int ori_X, int ori_Y, int meter_R, float meter_deg) {
  float meter_xx = sin((meter_deg + 89) * PI / 180) * meter_R + ori_X;
  float meter_yy = cos((meter_deg + 89) * PI / 180) * meter_R + ori_Y;
  display.drawCircle(ori_X, ori_Y, meter_R, SSD1306_WHITE);           //円盤を表示
  display.drawLine(ori_X, ori_Y, meter_xx, meter_yy, SSD1306_WHITE);  //ラインを表示                                                //ディスプレイに表示
}
