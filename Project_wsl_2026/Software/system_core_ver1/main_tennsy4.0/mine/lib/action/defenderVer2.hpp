#pragma once

// action類
#include "attacker.hpp"

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

void playDefenderVer2(Defender::Mode mode);

//// DEFENCE処理 ////
#define DEFENCE_IR_FRONT_Y_MAX 8.8f  // ボールのy座標がこの値から
#define DEFENCE_IR_FRONT_Y_MIN -8.8f // この値までであったら停止する

#define DEFENCE_YELLOW_GOAL_NORMAL_DEG 150 // 普通に守備をする角度(これを超えると対角線で守る)
#define DEFENCE_BLUE_GOAL_NORMAL_DEG 150

#define DEFENCE_YELLOW_GOAL_DIS 80.0f // ディフェンスの処理に移る距離
#define DEFENCE_BLUE_GOAL_DIS 80.0f

#define DEFENCE_LINE_TRACE_POWER_MAX 30.0f // ライントレースに使う力の上限

#define DEFENCE_TATE_IR_FOLLOW_POWER_MAX 40.0f // ボールの方向に行くときの力の上限
#define DEFENCE_YOKO_IR_FOLLOW_POWER_MAX 40.0f
#define DEFENCE_KADO_IR_FOLLOW_POWER_MAX 40.0f

//// SAKAIME処理(DEFENCE処理の中にある) ////
#define SAKAIME_YELLOW_GOAL_DEG_MIN -120 // ゴールの横線の上限の角度
#define SAKAIME_YELLOW_GOAL_DEG_MAX 120

#define SAKAIME_BLUE_GOAL_DEG_MIN -120
#define SAKAIME_BLUE_GOAL_DEG_MAX 120

//// MODORU処理 ////
#define MODORU_YELLOW_GOAL_DIS 70.0f // ゴール近距離を判断する距離とその時の力 (ゴールから逃げる・近寄る)
#define MODORU_BLUE_GOAL_DIS 70.0f

#define MODORU_POWER 40           // この力で戻りながら
#define MODORU_IR_FOLLOW_POWER 40 // この力でボールを追う

//// TEIITI処理 ////
#define TEIITI_LINE_ESCAPE_POWER 40 // ラインが反応したときの力 (ラインから離れる動きの力)
#define TEIITI_POWER 40             // 定位置に戻るときの力

//// ATTACK処理 ////
#define ATTACK_IR_FOLLOW_POWER 40 // アタッカーのときの一時的にボールに追う動作をするときの力