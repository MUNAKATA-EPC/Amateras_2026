#pragma once

#include "Arduino.h"
#define PACKET_MAX_SIZE 10

class Packet_manager
{
private:
    uint8_t _data[PACKET_MAX_SIZE];
    int _next_index = 0;

    uint8_t _start_header = 0x55;         // 最初のヘッダーを読んで
    int _byte_size = PACKET_MAX_SIZE - 2; // 8バイト分のデータを読んで
    uint8_t _end_header = 0xAA;           // 最後のヘッダーを読む

public:
    void setup(uint8_t start_header, int byte_size, uint8_t end_header); // パケットのフォーマットを設定する

    void reset();           // パケット読み取りを最初からやり直す
    void add(uint8_t byte); // 1バイト追加する

    bool isComplete();      // パケットが完成しているかどうか
    uint8_t get(int index); // index番目のデータを取得する
};