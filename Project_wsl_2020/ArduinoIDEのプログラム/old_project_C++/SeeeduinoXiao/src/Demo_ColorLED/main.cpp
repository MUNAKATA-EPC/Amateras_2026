#include <Arduino.h>
#include "USE_ColorLED.h"
#include "USE_m5pico.h"

const int m5pico_TalkStart_ms = 4500;

void setup()
{
    Serial.begin(9600);
    
    M5PICObegin(9600);  // D6をRX,D7をTXとしている。通信速度は9600でM5-picoと通信開始
    Stick_Adjust(8, 8); // AnalogStickの-1とする範囲の調整。X・Yどちらも±5は-1としている。

    ColorLEDbegin(8, 9, 10); // LEDに使うピンを指定
}

void loop()
{
    M5PICOget();
    LEDdeg(LStick_Angle, 1.0, 1.0);
    Serial.println(LStick_Angle);
    delay(100);
}
