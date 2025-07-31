#ifndef USE_NEOPIXELL_H

#define USE_NEOPIXELL_H

// メーターがマックスになったら1とする変数
extern bool MeterMax;

// NeoPixelを始める関数
void NeoPixel_begin(int a, int b, int c);
// NeoPixelを表示させる関数
void NeoPixel_Show();
// NeoPixelをクリアにする変数
void NeoPixel_Clear();

// 色相環からr,g,bを算出する関数
void getRGBnum(int RGBedge, int &r, int &g, int &b);

// 指定した角度のLEDを指定した色に光らせる
void One_NeoLight(int LEDedge, int RGBedge, int PerBright);
// 色を指定した角度の所で光らせる関数(このピン番号から, このピン番号まで, 光らせるLEDの数, 色相環の角度min, 色相環の角度max, 明るさ)
void Mapping_NeoLight(int fromLEDpin, int toLEDpin, int LightLEDs, int minRGBedge, int maxRGBedge, int PerBright);
// 色相環をSpeedで回転させる関数
void SpeedTurn_NeoLight(double Speed, int PerBright);
// メーター表示させる関数
void Meter_NeoLight(double Speed, int PerBright);
void Meter_NeoLight_Clear();
// 指定した角度に指定した色を光らせる関数[PlusMinusLEDsとは、a度にあるLEDから右にPlusMinusLEDs個目左にPlusMinusLEDs個目までの範囲を光らせるということ]
void LEDedge_NeoLight(int LEDedge, int PlusMiusLEDs, int minRGBedge, int maxRGBedge, int PerBright);

#endif