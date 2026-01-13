#ifndef MYLIB_LINEXIAO_H

#define MYLIB_LINEXIAO_H

// LINEの角度を格納する変数
extern int LINEedge;
// サイドLINEの状態を格納する変数
extern bool LINEright, LINEleft, LINEback;
// LINEが存在するかどうか格納する変数
extern bool LINEexist;

// LINEXIAOとの通信を開始する関数
void LINEXIAObegin();
// LINEXIAOからのデータを入手する関数
void LINEXIAOget();

#endif