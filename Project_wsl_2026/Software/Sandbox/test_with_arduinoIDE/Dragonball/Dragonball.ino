//-----------------------------------------------------------------------
// RTTTLのデータを再生
//-----------------------------------------------------------------------
// Copyright (C) Future Robotics Technology Center All Right Reserved
//-----------------------------------------------------------------------

//ライブラリを読み込む
#include <Sprite.h>
#include <Matrix.h>
#include <Tone.h>
#include <RPlib.h>

Matrix myMatrix = Matrix(11, 13, 1); //マトリクスLEDを使うときのオマジナイ
Tone tone1;                          //スピーカーを使うときのオマジナイ

//曲データ
char *song = "Dragonballz:d=16,o=5,b=140:4f6,48f.6,4g#6,4c7,48f.7,4g#6,4c7,48f.7,2p,2f6,2f,2f6,48f.,48f.,48f.,48f.,48f.,48f.,32p,48d#.6,48d#.,48d#.6,48d#.,48d#.,48d#.,48d#.,48d#.,48d#.,32€,48f.6,48f.,48f.6,48f.,48f.,48f.,48f.,48f.,48f.,6p,8f,8f7,8f6,8f,8f7,48f.6,48f.6,48f.6,48f.6,48f.6,48f.6,48f.6,p,a#,a#,p,c6,c,c6";

void setup(){
    tone1.begin(D2);        //D2にスピーカーを接続
}

void loop(){
    tone1.play_rtttl(song); //songという変数に入った曲を流す
    while(1);               //プログラムをストップ
}
