#include <Arduino.h>
#include "motor.hpp"

Motor motors[4];

void setup()
{
  motors[0].set_pin(4, 6, 5);
  motors[1].set_pin(7, 10, 9);
  motors[2].set_pin(11, 19, 20);
  motors[3].set_pin(31, 32, 34);

  Serial.print(9600); // PC送信用

  Serial1.begin(115200);  // Teensy4.0受信用
  Serial1.setTimeout(10); // 受信タイムアウト10ms
}

void loop()
{
  String receive_data = "";     // 受信データの例："1F1002B1003F0014F043"
  bool is_read_success = false; // 受信に成功したかどうか

  if (Serial1.available() > 0)
  {
    receive_data = Serial1.readStringUntil('\n');
    receive_data.trim(); // 改行コード削除

    if (receive_data.length() == 20)
    {
      is_read_success = true;
    }

    Serial.print(receive_data); // デバッグ用
  }

  if (is_read_success)
  {
    if (receive_data == "1R0002R0003R0004R000")
    {
      for (int i = 0; i < 4; i++)
      {
        motors[i].stop();
      }
    }
    else
    {
      int power[4];
      power[0] = (String(receive_data[2]) + String(receive_data[3]) + String(receive_data[4])).toInt();
      power[1] = (String(receive_data[7]) + String(receive_data[8]) + String(receive_data[9])).toInt();
      power[2] = (String(receive_data[12]) + String(receive_data[13]) + String(receive_data[14])).toInt();
      power[3] = (String(receive_data[17]) + String(receive_data[18]) + String(receive_data[19])).toInt();

      char rotate[4];
      rotate[0] = receive_data[1];
      rotate[1] = receive_data[6];
      rotate[2] = receive_data[11];
      rotate[3] = receive_data[16];

      for (int i = 0; i < 4; i++)
      {
        if (rotate[i] == 'F')
          motors[i].move(power[i]);
        else if (rotate[i] == 'B')
          motors[i].move(-power[i]);
      }
    }
  }
}