#include "pitches.h"

void setup() {
  Serial.begin(9600);
  for (int i = 3; i <= 13; i++)
    pinMode(i, INPUT_PULLUP);
}

void loop() {
  if (digitalRead(10) != HIGH && digitalRead(12) == HIGH) {
    if (digitalRead(9) != HIGH)
      tone(A5, NOTE_C3, 10);
    if (digitalRead(8) != HIGH)
      tone(A5, NOTE_D3, 10);
    if (digitalRead(7) != HIGH)
      tone(A5, NOTE_E3, 10);
    if (digitalRead(6) != HIGH)
      tone(A5, NOTE_F3, 10);
    if (digitalRead(5) != HIGH)
      tone(A5, NOTE_G3, 10);
    if (digitalRead(4) != HIGH)
      tone(A5, NOTE_A3, 10);
    if (digitalRead(3) != HIGH)
      tone(A5, NOTE_B3, 10);
  } else if (digitalRead(10) == HIGH && digitalRead(12) == HIGH) {
    if (digitalRead(9) != HIGH)
      tone(A5, NOTE_C4, 10);
    if (digitalRead(8) != HIGH)
      tone(A5, NOTE_D4, 10);
    if (digitalRead(7) != HIGH)
      tone(A5, NOTE_E4, 10);
    if (digitalRead(6) != HIGH)
      tone(A5, NOTE_F4, 10);
    if (digitalRead(5) != HIGH)
      tone(A5, NOTE_G4, 10);
    if (digitalRead(4) != HIGH)
      tone(A5, NOTE_A4, 10);
    if (digitalRead(3) != HIGH)
      tone(A5, NOTE_B4, 10);
  } else if (digitalRead(10) == HIGH && digitalRead(12) != HIGH) {
    if (digitalRead(9) != HIGH)
      tone(A5, NOTE_C5, 10);
    if (digitalRead(8) != HIGH)
      tone(A5, NOTE_D5, 10);
    if (digitalRead(7) != HIGH)
      tone(A5, NOTE_E5, 10);
    if (digitalRead(6) != HIGH)
      tone(A5, NOTE_F5, 10);
    if (digitalRead(5) != HIGH)
      tone(A5, NOTE_G5, 10);
    if (digitalRead(4) != HIGH)
      tone(A5, NOTE_A5, 10);
    if (digitalRead(3) != HIGH)
      tone(A5, NOTE_B5, 10);
  }else{
    if (digitalRead(9) != HIGH)
      tone(A5, NOTE_CS4, 10);
    if (digitalRead(8) != HIGH)
      tone(A5, NOTE_DS4, 10);
    if (digitalRead(7) != HIGH)
      tone(A5, NOTE_E4, 10);
    if (digitalRead(6) != HIGH)
      tone(A5, NOTE_FS4, 10);
    if (digitalRead(5) != HIGH)
      tone(A5, NOTE_GS4, 10);
    if (digitalRead(4) != HIGH)
      tone(A5, NOTE_AS4, 10);
    if (digitalRead(3) != HIGH)
      tone(A5, NOTE_B4, 10);
  }
}
