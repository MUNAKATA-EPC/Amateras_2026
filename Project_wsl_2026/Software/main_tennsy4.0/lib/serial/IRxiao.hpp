#ifndef IRXIAO_HPP
#define IRXIAO_HPP

#include <Arduino.h>

class IRxiao
{
private:
    HardwareSerial *_serial;
    int _baudrate;

    bool _exist;

    int _deg;
    int _distance;

public:
    IRxiao(HardwareSerial *serial); // 定義
    void begin(int baudrate);       // シリアル開始
    void update();                  // 更新

    bool isExist(); // ボールが存在するかどうか

    int getDeg();      // ボールの方向
    int getDistance(); // ボールまでの距離
};

#endif
