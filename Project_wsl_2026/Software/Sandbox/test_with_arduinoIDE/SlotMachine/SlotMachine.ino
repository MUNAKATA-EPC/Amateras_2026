//-----------------------------------------------------------------------
// スロットマシン１
//-----------------------------------------------------------------------
#include <LedControl.h>
#include <Tone.h> 

//タクトスイッチのピン設定
#define SW_LEFT_PIN 0
bool SW_Left = 0;
#define SW_MIDDLE_PIN 1
bool SW_Middle = 0;
#define SW_RIGHT_PIN 2
bool SW_Right = 0;
//フラグ変数
int flag_SlotRight;
int flag_SlotMiddle;
int flag_SlotLeft;
//スロットナンバー変数
int SlotLeft_num = random(0, 10);
int SlotMiddle_num = random(0, 10);
int SlotRight_num = random(0, 10);
//スロットタイマー更新時間初期設定
#define LEFT_SHOETIME 100
#define MIDDLE_SHOETIME 100
#define RIGHT_SHOETIME 100
int SlotLeft_ShowTime = LEFT_SHOETIME;
int SlotMiddle_ShowTime = MIDDLE_SHOETIME;
int SlotRight_ShowTime = RIGHT_SHOETIME;
//スロットタイマー変数
long SlotLeft_OldTime = millis();
long SlotMiddle_OldTime = millis();
long SlotRight_OldTime = millis();
long SlotLeft_NowTime = 0;
long SlotMiddle_NowTime = 0;
long SlotRight_NowTime = 0;
//減速機構の速度設定
#define SLOWADD 135
#define SLOWTIME 500

//7セグメントLEDの設定
LedControl lc = LedControl(11, 13, 1); 
//ブザーの設定
Tone tone1;
char *SWPushSounds = "M1:d=4,o=5,b=120:16g,16c6"; //スイッチ効果音
char *SWStopSounds = "M1:d=4,o=5,b=120:16g"; //スイッチ効果音
char *SuccessSounds = "M4:d=4,o=5,b=120:8c,8g,8c6,8g,8c6";//成功の効果音
char *Failure1Sounds = "M6:d=4,o=5,b=100:g3,g3";//失敗の効果音
char *Failure2Sounds = "M7:d=8,o=4,b=140:c,c#,d,d#,e";//失敗の効果音
char *Failure3Sounds = "M8:d=4,o=6,b=120:c6,b,a,g";//失敗の効果音
char *Failure4Sounds = "M9:d=8,o=5,b=100:c6,8p,a,8p,f";//失敗の効果音
char *Failure5Sounds = "M10:d=16,o=5,b=90:e,d#,d,c#,c";//失敗の効果音

void setup(){
  //7セグメントLEDの初期化
  lc.shutdown(0, false);             //７セグメントLEDをリセット
  lc.setIntensity(0, 5);             //色の濃さ
  lc.clearDisplay(0);                //表示クリア
  //ブザーのピン設定
  tone1.begin(8); 
  //フラグ変数を初期化
  Clear_flag();
}

void loop(){
  TurnSlot(); //スロットをターンさせる

  get_SW(); //タクトスイッチの情報を入手
  if(SW_Left && flag_SlotLeft == 0 && flag_SlotMiddle != 1  && flag_SlotRight != 1){
    flag_SlotLeft = 1;
  }
  if(SW_Middle && flag_SlotMiddle == 0 && flag_SlotLeft != 1  && flag_SlotRight != 1){
    flag_SlotMiddle = 1;
  }
  if(SW_Right && flag_SlotLeft != 1  && flag_SlotMiddle != 1){
    if(flag_SlotRight == 0){
      flag_SlotRight = 1;
    }
    else if(flag_SlotLeft == 2 && flag_SlotMiddle == 2 && flag_SlotRight == 2){
      //SW_rightが1600msよりも押されたらスロットをもう一度回し始める
      if(SW_RightPushTime() > 16)Clear_flag();
    }
  }

  //結果発表のサウンド
  if(flag_SlotLeft == 2 && flag_SlotMiddle == 2 && flag_SlotRight == 2){
    if(SlotLeft_num == SlotMiddle_num && SlotLeft_num == SlotRight_num){
      delay(500);
      
      tone1.play_rtttl(SuccessSounds);
    }
    else{
      delay(500);
      
      int i = random(1,6);
      switch(i){
      case 1:
        tone1.play_rtttl(Failure1Sounds);
        break;
      case 2:
        tone1.play_rtttl(Failure2Sounds);
        break;
      case 3:
        tone1.play_rtttl(Failure3Sounds);
        break;
      case 4:
        tone1.play_rtttl(Failure4Sounds);
        break;
      case 5:
        tone1.play_rtttl(Failure5Sounds);
        break;
      }
    }
    while(!SW_Right){
      get_SW(); //タクトスイッチの情報を入手
    }
  }
}

