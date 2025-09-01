#include "LINE.hpp"

LINE::LINE() {}

void LINE::init(HardwareSerial *serial, uint32_t baudrate, uint8_t frameHeader)
{
    _serial = serial;
    _baudrate = baudrate;
    _frameHeader = frameHeader;

    _serial->begin(_baudrate);

    // 初期化
    detected = false;
    deg = degMemory = -1;
    dis = -1;
    sideRight = sideLeft = sideBack = false;

    for (int i = 0; i < 19; i++)
        sensor[i] = sensorMemory[i] = false;
}

int LINE::degCompute(bool *data)
{
    if (!detected)
        return -1;

    x = 0;
    y = 0;
    for (int i = 0; i < 16; i++)
    {
        if (data[i])
        {
            x += cos(radians(22.5 * i));
            y += sin(radians(22.5 * i));
        }
    }

    if (sideRight)
    {
        x += cos(radians(90));
        y += sin(radians(90));
    }
    if (sideLeft)
    {
        x += cos(radians(270));
        y += sin(radians(270));
    }
    if (sideBack)
    {
        x += cos(radians(180));
        y += sin(radians(180));
    }

    return (int)round(degrees(atan2(y, x)));
}

void LINE::update()
{
    // データの読み取り
    while (_serial->available() >= 4)
    {
        while (_serial->available() > 0 && _serial->peek() != _frameHeader)
            _serial->read();

        if (_serial->available() < 4)
            break;

        if (_serial->peek() == _frameHeader)
        {
            _serial->read(); // ヘッダー読み捨て

            uint8_t low = _serial->read();
            uint8_t middle = _serial->read();
            uint8_t high = _serial->read();

            uint32_t bit_mask = ((uint32_t)high << 16) | ((uint32_t)middle << 8) | (uint32_t)low;
            detected = (bit_mask > 0);

            for (int i = 0; i < 19; i++)
                sensor[i] = (bit_mask & (1UL << i)) != 0;

            sideRight = sensor[16];
            sideLeft = sensor[17];
            sideBack = sensor[18];
        }
        else
        {
            _serial->read();
        }
    }

    // データの加工
    if (detected)
    {
        // 瞬間角度・距離
        deg = degCompute(sensor);
        if (deg < 0)
            deg += 360;
        dis = sqrt(x * x + y * y);

        // 記憶角度
        for (int i = 0; i < 19; i++)
        {
            if (sensor[i])
                sensorMemory[i] = true;
        }

        degMemory = degCompute(sensorMemory);
        if (degMemory < 0)
            degMemory += 360;
    }
    else
    {
        // 初期化
        deg = degMemory = -1;
        dis = -1;
        for (int i = 0; i < 19; i++)
            sensorMemory[i] = false;
    }
}
