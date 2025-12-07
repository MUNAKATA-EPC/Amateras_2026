#include "packet_manager.hpp"

void Packet_manager::setup(uint8_t start_header, int byte_size, uint8_t end_header)
{
    _start_header = start_header;
    _byte_size = byte_size;
    _end_header = end_header;

    for (int i = 0; i < PACKET_MAX_SIZE; i++)
    {
        _data[i] = 0;
    }
}

void Packet_manager::reset()
{
    _next_index = 0;
}

void Packet_manager::add(uint8_t byte)
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
    else if (_next_index < _byte_size + 1)
    {
        _data[_next_index] = byte;
        _next_index++;

        return;
    }
    else if (_next_index == _byte_size + 1)
    {
        if (byte == _end_header)
        {
            _data[_next_index] = byte;
            _next_index++;

            return;
        }
    }

    reset();
}

bool Packet_manager::isComplete()
{
    if (_next_index == _byte_size + 2)
        return true;

    return false;
}

uint8_t Packet_manager::get(int index)
{
    if (index < 0 || index > _byte_size + 1)
        return 0x00;

    return _data[index];
}