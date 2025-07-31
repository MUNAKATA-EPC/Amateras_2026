#include <Arduino.h>
#include "MotorMove.h"

void MotorMove(int PWM_val){
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