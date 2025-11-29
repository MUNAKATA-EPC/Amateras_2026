#pragma once

#include "Arduino.h"
#include "anyrtttl.h"

#define BUZZER_PIN 9 // 今回の機体は9ピン

// rtttl用の曲データ -made from nakano-
extern const char zenzenzense[]; // 前前前世

extern const char shokei_part3[]; // 第三部処刑用BGM

// tone関数用の周波数
enum BUZZER_ONKAI
{
    // C0: ゼロオクターブ（非常に低い音）
    MyC0 = 16,
    MyCS0 = 17, // C#0/Db0
    MyD0 = 18,
    MyDS0 = 19, // D#0/Eb0
    MyE0 = 21,
    MyF0 = 22,
    MyFS0 = 23, // F#0/Gb0
    MyG0 = 25,
    MyGS0 = 26, // G#0/Ab0
    MyA0 = 28,
    MyAS0 = 29, // A#0/Bb0
    MyB0 = 31,

    // C1: 1オクターブ
    MyC1 = 33,
    MyCS1 = 35,
    MyD1 = 37,
    MyDS1 = 39,
    MyE1 = 41,
    MyF1 = 44,
    MyFS1 = 46,
    MyG1 = 49,
    MyGS1 = 52,
    MyA1 = 55,
    MyAS1 = 58,
    MyB1 = 62,

    // C2: 2オクターブ
    MyC2 = 65,
    MyCS2 = 69,
    MyD2 = 73,
    MyDS2 = 78,
    MyE2 = 82,
    MyF2 = 87,
    MyFS2 = 93,
    MyG2 = 98,
    MyGS2 = 104,
    MyA2 = 110,
    MyAS2 = 117,
    MyB2 = 123,

    // C3: 3オクターブ
    MyC3 = 131,
    MyCS3 = 139,
    MyD3 = 147,
    MyDS3 = 156,
    MyE3 = 165,
    MyF3 = 175,
    MyFS3 = 185,
    MyG3 = 196,
    MyGS3 = 208,
    MyA3 = 220, // 基準音 A=440Hzの1オクターブ下
    MyAS3 = 233,
    MyB3 = 247,

    // C4: 4オクターブ (中央ド)
    MyC4 = 262, // 中央ド
    MyCS4 = 277,
    MyD4 = 294,
    MyDS4 = 311,
    MyE4 = 330,
    MyF4 = 349,
    MyFS4 = 370,
    MyG4 = 392,
    MyGS4 = 415,
    MyA4 = 440, // 基準音 A=440Hz
    MyAS4 = 466,
    MyB4 = 494,

    // C5: 5オクターブ
    MyC5 = 523,
    MyCS5 = 554,
    MyD5 = 587,
    MyDS5 = 622,
    MyE5 = 659,
    MyF5 = 698,
    MyFS5 = 740,
    MyG5 = 784,
    MyGS5 = 831,
    MyA5 = 880,
    MyAS5 = 932,
    MyB5 = 988,

    // C6: 6オクターブ
    MyC6 = 1047,
    MyCS6 = 1109,
    MyD6 = 1175,
    MyDS6 = 1245,
    MyE6 = 1319,
    MyF6 = 1397,
    MyFS6 = 1480,
    MyG6 = 1568,
    MyGS6 = 1661,
    MyA6 = 1760,
    MyAS6 = 1865,
    MyB6 = 1976,

    // C7: 7オクターブ (非常に高い音)
    MyC7 = 2093,
    MyCS7 = 2217,
    MyD7 = 2349,
    MyDS7 = 2489,
    MyE7 = 2637,
    MyF7 = 2794,
    MyFS7 = 2960,
    MyG7 = 3136,
    MyGS7 = 3322,
    MyA7 = 3520,
    MyAS7 = 3730,
    MyB7 = 3951,

    MyC8 = 4186,
    MyCS8 = 4435,
    MyD8 = 4699,
    MyDS8 = 4978
};