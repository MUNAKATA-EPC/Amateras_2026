#include "lineState.hpp"

float lineAbsoluteRingDeg()
{
    if (!lineRingDetected())
        return 0xFF;

    return normalizeDeg(lineRingDeg() - bnoDeg());
}

LinePosition linePositionCheck()
{
    LineChunk line_chunk[16];
    int line_chunk_count = lineChunkCount(line_chunk);

    if (line_chunk_count == 0)
    {
        return None;
    }
    else if (line_chunk_count == 1)
    {
        float absolute_line_chunk_deg = normalizeDeg(line_chunk[0].deg - bnoDeg());
        if (fabsf(absolute_line_chunk_deg) > 45 && fabsf(absolute_line_chunk_deg) < 135) // 縦線上にいる
        {
            return Tate;
        }
        else // 横線上にいる
        {
            return Yoko;
        }
    }
    else if (line_chunk_count == 2)
    {
        if (fabsf(lineAbsoluteRingDeg()) > 45 && fabsf(lineAbsoluteRingDeg()) < 135) // 縦線上にいる
        {
            return Tate;
        }
        else // 横線上にいる
        {
            return Yoko;
        }
    }
    else // if (line_chunk_count >= 3) // 端にいる
    {
        return Haji;
    }
}

Vector lineTrace(float move_deg, float move_power, float line_trace_power)
{
    if (!lineRingDetected())
        return Vector(0, 0);

    move_deg = normalizeDeg(move_deg); // 正規化

    Vector move_vec = Vector(nearSessenDeg(lineRingDeg(), move_deg), move_power);
    Vector line_trace_vec = Vector(lineRingDeg(), lineRingDis() * line_trace_power / 100.0f);
    Vector vec = move_vec + line_trace_vec;
    // vec = vec * (move_power + line_trace_power) / vec.length();
    return vec;
}
