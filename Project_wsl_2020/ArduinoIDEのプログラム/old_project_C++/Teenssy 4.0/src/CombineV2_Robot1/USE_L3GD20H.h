#ifndef USE_L3GD20H_H

#define USE_L3GD20H_H

// L3GD20HのデータをString型で格納する変数
extern String StrL3GD20H_Data;

// L3GD20Hとのシリアル通信を行う関数
void L3GD20Hbegin(int Serial_speed);

// L3GD20Hからの角度を返す関数
int L3GD20Hread();

#endif