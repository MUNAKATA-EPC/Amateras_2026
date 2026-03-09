#include <TFT.h>
#include <SPI.h>
#include <RPlib.h>

TFT display = TFT(A2, 1, 9);        //液晶ディスプレイを使うときのオマジナイ

// 背景色
const int background_color_num[3] = {
  0,0,0};

// 可変抵抗のピン
const int setting_adjuster_pin = A1;
const int char_y_adjuster_pin = A1;
// タクトスイッチ・LEDピン
const int tact_pin = A4;
const int led_pin = A5;
//　タクトスイッチ用変数
int is_tact_on = 0,is_old_tact_on = 0;

// 操作方法
#define ADJUSTER 0
#define TACT 1
int control_type = ADJUSTER;

// キャラクターのスキン(可変)・当たり判定
char char_skin[2] = "D"; //97a~122z
const int char_area[2] = {
  5,8};

// キャラクターの壁避け状況
#define AVOIDING 0
#define SUCCESS_AVOID 1
#define SAFE 2
#define HIT 3
int char_avoid_condition = SAFE; //キャラクターが壁から脱出した瞬間だけSUCCESS_AVOIDとなる
// キャラクターの壁回避の成功数をカウント
int char_avoid_success_count = 0;

// 長方形同士の幅・高さ
const int rect_width = 10;
int rect_x = 160;    // 初期X座標を160に設定
int rect_height = 0; // random()でゲームエリアの制約に合わせて初期化・更新される
// 長方形のスペース・速度格納用
int rect_space = 50;
int min_rect_space = rect_space - 10,max_rect_space = rect_space + 10; // スペースの最大値と最小値（初期値はrect_sapceの±10）
int rect_speed = 2;

// スコアボードの幅(スコアボードは画面下部に設置する)
const int score_borad_height = 20;
// スコア
double score = 0.0;
unsigned int start_ms = 0; //開始時間 

// 時間経過によるスコア加算の基準時間（秒）を記録
double old_score = 0.0; 

// 画面の高さ
const int screen_height = 128;
// "ゲーム"エリアの下端のY座標 (0始まり: 128 - 20 - 1 = 107)
const int game_area_bottom_y = screen_height - score_borad_height - 1;

// 始点と終点による長方形描画用
void rectPrint(int x1, int y1, int x2, int y2){
  // 1. 座標を画面範囲内に制限（クリッピング）
  int clipped_x1 = max(0, x1);
  int clipped_y1 = max(0, y1);
  int clipped_x2 = min(159, x2); // 横が159
  int clipped_y2 = min(127, y2); // 縦が127

  // 2. 制限後の座標から幅と高さを計算
  // x1 > x2 や y1 > y2 の場合、この計算で幅/高さが0以下になる
  int w = clipped_x2 - clipped_x1 + 1;
  int h = clipped_y2 - clipped_y1 + 1;

  // 3. 有効な幅と高さがある場合のみ描画
  if(w > 0 && h > 0){
    // display.rect(X座標, Y座標, 幅, 高さ) で描画
    display.rect(clipped_x1, clipped_y1, w, h);
  }
  // 描画範囲が完全に画面外にある場合 (w <= 0 または h <= 0)、何も描画しない
}

// int型プリント用
void intPrint(int data,int x,int y){
  String data_str = String(data);

  char data_char[10];                 //char配列準備
  data_str.toCharArray(data_char,10); //変換

  display.text(data_char, x, y); //(x,y)にdataをプリント
}

// double型プリント用
void doublePrint(double data, int x, int y){
  String data_str = String(data, 2); // 小数点以下2桁を含め、dataを文字列に変換

  char data_char[20];                  // char配列準備 (小数点以下5桁だと長くなるため、サイズを多めにとる)
  data_str.toCharArray(data_char, 20); // 変換

  display.text(data_char, x, y); // (x,y)にdataをプリント
}

