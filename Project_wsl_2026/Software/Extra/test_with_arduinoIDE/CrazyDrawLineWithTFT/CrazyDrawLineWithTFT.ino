#include <TFT.h>
#include <SPI.h>

//================================================================
// 1. 構造体とグローバル変数の定義
//================================================================

// RGB値を格納するための構造体（すべての関数の前に定義）
struct RGBValue {
  int r; // Red (0-255)
  int g; // Green (0-255)
  int b; // Blue (0-255)
};

// TFTscreenの定義（すべての関数の前に定義）
TFT TFTscreen = TFT(A2, 1, 9); // 液晶ディスプレイを使うときのピン設定

//================================================================
// 2. 関数の前方宣言 (プロトタイプ)
//================================================================

// hsvToRgb関数とdrawColorfullCircle関数のプロトタイプを宣言
RGBValue hsvToRgb(int h, double s, double v);
void drawColorfullCircle(double x, double y, double minR, double maxR, int startDeg);

//================================================================
// 3. Arduino 標準関数 (setup/loop)
//================================================================

void setup(){
  TFTscreen.begin();

  TFTscreen.background(0, 0, 0);  // 背景色は白

  // カラフル背景
  for(int i = 0;i < 36;i ++){
    RGBValue color = hsvToRgb(i * 10, 1.0, 1.0); 
    TFTscreen.background(color.r, color.g, color.b);  // 背景色をカラフルにする
  }

  TFTscreen.background(255, 255, 255);  // 背景色は白

  // カラフル円環
  drawColorfullCircle(80, 64, 44, 64, 0);
  drawColorfullCircle(80, 64, 23, 43, 120);
  drawColorfullCircle(80, 64, 2, 22, 240);

  TFTscreen.background(255, 255, 255);  // 背景色は白
}

// サイン波による異常描画
// TFTscreenの幅と高さを確認（TFTscreen.width()=160, TFTscreen.height()=128と仮定）
const int width = 160;
const int height = 128;
const int loopNum = 120;  // 小さくすると速くなる
const double lineVal = 20.0; // 大きくするとより線が増えていく

double hz = 0.0;
int count = 0;

void loop(){
  // hz の計算を修正: 周期が 1.0 から 5.0 の間で変化するように調整
  // (1000回ループで一周)
  hz = lineVal * sin(count * 2.0 * PI / (double)loopNum ) + lineVal + 1.0; 

  for (int x_coord = 0; x_coord < width; x_coord++) {
    // 角度の計算: 2*PI / 幅 * 周期 * x座標
    double sin_val = sin(2.0 * PI / width * hz * x_coord); 
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    // H の範囲にマッピング: sin_val (-1.0〜1.0) を色相 (0〜240) に変換
    // 振幅 = 120.0, 中心 = 120.0 -> Hは 0.0 〜 240.0 の範囲で変化
    double hue = 120.0 * sin_val + 120.0; 
    // Hを0〜360の範囲に正規化（fmod関数を使用）
    hue = fmod(hue + 360.0, 360.0);
    // RGBへの変換
    RGBValue color = hsvToRgb((int)hue, 1.0, 1.0); // S=1.0, V=1.0 (鮮やかな色)
    // 描画
    TFTscreen.stroke(color.r, color.g, color.b);  
    // 画面の x_coord 列全体を、計算した色で塗りつぶす（縦線を描く）
    TFTscreen.line(x_coord, 0, x_coord, height); 
  }

  count += 1;
  // countをリセットすることで、オーバーフローを防ぎ、ループを繰り返す
  if (count >= loopNum) count = 0;
}

//================================================================
// 4. 関数の本体定義
//================================================================

// HSV (H:0-359, S:0.0-1.0, V:0.0-1.0) から RGB (0-255) へ変換
RGBValue hsvToRgb(int h, double s, double v) {
  RGBValue rgb;
  double r_prime, g_prime, b_prime; 

  // 1. 特殊ケースの処理: S=0 (無彩色)
  if (s <= 0.0) {
    int intensity = (int)(v * 255.0 + 0.5); 
    rgb.r = intensity;
    rgb.g = intensity;
    rgb.b = intensity;
    return rgb;
  }

  h = h % 360;  
  if (h < 0) h += 360;

  // 2. 予備計算
  double c = v * s;        
  double h_prime = (double)h / 60.0; 
  double x = c * (1.0 - fabs(fmod(h_prime, 2.0) - 1.0)); 
  double m = v - c;        

  // 3. R'G'B' の計算
  int sector = (int)h_prime; 

  switch (sector) {
  case 0: 
    r_prime = c; 
    g_prime = x; 
    b_prime = 0.0; 
    break;
  case 1: 
    r_prime = x; 
    g_prime = c; 
    b_prime = 0.0; 
    break;
  case 2: 
    r_prime = 0.0; 
    g_prime = c; 
    b_prime = x; 
    break;
  case 3: 
    r_prime = 0.0; 
    g_prime = x; 
    b_prime = c; 
    break;
  case 4: 
    r_prime = x; 
    g_prime = 0.0; 
    b_prime = c; 
    break;
  case 5: 
    r_prime = c; 
    g_prime = 0.0; 
    b_prime = x; 
    break;
  default: 
    r_prime = 0.0; 
    g_prime = 0.0; 
    b_prime = 0.0; 
    break;
  }

  // 4. 最終的な R, G, B (0-255への変換と四捨五入)
  rgb.r = (int)((r_prime + m) * 255.0 + 0.5);
  rgb.g = (int)((g_prime + m) * 255.0 + 0.5);
  rgb.b = (int)((b_prime + m) * 255.0 + 0.5);

  // 0-255の範囲にクランプ
  rgb.r = constrain(rgb.r, 0, 255);
  rgb.g = constrain(rgb.g, 0, 255);
  rgb.b = constrain(rgb.b, 0, 255);

  return rgb;
}

// 描画関数
void drawColorfullCircle(double x, double y, double minR, double maxR, int startDeg) {
  // 描画する線分の総数。これを角度の精度として使用します。
  const int numSegments = 1024;

  for (int i = 0; i < numSegments; i++) {

    // 1. 角度（度数法）を浮動小数点数で正確に計算
    // 1024ステップで360度を表現します
    double angleDeg = (double)i * 360.0 / numSegments;

    // 2. 色相 H を角度から取得
    // Hは0-360の整数値に戻し、開始角度を適用
    int h = (int)round(angleDeg);
    RGBValue color = hsvToRgb((h + startDeg) % 360, 1.0, 1.0); 

    // 線の色を動的に変更
    TFTscreen.stroke(color.r, color.g, color.b); 

    // 3. 描画用の角度（ラジアン）を計算
    // radians()関数は引数にdoubleを取ります
    double rad = radians(angleDeg);

    // 始点と終点の計算 (角度は rad を使用)
    double start_x = cos(rad) * minR + x;
    double start_y = sin(rad) * minR + y;
    double end_x = cos(rad) * maxR + x;
    double end_y = sin(rad) * maxR + y;

    // 描画: 中心から外側へ向かう放射状の線
    TFTscreen.line(start_x, start_y, end_x, end_y);
  }
}








