//通信準備
#include <Wire.h>
#include <SparkFunLSM9DS1.h>
LSM9DS1 imu;             // LSM9DS1 Library Init
#define LSM9DS1_M 0x1C   // コンパスのI2C初期アドレス
#define LSM9DS1_AG 0x6A  // 加速度とジャイロのI2C初期アドレス
#define bright 50        //LEDの明るさ

//初期設定
#define PRINT_SPEED 20.00   // ? ms毎にデータを出力
#define GYRO_SPLIT 1000.00  //出力したデータを何で割るか
#define DECLINATION 7.46    // コンパスの値

//変数一覧
float Gyro_add = 0.00, Gyro = 0.00;
int edge = 0, dac = 0;
static unsigned long lastPrint = 0;  // 最後の出力時刻をキープ

void setup() {
  //ピン設定
  pinMode(0, OUTPUT);
  pinMode(8, INPUT);    //Resisitar
  pinMode(7, INPUT);    //tact_switch
  pinMode(8, OUTPUT);   //LED_PWM
  pinMode(9, OUTPUT);   //LED_RIGHT
  pinMode(10, OUTPUT);  //LED_LEFT

  //シリアルモニターを始める
  Serial.begin(9600);

  //I2Cを始める
  Wire.begin();

  //IMUとの接続を確かめる
  if (imu.begin(LSM9DS1_AG, LSM9DS1_M, Wire) == false)  //IMUの起動を判定
  {
    Serial.println("Failed to communicate with LSM9DS1.");
  }

  //自動補正の値の設定
  Gyro_set();
}
void loop() {
  //可変抵抗からのデータを格納
  float Resis_add = analogRead(8) / 100.000;
  Resis_add -= 5.115;

  //Gyroに格納する値の設定(毎 PRINT_SPEED msごと)
  if ((lastPrint + PRINT_SPEED) < millis())  //更新タイミングが来たらデータを表示
  {
    if (digitalRead(7) == LOW) {
      //基準値の再設定
      Gyro = 0;
      Serial.print(" ");
      //自動補正の値の設定
      Gyro_set();
    }

    //距離を算出＋補正
    imu.readGyro();
    Gyro = ((imu.calcGyro(imu.gz) * PRINT_SPEED * PRINT_SPEED) / (2.00 * GYRO_SPLIT)) + Gyro + Resis_add - Gyro_add;

    edge = int(Gyro) / 10;
    if (edge > 302) {
      dac = 1023;
    } else if (edge < -267) {
      dac = 0;
    } else {
      dac = ((3 * edge) + 800) * 0.6;
    }
    //edgeの範囲を調整
    //シリアルモニターに表示
    Serial.print(edge);
    Serial.print("   ");
    Serial.println(Resis_add);

    //タイマーのリセット
    lastPrint = millis();  // 次回出力時刻を設定
  }

  //LEDの発光設定
  LED_begin();

  //DACの設定
  DAC_begin();
}

void Gyro_set() {
  //PRINT_SPEED msでどれだけジャイロがズレたか
  imu.readGyro();
  Gyro_add = (imu.calcGyro(imu.gz)) / GYRO_SPLIT;
  delay(PRINT_SPEED);
  imu.readGyro();
  Gyro_add = Gyro_add - (imu.calcGyro(imu.gz)) / GYRO_SPLIT;

  analogWrite(0, 530);
  analogWrite(9, bright);
  analogWrite(10, bright);
  delay(250);
  analogWrite(9, 0);
  analogWrite(10, 0);
  delay(250);
}

void LED_begin() {
  if (dac > 480) {
    analogWrite(9, bright);
    analogWrite(10, 0);
  } else if (dac == 480) {
    analogWrite(9, 0);
    analogWrite(10, 0);
  } else {
    analogWrite(10, bright);
    analogWrite(9, 0);
  }
}

void DAC_begin() {
  analogWrite(0, dac);
  Serial.print(dac);
  Serial.print("   ");
  Serial.println(edge);
}