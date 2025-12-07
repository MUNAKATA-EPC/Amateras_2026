#pragma once

#include "Arduino.h"

class Packet_manager
{
private:
    uint8_t *_data = nullptr; // ヒープメモリへのポインタ
    int _data_capacity = 0;   // _dataが確保している最大サイズ (byte_size + 2)
    int _byte_size = 0;       // 受け取るデータのバイト数
    int _next_index = 0;      // 次に書き込む配列のインデックス

    uint8_t _start_header = 0x55;
    uint8_t _end_header = 0xAA;

public:
    ~Packet_manager(); // デストラクタ：確保したメモリを解放

    // コピー禁止 (安全のため)
    Packet_manager(const Packet_manager &) = delete;
    Packet_manager &operator=(const Packet_manager &) = delete;

    void setup(uint8_t start_header, int byte_size, uint8_t end_header);

    void reset();           // パケットリセット
    void add(uint8_t byte); // 1バイト追加する
    bool isComplete();      // パケットが完成しているかどうか
    uint8_t get(int index); // index番目のデータを取得する
};