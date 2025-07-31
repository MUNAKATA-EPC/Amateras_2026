#ifndef LINEXIAO_HPP
#define LINEXIAO_HPP

#include <Arduino.h>
#include "function.hpp"

class LINExiao
{
private:
    HardwareSerial *_serial;
    int _baudrate;

    bool _exist;

    bool _side_right;
    bool _side_left;
    bool _side_back;
    int _deg;

public:
    LINExiao(HardwareSerial *serial); // 定義
    void begin(int baudrate);         // シリアル開始
    void update();                    // 更新

    bool isExist(); // ラインが存在するかどうか

    int getDeg();         // ラインの方向
    bool getSideRight(); // 右サイド
    bool getSideLeft();  // 左サイド
    bool getSideBack();  // 後サイド
};

#endif