void get_SW(){
  SW_Left = (analogRead(SW_LEFT_PIN) > 500);
  SW_Middle = (analogRead(SW_MIDDLE_PIN) > 500);
  SW_Right = (analogRead(SW_RIGHT_PIN) > 500);
}

void TurnSlot(){
  SlotLeft_NowTime = millis() - SlotLeft_OldTime;
  if(SlotLeft_NowTime >= SlotLeft_ShowTime){
    if(flag_SlotLeft == 0){
      SlotLeft_ShowTime = LEFT_SHOETIME;

      SlotLeft_num = (SlotLeft_num + 1) % 10;

      lc.setDigit(0, 0, SlotLeft_num, false);       //一番左の列に結果を表示
    }
    else{
      if(SlotLeft_ShowTime <= SLOWTIME){
        tone1.play_rtttl(SWStopSounds);

        SlotLeft_ShowTime += SLOWADD;
        SlotLeft_num = (SlotLeft_num + 1) % 10;
      }
      else{
        if(flag_SlotLeft == 1){
          tone1.play_rtttl(SWPushSounds);
          flag_SlotLeft = 2;
        }
      }
      lc.setDigit(0, 0, SlotLeft_num, false);       //一番左の列に結果を表示
    }

    SlotLeft_OldTime = millis();
  }

  SlotMiddle_NowTime = millis() - SlotMiddle_OldTime;
  if(SlotMiddle_NowTime >= SlotMiddle_ShowTime){
    if(flag_SlotMiddle == 0){
      SlotMiddle_ShowTime = MIDDLE_SHOETIME;

      SlotMiddle_num = (SlotMiddle_num + 1) % 10;

      lc.setDigit(0, 1, SlotMiddle_num, false);       //一番左の列に結果を表示
    }
    else{
      if(SlotMiddle_ShowTime <= SLOWTIME){
        tone1.play_rtttl(SWStopSounds);

        SlotMiddle_ShowTime += SLOWADD;
        SlotMiddle_num = (SlotMiddle_num + 1) % 10;
      }
      else{
        if(flag_SlotMiddle == 1){
          tone1.play_rtttl(SWPushSounds);
          flag_SlotMiddle = 2;
        }
      }
      lc.setDigit(0, 1, SlotMiddle_num, false);       //一番左の列に結果を表示
    }

    SlotMiddle_OldTime = millis();
  }

  SlotRight_NowTime = millis() - SlotRight_OldTime;
  if(SlotRight_NowTime >= SlotRight_ShowTime){
    if(flag_SlotRight == 0){
      SlotRight_ShowTime = RIGHT_SHOETIME;

      SlotRight_num = (SlotRight_num + 1) % 10;

      lc.setDigit(0, 2, SlotRight_num, false);       //一番左の列に結果を表示
    }
    else{
      if(SlotRight_ShowTime <= SLOWTIME){
        tone1.play_rtttl(SWStopSounds);

        SlotRight_ShowTime += SLOWADD;
        SlotRight_num = (SlotRight_num + 1) % 10;
      }
      else{
        if(flag_SlotRight == 1){
          tone1.play_rtttl(SWPushSounds);
          flag_SlotRight = 2;
        }
      }
      lc.setDigit(0, 2, SlotRight_num, false);       //一番左の列に結果を表示
    }

    SlotRight_OldTime = millis();
  }
}

void Clear_flag(){
  flag_SlotRight = 0;
  flag_SlotMiddle = 0;
  flag_SlotLeft = 0;
}

int SW_RightPushTime(){
  int i = 0; //計測用の変数を初期化
  while(SW_Right){
    get_SW(); //タクトスイッチの情報を入手
    delay(100);
    i ++;
  }
  return(i); //計測した数値を返す
}
































