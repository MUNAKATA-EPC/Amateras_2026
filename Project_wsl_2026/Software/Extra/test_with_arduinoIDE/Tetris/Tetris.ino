//-----------------------------------------------------------------------
// テトリス
//
//   ゲームパッド
//       左スティック  右スティック
//
//           移動          回転
//        左← ● →右     左← ● →右
//            ↓
//            下
//-----------------------------------------------------------------------
// Copyright (C) 2015  Takao Satoh  All Right Reserved
//-----------------------------------------------------------------------

//ライブラリを取り込む
#include <Sprite.h>
#include <Matrix.h>
#include <Button.h>
#include <RPlib.h>
#include <PS2X_lib.h>
#include <Tone.h>

Matrix myMatrix = Matrix(11, 13, 1); //LEDマトリクスを使う前のおまじない
//Button SW1 = Button(D0, HIGH); //D0に接続されたスイッチをSW1と命名する
//Button SW2 = Button(D1, HIGH); //D1に接続されたスイッチをSW2と命名する
bool Sw1, Sw2;
PS2X ps2x;
Tone tone1;

char *sound1 = "Clear:d=16,o=4,b=200:e6,g6,e7,c7,d7,g7";
char *sound2 = "G-over:d=16,o=4,b=100:4d,8d.,d,4d,8f.,e,8e.,d,8d.,c#,2d";

Sprite PatternA = Sprite(
  8, 8,
  B11000010,
  B01101010,
  B00001011,
  B11101000,
  B00101011,
  B10000011,
  B11001000,
  B01011100
);
Sprite PatternB1 = Sprite(
  8, 8,
  B00011000,
  B00111100,
  B01111110,
  B11011011,
  B11111111,
  B00100100,
  B01011010,
  B10100101
);
Sprite PatternB2 = Sprite(
  8, 8,
  B00011000,
  B00111100,
  B01111110,
  B11011011,
  B11111111,
  B00100100,
  B01100110,
  B01010010
);
char Part0[] = { 3, 2,  0, 1, 0,  1, 1, 1 };
char Part1[] = { 2, 3,  0, 1,  1, 1,  1, 0 };
char Part2[] = { 2, 3,  0, 1,  0, 1,  1, 1 };
char Part3[] = { 2, 2,  1, 1,  1, 1 };
char Part4[] = { 2, 3,  1, 0,  1, 0,  1, 1 };
char Part5[] = { 2, 3,  1, 0,  1, 1,  0, 1 };
char Part6[] = { 1, 4,  1,  1,  1,  1 };

char Part[4][4][4];  //現在の図形パターン
char Block[8][8];  //ブロックメモリ
char Title[31];
char X;  //図形左端x座標
char Y;  //図形下端y座標
char Width;  //図形幅
char Hight;  //図形高さ
char Angle;  //図形向き

int Time, Time2;  //タイムカウンタ
int Sound = 1;  //音の有無
int Start = -1;  //初期化フラグ
int Speed, Score, PartNo, Prev;
int Lx, Ly, Rx;  //アナログスティック値

void setup() {
  myMatrix.setBrightness(1);  //マトリクスの明るさ（０−８）
  ps2x.config_gamepad(13,11,10,12,true,true);  //おまじない、ピン設定
  tone1.begin(D2);  //D2にスピーカを繋げる
  //オープニング
  sprintf(Title, "TETRIS");
  myMatrix.clear();
  myMatrix.scrollText(Title, 80);
//  delay(1500);
}

void demo(int partno, int display) {
  int sound = Sound;  char angle;

  Sound = 0;
  if (partno > 6) {
    for (partno = 0; partno < 7; partno++)
      demo(partno, display);
  } else {
    switch (partno) {
      case 0:  X = 3;  Y = 7;  angle = 0;  break;
      case 1:  X = 0;  Y = 1;  angle = 3;  break;
      case 2:  X = 0;  Y = 4;  angle = 3;  break;
      case 3:  X = 6;  Y = 5;  angle = 0;  break;
      case 4:  X = 6;  Y = 2;  angle = 0;  break;
      case 5:  X = 0;  Y = 7;  angle = 0;  break;
      case 6:  X = 4;  Y = 4;  angle = 0;  break;
      default:  return;
    }
    PartNo = partno;  next();  Angle = angle;
    if (Angle % 2) {  //値交換
      Width += Hight;  Hight = Width - Hight;  Width -= Hight;
    }
    if (display) {
      Y -= 8;  move(0, 8);
    } else {
      move(0, -8);
    }
  }
  Sound = sound;
}

