#pragma once
#include <Arduino.h>
#include <Servo.h>

class Dribbler {
public:
    bool init(uint8_t pin, int min = 1000, int max = 2000);

    void initMoving();
    bool isInitMoving();

    void move(int power);

private:
    Servo _esc;
    uint8_t _pin;
    int _min;
    int _max;
    bool _flag = false;
    uint32_t _start_time;
};

extern Dribbler dribbler1;