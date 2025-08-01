#ifndef COMPUTE_HPP
#define COMPUTE_HPP

#include <Arduino.h>
#include "IRxiao.hpp"
#include "LINExiao.hpp"

class Compute
{
private:
    int _line_deg; // ラインセンサーの算出した角度を格納

    int _mawarikomi_deg; // 回り込みの角度を格納

public:
    Compute(); // 定義

    int computeLineDeg(LINExiao *line); // エンジェルラインの角度算出

    int computeMawarikomiDeg(IRxiao *irball);                   // 回り込み角度算出
    int computeFollowDeg(IRxiao *irball, double frontdistance); // ボールの向かう角度算出

    int computeGyroPower(); // PD制御の出力算出
};

#endif