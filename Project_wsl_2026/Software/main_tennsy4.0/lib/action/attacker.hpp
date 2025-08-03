#ifndef ATTACKER_HPP
#define ATTACKER_HPP

// IRについて
#include "IRserial.hpp"
#include "IRcompute.hpp"
// LINEについて
#include "LINEserial.hpp"
#include "LINEcompute.hpp"
// BNO055について
#include "BNO055.hpp"
// OpenMVについて
#include "OpenMV.hpp"
// DSR1202について
#include "DSR1202.hpp"
#include "motors.hpp"

// 攻撃用のプログラムを実行
void play_attacker();

#endif