void loop() {
  if (Start < 0) {  //オープニング
    for (Time = 0; Time < 16; Time++) {
      ps2x.read_gamepad();  //無線通信復帰用ダミー読み込み
      delay(100);
    }
//    myMatrix.write(0, 0, PatternA);
    fix(0);  //ブロックメモリ初期化
    Start = 1;  Time = 0;
  }
//  SW1.listen();  //SW1の状態をマイコンに聞く
//  SW2.listen();  //SW2の状態をマイコンに聞く
//  Sw1 = SW1.isPressed();  //SW1押下状況
//  Sw2 = SW2.isPressed();  //SW2押下状況
  ps2x.read_gamepad();  //ゲームパッドから値を読み込む
  Lx = ps2x.Analog(PSS_LX);  //アナログ左スティック左右
  Ly = ps2x.Analog(PSS_LY);  //アナログ左スティック上下
  Rx = ps2x.Analog(PSS_RX);  //アナログ右スティック左右
  if (abs(Lx) < 16)  Lx = 0;  //不感帯
  if (abs(Ly) < 16)  Ly = 0;  //不感帯
  if (abs(Rx) < 16)  Rx = 0;  //不感帯

  if (Start) {
    if (Sw1 || Sw2 || Lx || Ly || Rx) {  //SW1またはSW2が押されたら
      myMatrix.clear();
      Y = -1;  Speed = 160;  PartNo = Start = Score = Time = 0;
    } else {
      if (Time == 800)  Time = 0;
      if (Time % 50 == 0)
        if (Time % 100)
          demo(Time/100, 0);
        else
          demo(Time/100, 1);
    }
  } else if (Time == 1) {
    if (Y < 0) {  //次の図形描画前
      next();     //次の図形準備
      X = (8 - Width) / 2;
//      Speed = (Score > 50) ?  10 :  160 - Score * 3;
      Prev = 0;
    }
  } else if (Time >= Speed) {
    if ( move(0, 1) ) {  //落下＆着地
      if (Y - Hight + 1 < 0) {  //上部はみ出し
        over(Score);  //ゲームオーバー
        Start = -1;
      } else {
        fix(1);  //図形固着
        Y = line();  //ライン消し
        if (Y && Speed > 10)
//          Speed = (Speed * 9 + 5) / 10;  //10%時短
          Speed = (Speed * 19 + 10) / 20;  // 5%時短
        Score += Y;  Y = -1;
      }
    }
    Time = 0;
  } else if (Y < 0) {
    //操作無効
  } else if (Sw1 || Lx < 0) {  //左移動
    if (Prev != -1 || 
        Prev == -1 && (Sw1 && Time2 >= 50 || Time2 - Lx / 2 >= 75)) {
      move(-1, 0);
      Time2 = 0;
    }
    Prev = -1;
  } else if (Sw2 || Lx > 0) {  //右移動
    if (Prev != 1 || 
        Prev == 1 && (Sw2 && Time2 >= 50 || Time2 + Lx / 2 >= 75)) {
      move(1, 0);
      Time2 = 0;
    }
    Prev = 1;
  } else if (Ly < 0) {  //下移動
    if (Prev != 3 || 
        Prev == 3 && (Time2 - Ly / 2 >= 75)) {
      if ( move(0, 1) ) {
        Time = Speed;
      } else {
        Time = 0;
      }
      Time2 = 0;
    }
    Prev = 3;
  } else if (Rx < 0) {  //左回転
    if (Prev != -2 || 
        Prev == -2 && (Time2 - Rx / 3 >= 75)) {
      rotate(-1);
      Time2 = 0;
    }
    Prev = -2;
  } else if (Rx > 0) {  //右回転
    if (Prev != 2 || 
        Prev == 2 && (Time2 + Rx / 3 >= 75)) {
      rotate(1);
      Time2 = 0;
    }
    Prev = 2;
  } else {
    Prev = Time2 = 0;
  }
  Time++;  Time2++;  delay(10);
}

void next() {
  char *part[] = {Part0, Part1, Part2, Part3, Part4, Part5, Part6};
  char x, y, dot;

  Angle = 0;
  Width = part[PartNo][0];
  Hight = part[PartNo][1];
  for (y = 0; y < Hight; y++) {
    for (x = 0; x < Width; x++) {
      dot = part[PartNo][2 + y * Width + x];
      Part[0][y][x] = dot;
      Part[1][x][Hight - 1 - y] = dot;
      Part[2][Hight - 1 - y][Width - 1 - x] = dot;
      Part[3][Width - 1 - x][y] = dot;
    }
  }
  if (++PartNo > 6)  PartNo = 0;
}

void fix(int mode) {
  char x, y;

  if (mode == 0) {  //ブロックメモリ初期化
    for (y = 0; y < 8; y++)
      for (x = 0; x < 8; x++)
        Block[y][x] = 0;
  } else {  //図形固着
    for (y = 0; y < Hight; y++)
      for (x = 0; x < Width; x++)
        Block[Y + y - Hight + 1][X + x] += Part[Angle][y][x];
  }
}

