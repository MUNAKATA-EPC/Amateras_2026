#pragma once

// common類
#include "angleHelper.hpp"
#include "pd.hpp"
#include "timer.hpp"
#include "vector.hpp"
// driver類
#include "dribbler.hpp"
#include "kicker.hpp"
#include "motors.hpp"
#include "ui.hpp"
// sensor類
#include "bno.hpp"
#include "button.hpp"
#include "digitalSensor.hpp"
#include "ir.hpp"
#include "line.hpp"
#include "ps3.hpp"
#include "openmv.hpp"
#include "uss.hpp"

// manabuモードで一時的に格納するデータの配列
int reliable_subdata_counter[91]; // 角度ごとのカウンターを格納する配列
int deg_subdata[91][5];           // 角度ごとのデータを格納する配列
int power_subdata[91][5];         // 角度ごとのデータを格納する配列

// 最終的な回り込みデータの配列
int deg_data[91][5];   // 角度ごとのデータを格納する配列
int power_data[91][5]; // 角度ごとのデータを格納する配列

void playJAMVer2(JAM::Mode mode);
void printJAMVer2(JAM::Mode mode);