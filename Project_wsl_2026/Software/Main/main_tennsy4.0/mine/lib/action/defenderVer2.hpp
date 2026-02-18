#pragma once

// action類
#include "attacker.hpp"

// common類
#include "angleHelper.hpp"
#include "pd.hpp"
#include "timer.hpp"
#include "vector.hpp"
#include "lagrangeShifter.hpp"
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

void playDefenderVer2(Defender::Mode mode);

//// DEFENCE処理 ////
#define DEFENCE_IR_FRONT_Y_MAX 90.0f  // ボールのy座標がこの値から
#define DEFENCE_IR_FRONT_Y_MIN -90.0f // この値までであったら停止する

#define DEFENCE_YELLOW_GOAL_NORMAL_DEG 150 // 普通に守備をする角度(これを超えると対角線で守る)
#define DEFENCE_BLUE_GOAL_NORMAL_DEG 150

// DEFENCE認識条件-------------------------------------------------------------------
// USS
#define DEFENCE_USS_DIS_MIN 40 // ディフェンスの処理に移る最低の距離
// 1段階目
#define DEFENCE_YELLOW_GOAL_DIS1 80.0f // ディフェンスの処理に移る距離
#define DEFENCE_BLUE_GOAL_DIS1 80.0f

#define DEFENCE_YELLOW_GOAL_DEG1 120 // ディフェンスの処理に移る角度
#define DEFENCE_BLUE_GOAL_DEG1 120
// 2段階目(端に近いときは厳しい条件)
#define DEFENCE_YELLOW_GOAL_DIS2 80.0f // ディフェンスの処理に移る距離
#define DEFENCE_BLUE_GOAL_DIS2 80.0f

#define DEFENCE_YELLOW_GOAL_DEG2 90 // ディフェンスの処理に移る角度
#define DEFENCE_BLUE_GOAL_DEG2 90
// ---------------------------------------------------------------------------------

#define DEFENCE_LINE_TRACE_POWER_MAX 20.0f // ライントレースに使う力の上限

#define DEFENCE_YOKO_IR_FOLLOW_POWER_MAX 80.0f         // ボールの方向に行くときの力の上限
#define DEFENCE_TATE_ZENSHIN_IR_FOLLOW_POWER_MAX 80.0f // 縦　前進用
#define DEFENCE_TATE_KOUTAI_IR_FOLLOW_POWER_MAX 40.0f  // 縦　後退用
#define DEFENCE_KADO_IR_FOLLOW_POWER_MAX 70.0f         // 角
#define DEFENCE_HAJI_IR_FOLLOW_POWER_MAX 40.0f         // 端

#define DEFENCE_GO_CENTRAL_POWER 65.0f // ゴールの中心に行くパワー

//// MODORU処理 ////
#define MODORU_YELLOW_GOAL_DIS 68.0f // ゴール近距離を判断する距離とその時の力 (ゴールから逃げる・近寄る)
#define MODORU_BLUE_GOAL_DIS 68.0f

#define MODORU_POWER 75.0f       // この力で戻りながら
#define MODORU_IR_FOLLOW_POWER 0 // この力でボールを追う

//// TEIITI処理 ////
#define TEIITI_LINE_ESCAPE_POWER 70 // ラインが反応したときの力 (ラインから離れる動きの力)
#define TEIITI_POWER 70             // 定位置に戻るときの力

//// ATTACK処理 ////
#define ATTACK_IR_FOLLOW_POWER 80 // アタッカーのときの一時的にボールに追う動作をするときの力