#include <Arduino.h>

// 定数
#define M1_D1 4
#define M1_D2 3
#define M1_PWM 2
#define M2_D1 7
#define M2_D2 6
#define M2_PWM 5
#define Start_PIN 9
// 変数

// 関数
void MotorMove(int PWM_val); // MotorMove(M1の出力値,M2の出力値); 出力値の範囲は、-100～100

void setup()
{
  pinMode(M1_D1, OUTPUT);
  pinMode(M1_D2, OUTPUT);
  pinMode(M1_PWM, OUTPUT);

  pinMode(M2_D1, OUTPUT);
  pinMode(M2_D2, OUTPUT);
  pinMode(M2_PWM, OUTPUT);

  pinMode(Start_PIN, INPUT_PULLUP);
}

void loop()
{
  for(int i = -100;i <= 100;i ++){
    MotorMove(i);
    delay(100);
  }
  for(int i = 100;i >= -100;i --){
    MotorMove(i);
    delay(100);
  }
}

void MotorMove(int PWM_val)
{
  if (PWM_val > 0)
  {
    digitalWrite(M1_D1, HIGH);
    digitalWrite(M1_D2, LOW);

    digitalWrite(M2_D1, HIGH);
    digitalWrite(M2_D2, LOW);
  }
  else
  {
    digitalWrite(M1_D1, LOW);
    digitalWrite(M1_D2, HIGH);

    digitalWrite(M2_D1, LOW);
    digitalWrite(M2_D2, HIGH);
  }
  analogWrite(M1_PWM, abs(PWM_val) * 255 / 100);
  analogWrite(M2_PWM, abs(PWM_val) * 255 / 100);
}