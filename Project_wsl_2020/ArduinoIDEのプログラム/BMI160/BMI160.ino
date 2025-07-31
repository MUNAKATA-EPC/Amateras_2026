#include <DFRobot_BMI160.h>

DFRobot_BMI160 bmi160;
const int8_t i2c_addr = 0x69;
const int buttonPin = 2;
double yawAngle = 0.0;  // ヨー角
unsigned long lastTime = 0;

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);
  delay(100);

  // BMI160初期化
  if (bmi160.softReset() != BMI160_OK) {
    Serial.println("reset false");
    while (1);
  }

  if (bmi160.I2cInit(i2c_addr) != BMI160_OK) {
    Serial.println("init false");
    while (1);
  }

  lastTime = millis();
}

void loop() {
  static double gyroZOffset = 0.0;
  static bool calibrated = false;
  int16_t accelGyro[6] = {0};
  unsigned long currentTime = millis();
  double dt = (currentTime - lastTime) / 1000.0;  // 時間間隔 (秒)
  lastTime = currentTime;

  // 加速度＆ジャイロデータ取得
  if (bmi160.getAccelGyroData(accelGyro) == 0) {
    double gyroZ = accelGyro[2] * 3.14 / 180.0;  // Z軸ジャイロデータ（rad/s）
    
    // キャリブレーション
    if (digitalRead(buttonPin) == LOW) {
      delay(400);
      Serial.println("Calibration started...");
      double sum = 0;
      for (int i = 0; i < 400; i++) {
        bmi160.getAccelGyroData(accelGyro);
        sum += accelGyro[2] * 3.14 / 180.0;
        delay(1);
      }
      gyroZOffset = sum / 400.0;
      yawAngle = 0.0;
      calibrated = true;
      Serial.println("Calibration completed!");
    }

    // オフセット補正 & ヨー角計算
    if (calibrated) {
      gyroZ -= gyroZOffset;
    }
    yawAngle += gyroZ * dt;
    
    // 出力
    Serial.print("Yaw Angle: ");
    Serial.println(yawAngle * 10 / 3);
  } else {
    Serial.println("Sensor error");
  }
  delay(10);
}











