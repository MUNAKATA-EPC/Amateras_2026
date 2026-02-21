#include <Arduino.h>
#include <Wire.h>
#include <LSM6DSV16XSensor.h>

// ADD_L = 0x6A もしくは ADD_H = 0x6B（センサーのピン接続による）
LSM6DSV16XSensor AccGyr(&Wire, LSM6DSV16X_I2C_ADD_L);

int32_t acc[3];
int32_t gyr[3];

void setup()
{
  Serial.begin(9600);
  Serial.println("LSM6DSV16X START");

  // ----------------------------
  // I2C 初期化（最重要）
  // ----------------------------
  Wire.begin();
  Wire.setClock(100000); // ← XIAO対策（400kHz禁止）
  delay(500);            // ← センサー起動待ち（重要）

  // ----------------------------
  // センサー初期化
  // ----------------------------
  if (AccGyr.begin() != LSM6DSV16X_OK)
  {
    Serial.println("IMU begin FAILED");
    while (1)
      ;
  }

  // WHO_AM_I確認（通信正常チェック）
  uint8_t id;
  AccGyr.ReadID(&id);
  Serial.print("WHO_AM_I = 0x");
  Serial.println(id, HEX);

  if (id != 0x70)
  {
    Serial.println("Wrong device ID");
    while (1)
      ;
  }

  // ----------------------------
  // 安定化設定（ST推奨順）
  // ----------------------------

  // データ更新保護
  AccGyr.Enable_Block_Data_Update();

  // ODR先設定（Enableより先）
  AccGyr.Set_X_ODR(60.0f);
  AccGyr.Set_G_ODR(60.0f);

  // センサーON
  AccGyr.Enable_X();
  AccGyr.Enable_G();

  delay(100);

  Serial.println("IMU READY");
}

void loop()
{
  uint8_t acc_ready = 0;
  uint8_t gyr_ready = 0;

  // 新しいデータが来たか確認
  AccGyr.Get_X_DRDY_Status(&acc_ready);
  AccGyr.Get_G_DRDY_Status(&gyr_ready);

  // 両方更新されたときのみ取得
  if (acc_ready && gyr_ready)
  {
    if (AccGyr.Get_X_Axes(acc) == LSM6DSV16X_OK &&
        AccGyr.Get_G_Axes(gyr) == LSM6DSV16X_OK)
    {
      Serial.print("ACC: ");
      Serial.print(acc[0]);
      Serial.print(", ");
      Serial.print(acc[1]);
      Serial.print(", ");
      Serial.print(acc[2]);

      Serial.print(" | GYR: ");
      Serial.print(gyr[0]);
      Serial.print(", ");
      Serial.print(gyr[1]);
      Serial.print(", ");
      Serial.println(gyr[2]);
    }
  }
  else
  {
    Serial.print("no new data:");
    Serial.println(millis());
  }
}
