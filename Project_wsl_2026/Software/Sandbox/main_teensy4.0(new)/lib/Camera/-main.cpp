#include <Arduino.h>
#include "MySettings.h"
#include "OpenMV.h"

// カートの角度
int Soccer_field_max_deg = -1;

// 黄色ゴール
int Yellow_goal_deg = -1;
bool Yellow_goal_exist = false;
int Yellow_goal_distance = -1;

// 青ゴール
int Blue_goal_deg = -1;
bool Blue_goal_exist = false;
int Blue_goal_distance = -1;

void OpenMVbegin()
{
    OPENMV_SERIAL.begin(OPENMV_SERIALSPEED);
    OPENMV_SERIAL.setTimeout(50); // タイムアウト10ms
}

void OpenMVget()
{
    // 1行分をまとめて受信
    if (OPENMV_SERIAL.available())
    {
        String line = OPENMV_SERIAL.readStringUntil('\n'); // 改行まで読み込み

        // 改行までのデータを解析
        int buf[5] = {-1, -1, -1, -1, -1};

        int aPos = line.indexOf('a');
        int bPos = line.indexOf('b');
        int cPos = line.indexOf('c');
        int dPos = line.indexOf('d');
        int ePos = line.indexOf('e');

        if (aPos > 0 && bPos > aPos && cPos > bPos && dPos > cPos && ePos > dPos)
        {
            buf[0] = line.substring(0, aPos).toInt();
            buf[1] = line.substring(aPos + 1, bPos).toInt();
            buf[2] = line.substring(bPos + 1, cPos).toInt();
            buf[3] = line.substring(cPos + 1, dPos).toInt();
            buf[4] = line.substring(dPos + 1, ePos).toInt();
        }

        Soccer_field_max_deg = buf[0];

        Yellow_goal_deg = buf[1];
        Yellow_goal_distance = buf[2];
        if (Yellow_goal_deg != -1)
            Yellow_goal_exist = true;
        else
            Yellow_goal_exist = false;

        Blue_goal_deg = buf[3];
        Blue_goal_distance = buf[4];
        if (Blue_goal_deg != -1)
            Blue_goal_exist = true;
        else
            Blue_goal_exist = false;
    }
}