void setup(){
  display.begin();
  display.background(background_color_num[0], background_color_num[1], background_color_num[2]); // 背景色でリセット

  pinMode(tact_pin, INPUT_PULLUP);    //タクトスイッチのピン設定
  pinMode(led_pin, OUTPUT);           //LEDのピン設定

  // 初期 rect_height の設定 (loop()のロジックに合わせた初期値)
  const int max_rect_height = screen_height - score_borad_height - rect_space;
  rect_height = random(8, max_rect_height); 

  display.stroke(255,255,255);    //白で描画
  display.setTextSize(1);         //文字サイズ1
  display.text("speed:",10,8*3+8);
  display.text("skin :",10,8*3+8+8);
  display.text("type :",10,8*3+8+8+8);

  int select_mode = 0;
  int setting_adjuster_val = 0,old_setting_adjuster_val = analogRead(setting_adjuster_pin); // 読み取っておく
  int setting_adjuster_moved = 0;
  while(1){
    display.stroke(random(40,160),random(195,255),0);      //緑で描画
    display.setTextSize(2);         //文字サイズ2
    display.text("- Avoid",0,0);
    display.text("  The Walls -",6,2*8);

    //　タクトスイッチ読み取り
    is_tact_on = digitalRead(tact_pin) == LOW;
    // タクトスイッチによる計算
    if(is_tact_on && !is_old_tact_on){
      setting_adjuster_moved = 0;
      select_mode ++;
    }

    // 4回ボタンが押されたらスタート(break;でループ脱出)
    if(select_mode > 2)break;

    // 可変抵抗の読み取り(setting_adjuster)
    setting_adjuster_val = analogRead(setting_adjuster_pin);
    // 可変抵抗が動かされたかどうか(タクトスイッチが押されるとリセット)
    if(abs(old_setting_adjuster_val - setting_adjuster_val) > 5)setting_adjuster_moved = 1;

    //　バー・長方形のスピード・スキンの計算
    int bar_x = 5,bar_y1,bar_y2;
    switch(select_mode){
    case 0: // スピード
      bar_y1=8*3+8;
      bar_y2=8*3+8+8;
      if(setting_adjuster_moved == 1)rect_speed = constrain((int)round(map(setting_adjuster_val,0,1023,1,10)),1,10);
      break;
    case 1: // スキン
      bar_y1=8*3+8+8;
      bar_y2=8*3+8+8+8;
      if(setting_adjuster_moved == 1)char_skin[0] = constrain((int)round(map(setting_adjuster_val,0,1023,'A','Z')),'A','Z');
      break;
    case 2: // 操作タイプ
      bar_y1=8*3+8+8+8;
      bar_y2=8*3+8+8+8+8;
      if(setting_adjuster_moved == 1)control_type = (setting_adjuster_val < 512) ? ADJUSTER : TACT;
      break;
    }

    // 描画
    display.stroke(255,255,255);             // 白で描画
    display.setTextSize(1);                  //文字サイズ1
    display.line(bar_x,bar_y1,bar_x,bar_y2); // バーをプリント
    intPrint(rect_speed,10+8*6,8*3+8);         // 長方形のスピードをプリント
    display.text(char_skin,10+8*6,8*3+8+8);  // スキンをプリント
    if(control_type == ADJUSTER){
      display.text("Pot",10+8*6,8*3+8+8+8);  // 可変抵抗モードをプリント
    }
    else{
      display.text("Tact",10+8*6,8*3+8+8+8);  // タクトスイッチモードをプリント
    }

    delay(10);

    // 消す描画
    display.stroke(background_color_num[0], background_color_num[1], background_color_num[2]); //背景色で描画
    display.setTextSize(1);                  //文字サイズ1
    display.line(bar_x,bar_y1,bar_x,bar_y2); // バーを消す
    intPrint(rect_speed,10+8*6,8*3+8);       // 長方形のスピードを消す
    display.text(char_skin,10+8*6,8*3+8+8);  // スキンをプリントを消す
    if(control_type == ADJUSTER){
      display.text("Pot",10+8*6,8*3+8+8+8);  // 可変抵抗モードを消す
    }
    else{
      display.text("Tact",10+8*6,8*3+8+8+8);  // タクトスイッチモードを消す
    }

    //可変抵抗記録
    old_setting_adjuster_val = setting_adjuster_val;

    //タクトスイッチ記録
    is_old_tact_on = is_tact_on;
  }

  display.stroke(255,255,255);             // 白のラインで描画
  display.setTextSize(1);                  //　文字サイズ1
  intPrint(rect_speed,10+8*6,8*3+8);       // 長方形のスピードを再プリント
  display.text(char_skin,10+8*6,8*3+8+8);  // スキンを再プリント
  if(control_type == ADJUSTER){
    display.text("Pot",10+8*6,8*3+8+8+8);  // 可変抵抗モードをを再プリント
  }
  else{
    display.text("Tact",10+8*6,8*3+8+8+8);  // タクトスイッチモードを再プリント
  }
  display.text("->Game start!",10,8*3+8+8+8+8+8);
  delay(1000);

  display.background(background_color_num[0], background_color_num[1], background_color_num[2]); // 背景色で再リセット

  //　スコアボードの動かない部分を今のうちにプリントしておく
  display.stroke(255, 255,255); // 白色で描画
  display.fill(255, 255,255);   // 白色の塗りつぶし
  rectPrint(0,108,160,128);
  display.stroke(0, 0,0);       // 黒色で描画
  display.text("score:",1,109);
  display.text("count:",1,109+8);
  //　開始時間を記録
  start_ms = millis(); 
}

