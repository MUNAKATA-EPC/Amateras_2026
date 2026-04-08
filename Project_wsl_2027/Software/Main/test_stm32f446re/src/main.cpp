#include <Arduino.h>

const int button_pin = PC13;
const int led_pin = LED_BUILTIN;

void setup()
{
    Serial.begin(115200);

    pinMode(led_pin, OUTPUT);
    pinMode(button_pin, INPUT_PULLUP);
}

void loop()
{
    bool btn_pressed = (digitalRead(button_pin) == LOW);
    static bool old_btn_pressed = btn_pressed;

    if (btn_pressed)
    {
        if (!old_btn_pressed)
        {
            uint32_t millis = ::millis();
            Serial.print("time:");
            Serial.print(millis);
            Serial.println("ms");
        }

        digitalWrite(led_pin, HIGH);
    }
    else
    {
        digitalWrite(led_pin, LOW);
    }

    delay(10);

    old_btn_pressed = btn_pressed;
}