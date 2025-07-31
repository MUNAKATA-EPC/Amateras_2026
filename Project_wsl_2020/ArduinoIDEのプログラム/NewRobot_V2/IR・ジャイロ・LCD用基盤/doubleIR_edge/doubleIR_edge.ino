void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int IR_values[8] = { analogRead(1), analogRead(2), analogRead(3), analogRead(4), analogRead(5), analogRead(8), analogRead(9), analogRead(10) };
  int IR_min1[2] = { 1000, -1 };
  for (int i = 0; i <= 7; i++) {
    if (IR_values[i] < IR_min1[0]) {
      IR_min1[0] = IR_values[i];
      IR_min1[1] = i;
    }
  }
  int IR_min2[2] = { 1000, -1 };
  for (int i = 0; i <= 7; i++) {
    if (IR_values[i] < IR_min2[0] && i != IR_min1[1]) {
      IR_min2[0] = IR_values[i];
      IR_min2[1] = i;
    }
  }
  Serial.print(IR_min1[1]);
  Serial.print(IR_min2[1]);
  Serial.print("  ");
}
