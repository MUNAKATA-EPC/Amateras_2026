/*実際の動きの部分*/
#pragma once

#include <Arduino.h>
#include "Sensors.hpp"
#include "Functions.hpp"
#include "Drivers.hpp"

#include "Attacker/Attacker.hpp"
#include "Defender/Defender.hpp"
#include "Radicon/Radicon.hpp"

extern PD pdGyro; // ジャイロ用のPD
extern PD pdCam;  // カメラ用のPD