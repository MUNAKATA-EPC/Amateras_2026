//LEDの個数・ピン番号を指定
const int LEDs = 10;
const int LEDpins[LEDs] = { 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
//アナログのピン番号を指定
const int Analogpin = 0;
const int LightNum = 140;
const int DarkNum = 450;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  for (int i = 0; i < LEDs; i++) {
    pinMode(LEDpins[i], OUTPUT);
  }
  pinMode(Analogpin, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int AnalogValue = analogRead(Analogpin);  //明るい120~620暗い
  Serial.print(String(AnalogValue) + " -> ");
  AnalogValue = constrain(AnalogValue, LightNum, DarkNum);
  AnalogValue = map(AnalogValue, LightNum, DarkNum, 1023, 0);
  Serial.println(AnalogValue);

  for (int i = 1; i <= LEDs; i++) {
    bool LEDlevel = (AnalogValue > i * (1023 / (LEDs + 2)));
    digitalWrite(LEDpins[i - 1], LEDlevel);
  }

  delay(100);
}
