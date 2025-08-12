#include <Arduino.h>
#include <Ps3Controller.h>

/*m5stamp-picoのアドレスを取得する*/

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ; // PCとつながるまで待つ

  // m5stamp-picoのアドレスを取得
  uint8_t btmac[6];
  esp_read_mac(btmac, ESP_MAC_BT);
  Serial.printf("[Bluetooth] Mac Address = %02X:%02X:%02X:%02X:%02X:%02X\r\n", btmac[0], btmac[1], btmac[2], btmac[3], btmac[4], btmac[5]);
}

void loop()
{
}