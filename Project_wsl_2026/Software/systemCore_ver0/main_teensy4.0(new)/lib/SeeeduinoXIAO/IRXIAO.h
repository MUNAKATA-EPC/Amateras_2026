#ifndef MYLIB_IRXIAO_H

#define MYLIB_IRXIAO_H

//  IRボールの角度を格納する変数
extern int IRedge;
//  IRボールの距離を格納する変数
extern int IRdist;
//  IRボールが存在するかどうかを格納する変数
extern bool IRexist;

/*　SeeeduinoXiaoとの通信関連　*/

// IRXIAOとの通信を開始する関数
void IRXIAObegin();
// IRXIAOからのデータを入手する関数
void IRXIAOget();

#endif