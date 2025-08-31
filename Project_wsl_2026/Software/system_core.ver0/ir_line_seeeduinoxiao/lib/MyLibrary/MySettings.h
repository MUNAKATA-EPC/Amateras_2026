#ifndef MYSET_H

#define MYSET_H

/*　シリアルについて　*/
/*IRsendプログラム*/
#define IRSEND_SERIALRXPIN 6    // シリアルの受信ピン番号を指定
#define IRSEND_SERIALTXPIN 7    // シリアルの送信ピン番号を指定
#define IRSEND_SERIALSPEED 9600 // シリアルの通信速度を指定
/*LINEsendプログラム*/
#define LINESEND_SERIALRXPIN 6    // シリアルの受信ピン番号を指定
#define LINESEND_SERIALTXPIN 7    // シリアルの送信ピン番号を指定
#define LINESEND_SERIALSPEED 9600 // シリアルの通信速度を指定
/*L3GD20Hsendプログラム*/
#define L3GDSEND_SERIALRXPIN 6    // シリアルの受信ピン番号を指定
#define L3GDSEND_SERIALTXPIN 7    // シリアルの送信ピン番号を指定
#define L3GDSEND_SERIALSPEED 9600 // シリアルの通信速度を指定

/*　Multiplexer16chについて　*/
#define MULTI16_S0PIN D1  // S0(出力)ピンの番号を指定
#define MULTI16_S1PIN D2  // S1(出力)ピンの番号を指定
#define MULTI16_S2PIN D3  // S2(出力)ピンの番号を指定
#define MULTI16_S3PIN D4  // S3(出力)ピンの番号を指定
#define MULTI16_COMPIN A5 // COM(読み取り)ピンの番号を指定
#define MULTI16_WAITMS 10 // マルチプレクサが信号を読み取るのを待つ時間(マイクロミリ秒)を指定

#endif