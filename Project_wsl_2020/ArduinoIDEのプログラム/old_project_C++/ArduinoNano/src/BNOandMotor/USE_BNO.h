#ifndef BNOREAD_H

#define BNOREAD_H

//なにかの定数
#define BNO055_SAMPLERATE_DELAY_MS 10

//BNOのリセットピンを定義する関数
void BNObegin(int pin_num);

//BNOからの角度を返す関数
int BNOread();

#endif