// タクトスイッチによる重力ジャンプに際に必要な変数
double double_char_y = 0.0;
double double_char_v = 0.0;

int first_call = 1; // 長方形たちを初期地に戻すための変数

void loop(){
  //　長方形の描画
  int up_rect_x1 = rect_x;
  int up_rect_y1 = 0;                                                   // 上の長方形の始点 Y=0
  int up_rect_x2 = rect_x + rect_width - 1;
  int up_rect_y2 = rect_height - 1;                                     // 上の長方形の終点 Y=rect_height - 1 (高さは rect_height)

  int down_rect_x1 = rect_x;
  int down_rect_y1 = rect_height + rect_space;                          // 下の長方形の始点 Y=開口部の直下
  int down_rect_x2 = rect_x + rect_width - 1;
  int down_rect_y2 = game_area_bottom_y;                                // 下の長方形の終点

  display.stroke(random(50,150),random(200,255),random(10));      //緑で描画
  display.fill(100,255,0);        //緑の塗りつぶしを設定
  rectPrint(up_rect_x1,up_rect_y1,up_rect_x2,up_rect_y2);         //上の長方形の描画
  rectPrint(down_rect_x1,down_rect_y1,down_rect_x2,down_rect_y2); //下の長方形の描画

  //　キャラクターの座標計算・描画
  int char_x = 10;
  const int char_max_y = screen_height - char_area[1] - score_borad_height;  // キャラクターの最大Y座標 = ゲームエリアの底 Y座標 - キャラクターの高さ + 1 (文字の描画は左上隅の座標)

  int char_y; // キャラクターのy座標格納用
  if(control_type == ADJUSTER){ // 可変抵抗を使う場合
    char_y = map(analogRead(char_y_adjuster_pin),0,1023,0,char_max_y);     // Y座標を0から100の範囲にマップ (Y=100がキャラクターの底辺108になる)
  }
  else{ // タクトスイッチを使う場合
    is_tact_on = digitalRead(tact_pin) == LOW; //　タクトスイッチ読み取り

    const double fall_a = 0.5;     // 落ちていくときの加速度
    const double tact_jump_v0 = 2.0; // タクトスイッチが押されたときに加えられる初速度

    if(is_tact_on && !is_old_tact_on){
      double_char_v += tact_jump_v0; // 速度に初速度を加える
    }
    double_char_v -= fall_a; // 速度に加速度を加える

    double_char_y += double_char_v; // 速度から距離を求める
    if(double_char_y < 0){
      double_char_y = 0.0;
      double_char_v = 0.0; // 上限なら速度も0.0にする
    }
    else if(double_char_y > (double)char_max_y){
      double_char_y = (double)char_max_y;
      double_char_v = 0.0; // 上限なら速度も0.0にする
    }

    char_y = (int)round((double)char_max_y - double_char_y); // 下を地面にするために反転させる！

    is_old_tact_on = is_tact_on;               //タクトスイッチ記録
  }

  display.stroke(255,255,255); // 白で描画
  display.setTextSize(1);      //文字サイズ1
  display.text(char_skin,char_x,char_y);

  // 壁との衝突判定
  // 1. X軸の衝突判定 (長方形がキャラクターのX座標範囲にあるか)
  if(!(char_x + char_area[0] < rect_x || rect_x + rect_width <= char_x)){ 
    // X軸で衝突の可能性あり

    // 2. Y軸の衝突判定
    // 開口部の開始Y座標: rect_height
    // 開口部の終了Y座標: rect_height + rect_space - 1 (rect_height + rect_space の1つ手前)

    // キャラクターのY範囲: char_y から char_y + char_area[1] - 1

    // キャラクターが Y >= rect_height かつ キャラクターの底辺Y <= rect_height + rect_space の場合に回避成功
    if(char_y >= rect_height && char_y + char_area[1] <= rect_height + rect_space){
      char_avoid_condition = AVOIDING; // 避け中
    }
    else{
      char_avoid_condition = HIT; // 衝突
    }
  }
  else{
    if(char_avoid_condition == AVOIDING){
      char_avoid_condition = SUCCESS_AVOID; // 壁から脱出した瞬間

      char_avoid_success_count ++; //カウントアップ！
    }
    else{
      char_avoid_condition = SAFE; // 壁がなく安全
    }
  }

  //　当たってしまったらGameIsOverアニメーション
  if(char_avoid_condition == HIT){
    display.background(background_color_num[0], background_color_num[1], background_color_num[2]);//背景色にする（リセット）

    display.setTextSize(2);         //文字サイズ2  
    display.stroke(0,0,255); //赤で描画
    display.text("Game",0,0);
    delay(1000);
    display.stroke(255,0,0); //青で描画
    display.text("Over",64,0);
    delay(1000);
    display.stroke(255,255,255); //白で描画
    display.line(0,2*8,160,2*8);
    display.line(0,2*8+4,160,2*8+4);
    delay(1000);

    display.stroke(255,255,255); // 白で描画
    display.text("score:",0,24);
    display.text("count:",0,24+8*2);
    doublePrint(score,75,24);
    intPrint(char_avoid_success_count,75,24+8*2);
    delay(1000);
    
    display.stroke(255,255,255); //白で描画
    display.line(0,40+2*8+2,160,40+2*8+2);
    display.line(0,40+2*8+2+4,160,40+2*8+2+4);
    delay(1000);

    while(1); // 終了
  }

  // スコアの計算 (避けた壁の数と難易度に基づきスコア計算)
  // 1. 衝突を回避した時点でスコアを加算
  if(char_avoid_condition == SUCCESS_AVOID){
    // スコア = (固定点 + 速度ボーナス) * (避けた壁の数)
    // 速度が速いほど、成功時の基礎点が高い
    double base_points = 3.0 + (double)rect_speed * double(round(100) / 200);
    score += base_points;
  }
  // 2. 時間経過によるスコアの加算（ゲーム継続ボーナス）
  // 1秒ごとに一定値を加算する
  score += (double)(millis() - start_ms - old_score * 1000.0) / 1000.0 * 2.0;
  old_score = (double)(millis() - start_ms) / 1000.0; // 時間経過による加算の基準点を更新

  // スコアボードの描画(画面下部の20ピクセル分をつかう)
  display.stroke(0,0,0);       // 黒で描画
  display.setTextSize(1);      //文字サイズ1
  doublePrint(score,5*8,109);
  intPrint(char_avoid_success_count,5*8,109+8);

  delay(10);

  // 消す描画にうつる
  display.stroke(background_color_num[0], background_color_num[1], background_color_num[2]); // 背景色で描画
  display.fill(background_color_num[0], background_color_num[1], background_color_num[2]);   // 背景色の塗りつぶしを設定
  // キャラクターを消す
  display.setTextSize(1);                //文字サイズ1
  display.text(char_skin,char_x,char_y); 
  // 長方形を消す
  rectPrint(up_rect_x1,up_rect_y1,up_rect_x2,up_rect_y2);
  rectPrint(down_rect_x1,down_rect_y1,down_rect_x2,down_rect_y2); 
  //スコアボードを消す
  display.stroke(255,255,255);       // 白で描画（スコアボードが白だから）
  display.setTextSize(1); //文字サイズ1
  doublePrint(score,5*8,109);
  intPrint(char_avoid_success_count,5*8,109+8);

  // 長方形移動・スペース位置の計算
  rect_x -= rect_speed;
  if(rect_x < -rect_width || first_call == 1){
    rect_x = 160;
    first_call = 0;

    // 避けた壁の総数に応じて難易度を上げていく
    int count = char_avoid_success_count;
    int old_speed = rect_speed; // 速度が変わったかチェックするために保持

    // 難易度インジケータ（カラフルバー）の描画設定
    int bar_color_r, bar_color_g, bar_color_b;
    // メーターのブロック設定
    const int bar_width = 8; // 各レベルのブロックの幅
    const int bar_height = 16; // 長方形の高さ（文字サイズ1の高さと同じくらい）
    const int meter_y_start = 110; // メーターのY座標 (スコアの行に合わせる)
    const int meter_y_end = meter_y_start + bar_height - 1; // メーターの下端

    // メーターの開始X座標 (スコア表示 'score:00.00' の右側から始める)
    // 'score:' の幅 (5文字 * 8px) + 値の幅 (4文字 * 8px) = 72px 程度
    // 安全を見て 80px から開始します。
    const int meter_start_x = 80;

    // 描画する長方形の数（現在の難易度レベルを示す）
    int num_blocks_to_draw = 0;

    // 難易度レベルごとの速度、最小スペース、および描画ブロック数の調整
    if(count >= 70){ 
      rect_speed = 18; 
      min_rect_space = 12; 
      bar_color_r = 255; 
      bar_color_g = 0; 
      bar_color_b = 0; // 赤 (最高難度)
      num_blocks_to_draw = 9; // レベル10
    }
    else if(count >= 64){ 
      rect_speed = 14; 
      min_rect_space = 18; 
      bar_color_r = 255; 
      bar_color_g = 0; 
      bar_color_b = 0; // 赤に近い色
      num_blocks_to_draw = 8; // レベル9
    }
    else if(count >= 55){ 
      rect_speed = 11; 
      min_rect_space = 23; 
      bar_color_r = 255; 
      bar_color_g = 165; 
      bar_color_b = 0; // オレンジ
      num_blocks_to_draw = 7; // レベル8
    }
    else if(count >= 40){ 
      rect_speed = 9; 
      min_rect_space = 26; 
      bar_color_r = 255; 
      bar_color_g = 255; 
      bar_color_b = 0; // 黄
      num_blocks_to_draw = 6; // レベル7
    }
    else if(count >= 20){ 
      rect_speed = 7; 
      min_rect_space = 30; 
      bar_color_r = 0; 
      bar_color_g = 255; 
      bar_color_b = 0; // 緑
      num_blocks_to_draw = 5; // レベル6
    }
    else if(count >= 12){ 
      rect_speed = 6; 
      min_rect_space = 34; 
      bar_color_r = 0; 
      bar_color_g = 255; 
      bar_color_b = 255; // シアン
      num_blocks_to_draw = 4; // レベル5
    }
    else if(count >= 7){ 
      rect_speed = 5; 
      min_rect_space = 37; 
      bar_color_r = 0; 
      bar_color_g = 128; 
      bar_color_b = 255; // 明るい青
      num_blocks_to_draw = 3; // レベル4
    }
    else if(count >= 3){ 
      rect_speed = 4; 
      min_rect_space = 40; 
      bar_color_r = 0; 
      bar_color_g = 0; 
      bar_color_b = 255; // 青
      num_blocks_to_draw = 2; // レベル3
    }
    else if(count >= 1){ 
      rect_speed = 3; 
      min_rect_space = 40; 
      bar_color_r = 128; 
      bar_color_g = 128; 
      bar_color_b = 128; // 灰 (初期値)
      num_blocks_to_draw = 1; // レベル2
    }
    else { 
      rect_speed = 2; 
      min_rect_space = 40; 
      bar_color_r = 128; 
      bar_color_g = 128; 
      bar_color_b = 128; // 灰 (初期値)
      num_blocks_to_draw = 0; // レベル1
    }

    // 難易度レベルに応じて、過去のレベルのブロックも含む全てを描画

    // 10この色
    const int colors[10][3] = {
      { 
        150, 0, 0                                     }
      ,    // 0. 濃い青 (B:150, G:0, R:0)
      { 
        200, 50, 0                                     }
      ,   // 1. 青 (B:200, G:50, R:0)
      { 
        255, 100, 0                                     }
      ,  // 2. 明るい青 (B:255, G:100, R:0)
      { 
        255, 200, 0                                     }
      ,  // 3. シアンに近い青 (B:255, G:200, R:0)
      { 
        200, 255, 0                                     }
      ,  // 4. シアンと緑の中間 (B:200, G:255, R:0)
      { 
        0, 255, 0                                     }
      ,    // 5. 緑 (B:0, G:255, R:0)
      { 
        0, 255, 150                                     }
      ,  // 6. 黄緑 (B:0, G:255, R:150)
      { 
        0, 255, 255                                     }
      ,  // 7. 黄 (B:0, G:255, R:255)
      { 
        0, 100, 255                                     }
      ,  // 8. オレンジ (B:0, G:100, R:255)
      { 
        0, 0, 255                                     }     // 9. 赤 (B:0, G:0, R:255)
    };

    // 各ブロックのX座標を計算
    int block_x_start = meter_start_x + num_blocks_to_draw * bar_width;
    int block_x_end = block_x_start + bar_width - 1;
    // 現在のブロックの色を設定
    int current_b = colors[num_blocks_to_draw][0];
    int current_g = colors[num_blocks_to_draw][1];
    int current_r = colors[num_blocks_to_draw][2];
    // ブロックを描画
    display.stroke(current_b, current_g, current_r);
    display.fill(current_b, current_g, current_r);
    rectPrint(block_x_start, meter_y_start, block_x_end, meter_y_end);

    // 速度が変化した場合、最大スペースも調整してランダム性を維持
    if (rect_speed != old_speed) {
      max_rect_space = min_rect_space + 10; // スペースの最大値も最小値に連動させる
    }

    // 長方形の速度が以前より遅くなるのを防ぐ
    if(rect_speed < old_speed) rect_speed = old_speed;

    // 強制的な最小スペース制限 (キャラクターが通れる最小限)
    if(min_rect_space <= char_area[1] + 2) min_rect_space = char_area[1] + 2; 

    // 新しい長方形のスペースの長さを計算
    rect_space = random(min_rect_space,max_rect_space + 1); 

    // 最小高さ: 8
    const int max_rect_height_for_random = screen_height - score_borad_height - rect_space + 1;
    rect_height = random(8, max_rect_height_for_random); // 長方形の高さを、ゲームエリアの範囲内でランダムに設定
  }
}


































