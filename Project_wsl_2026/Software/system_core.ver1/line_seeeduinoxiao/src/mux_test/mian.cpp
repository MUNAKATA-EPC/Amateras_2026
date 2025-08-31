#include <Arduino.h>
#include "multiplexer.hpp"

/*マルチプレクサのテスト用*/

Multiplexer line_mux; // 定義

void setup()
{
    Serial.begin(9600);

    line_mux.set_pin(1, 2, 3, 4, 5, -1); // ピンを指定
    line_mux.init(10);                   // 時間を指定
}

void loop()
{
    for (int i = 0; i < 15; i++)
    {
        Serial.print("pin");
        Serial.print(i);
        Serial.print(":");
        Serial.print(line_mux.read(i));
        Serial.print(",");
    }
    Serial.print("\n");

    delay(10); // 10ms待機
}
