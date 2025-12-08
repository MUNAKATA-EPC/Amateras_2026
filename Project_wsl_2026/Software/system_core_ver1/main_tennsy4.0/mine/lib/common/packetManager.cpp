/*#include "packetManager.hpp"

// メモリ解放
PacketManager::~PacketManager()
{
    if (_data != nullptr)
    {
        delete[] _data;
        _data = nullptr;
    }
}

void PacketManager::setup(uint8_t start_header, int byte_size, uint8_t end_header)
{
    // 既にメモリが確保されていたら一度解放する (再セットアップへの対応)
    if (_data != nullptr)
    {
        delete[] _data;
        _data = nullptr;
    }

    _start_header = start_header;
    _byte_size = byte_size;
    _end_header = end_header;

    // スタートヘッダー1バイト + データ(byte_size)バイト + エンドヘッダー1バイト
    _data_capacity = _byte_size + 2;

    // byte_sizeが無効値でないかチェック
    if (_byte_size <= 0 || _data_capacity > 255)
    {
        _data_capacity = 0; // 無効な状態
        return;
    }

    // メモリを動的に確保
    _data = new uint8_t[_data_capacity];

    // 確保失敗時のチェック
    if (_data == nullptr)
    {
        _data_capacity = 0;
        _byte_size = 0;
    }

    // 初期化
    for (int i = 0; i < _data_capacity; i++)
    {
        _data[i] = 0;
    }

    reset();
}

void PacketManager::reset()
{
    _next_index = 0;
}

void PacketManager::add(uint8_t byte)
{
    // メモリが確保されていない、または無効なサイズの場合は処理しない
    if (_data == nullptr || _data_capacity == 0)
        return;

    // 既にパケットが完成している場合はリセットしてから追加を開始
    if (isComplete())
    {
        reset();
        _next_index = 0;
    }

    // スタートヘッダーを待機中
    if (_next_index == 0)
    {
        if (byte == _start_header)
        {
            _data[_next_index] = byte;
            _next_index++;
        }
        // スタートヘッダーでない場合は、リセット状態を維持し、次のバイトを待つ
    }
    // データ部分を受信中
    else if (_next_index >= 1 && _next_index <= _byte_size)
    {
        _data[_next_index] = byte;
        _next_index++;
    }
    // エンドヘッダーを受信中
    else if (_next_index == _data_capacity - 1) // _data_capacity - 1 は _byte_size + 1 と等しい
    {
        if (byte == _end_header)
        {
            _data[_next_index] = byte;
            _next_index++;
        }
        else
        {
            reset();
        }
    }
    else
    {
        reset();
    }
}

bool PacketManager::isComplete()
{
    // _next_index がパケット全体のサイズ (_data_capacity) と等しいとき、全バイト受信済み
    if (_data_capacity > 0 && _next_index == _data_capacity)
        return true;

    return false;
}

uint8_t PacketManager::get(int index)
{
    // メモリが確保されていない、または無効なインデックスの場合はエラー
    if (_data == nullptr || index < 0 || index >= _data_capacity)
        return 0x00;

    return _data[index];
}*/

#include "packetManager.hpp"

void PacketManager::setup(uint8_t start_header, int data_size, uint8_t end_header)
{
    _start_header = start_header;
    byte_size = data_size;
    _end_header = end_header;

    for (int i = 0; i < PACKET_MAX_SIZE; i++)
    {
        _data[i] = 0;
    }
}

void PacketManager::reset()
{
    _next_index = 0;

    for (int i = 0; i < PACKET_MAX_SIZE; i++)
    {
        _data[i] = 0;
    }
}

void PacketManager::add(uint8_t byte)
{
    if (_next_index == 0)
    {
        if (byte == _start_header)
        {
            _data[_next_index] = byte;
            _next_index++;

            return;
        }
    }
    else if (_next_index < byte_size + 1)
    {
        _data[_next_index] = byte;
        _next_index++;

        return;
    }
    else if (_next_index == byte_size + 1)
    {
        if (byte == _end_header)
        {
            _data[_next_index] = byte;
            _next_index++;

            return;
        }
    }

    reset();
    return;
}

bool PacketManager::isComplete()
{
    if (_next_index == byte_size + 2)
        return true;

    return false;
}

uint8_t PacketManager::get(int index)
{
    if (index < 0 || index > byte_size + 1)
    {
        return 0;
    }

    return _data[index];
}