#include <Arduino.h>
#include "multiplexer.hpp"

/*マルチプレクサのテスト用*/

Multiplexer mux; // 定義
int min_value[16] = {1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023};

void setup()
{
    Serial.begin(9600);

    mux.set_pin(1, 2, 3, 4, 0, -1); // ピンを指定
    mux.init(10);                   // 時間を指定
}

void loop()
{
    for (int i = 0; i < 16; i++)
    {
        int value = mux.read(i); // マルチプレクサから値を読む

        if (value < min_value[i])
            min_value[i] = value;

        Serial.print("m" + String(i) + ":" + String(min_value[i]) + ",");
    }
    Serial.print("\n");

    delay(10); // 10ms待機
}