int move(char dx, char dy) {
  char x, y, nx, ny;

  nx = X + dx;              //移動後図形左端x座標
  ny = Y + dy - Hight + 1;  //移動後図形上端y座標
  if (nx < 0 || nx + Width > 8 || ny + Hight > 8) {  //移動限界
    if (Sound)  tone1.play(NOTE_D4, 10);  //無効音
    return -1;
  }
  for (y = 0; y < Hight; y++) {
    if (ny + y < 0)  continue;
    for (x = 0; x < Width; x++)
      if (Block[ny + y][nx + x] && Part[Angle][y][x]) {  //重なり
        if (Sound)  tone1.play(NOTE_D4, 10);  //無効音
        return 1;
      }
  }
  if (Sound)  tone1.play(NOTE_F4, 10);  //移動音

  for (y = 0; y < Hight; y++)
    for (x = 0; x < Width; x++)
      if (Part[Angle][y][x]) {
        myMatrix.write(X + x, Y + y - Hight + 1, LOW);  //移動前消去
      }
  for (y = 0; y < Hight; y++)
    for (x = 0; x < Width; x++)
      if (Part[Angle][y][x]) {
        myMatrix.write(nx + x, ny + y, HIGH);  //移動後描画
      }
  X = nx;  Y = ny + Hight - 1;  return 0;
}

int rotate(char da) {
  char x, y, nx, ny, nw, nh, na;
  char i, j, ng;

  na = Angle + da;
  if (na < 0)  na = 3;
  if (na > 3)  na = 0;
  if (da % 2) { nw = Hight;  nh = Width; }
  ny = Y - nh + 1;  //回転後図形上端y座標

  j = abs(nw - Width) + 1;  //横振り回数
  for (i = 1; i <= j; i++) {
    nx = X - (nw - Width) / 2;  //回転後図形左端x座標デフォルト
    if (i % 2) {
      nx += (nw > Width) ?  i / 2 :  -i / 2;
    } else {
      nx += (nw > Width) ?  -i / 2 :  i / 2;
    }
    if (nx < 0 || nx + nw > 8)
      continue;
    for (ng = y = 0; y < nh; y++) {
      if (ny + y < 0)  continue;
      for (x = 0; x < nw; x++)
        if (Block[ny + y][nx + x] && Part[na][y][x]) {  //重なり
          ng++;  break;
        }
      if (ng)  break;
    }
    if (!ng)  break;
  }
  if (ng) {  //回転不能
    if (Sound)  tone1.play(NOTE_D4, 10);  //無効音
    return 2;
  }
  if (Sound)  tone1.play(NOTE_A4, 10);  //回転音

  for (y = 0; y < Hight; y++)
    for (x = 0; x < Width; x++)
      if (Part[Angle][y][x]) {
        myMatrix.write(X + x, Y + y - Hight + 1, LOW);  //回転前消去
      }
  for (y = 0; y < nh; y++)
    for (x = 0; x < nw; x++)
      if (Part[na][y][x]) {
        myMatrix.write(nx + x, ny + y, HIGH);  //回転後描画
      }
  X = nx;  Width = nw;  Hight = nh;  Angle = na;  return 0;
}

int line() {
  char x, y, n, lines = 0;

  for (y = 7; y >= 0; y--) {
    for (n = x = 0; x < 8; x++) {
      Block[y][x]  &&  n++;
    }
    if (n == 8) {  //ライン消し
      for (x = 0; x < 8; x++)
        myMatrix.write(x, y, LOW);
      lines++;
    }
  }
  if (lines) {
    Sound ?  tone1.play_rtttl(sound1) :  delay(500);  //ライン消し音
    lines = 0;
    for (y = 7; y >= 0; y--) {
      for (n = x = 0; x < 8; x++) {
        Block[y][x]  &&  n++;
      }
      if (n == 8) {
        lines++;
      } else if (lines) {
        n = y + lines;
        for (x = 0; x < 8; x++) {
          if (Block[y][x]) {
            Block[n][x] = 1;
            myMatrix.write(x, n, HIGH);
          } else {
            Block[n][x] = 0;
            myMatrix.write(x, n, LOW);
          }
        }
      }
    }
    for (y += lines; y >= 0; y--) {
      for (x = 0; x < 8; x++) {
        Block[y][x] = 0;
        myMatrix.write(x, y, LOW);
      }
    }
    if (Sound)  tone1.play(NOTE_C2, 200);  //だるま落とし音
  }
  return lines;
}

void over(int score) {
  char x, y;

  if (Sound)  tone1.play(NOTE_D4, 200);  delay(200);
  if (Sound)  tone1.play(NOTE_D4, 400);  delay(800);

  myMatrix.write(0, 0, PatternB1);
  Sound ?  tone1.play_rtttl(sound2) :  delay(200);

  for (x = 0; x < 8; x++) {
    if (x % 2) {
      myMatrix.write(0-x, 0, PatternB1);
    } else {
      myMatrix.write(0-x, 0, PatternB2);
    }
    for (y = 0; y < 8; y++) {
      myMatrix.write(8-x, y, LOW);
    }
    delay(200);
  }
  sprintf(Title, "GAMEOVER %dp", score);
  myMatrix.clear();
  myMatrix.scrollText(Title, 80);
//  delay(1500);
}

