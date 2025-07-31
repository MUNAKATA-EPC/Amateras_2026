#ifndef USE_KICKER_H

#define USE_KICKER_H

//Kickerの初期設定を行う関数
void Kickerbegin(int fet1,int fet2);

//0より大きな数字が関数に入力されるとKickerを動かす、そうでないとチャージする
void CheckKick(int Kickdata);

#endif