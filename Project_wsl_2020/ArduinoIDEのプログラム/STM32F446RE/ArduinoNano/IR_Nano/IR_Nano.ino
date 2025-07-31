int IR_value[8], IR_near[2];
int angle = 0;
//ex : IR_value[0] = analogRead(0)
//ex : IR_near[0] = min_IRpin  ,  IR_near[0] = analogRead(min_IRpin)

void setup() {
  Serial.begin(9600);
}

void loop() {
  get_IR();
  get_angle();

  Serial.println();
  delay(10);
}

//function

void get_IR() {
  Serial.print("| IR_value : ");
  for (int i = 0; i <= 7; i++) {
    IR_value[i] = analogRead(i);
    Serial.print(IR_value[i]);
    Serial.print("  ");
  }

  IR_near[0] = 0;
  IR_near[1] = IR_value[0];

  if (IR_value[0] > 1000 && IR_value[1] > 1000 && IR_value[2] > 1000 && IR_value[3] > 1000 && IR_value[4] > 1000 && IR_value[5] > 1000 && IR_value[6] > 1000 && IR_value[7] > 1000) {
    IR_near[0] = -1;
    IR_near[1] = -1;
  } else {
    for (int i = 1; i <= 7; i++) {
      if (IR_value[i] < IR_near[1]) {
        IR_near[0] = i;
        IR_near[1] = IR_value[i];
      }
    }
  }
}

void get_angle() {
  int IR_three[3];
  IR_three[0] = IR_near[0] - 1;
  IR_three[1] = IR_near[0];
  IR_three[2] = IR_near[0] + 1;
  if (IR_three[0] < 0) {
    IR_three[0] = 7;
  }
  if (IR_three[2] > 7) {
    IR_three[2] = 0;
  }
  if (IR_three[1] == -1) {
    IR_three[0] = -1;
    IR_three[2] = -1;
  }
  Serial.print("| IR_min : ");
  Serial.print(IR_three[0]);
  Serial.print(" , ");
  Serial.print(IR_three[1]);
  Serial.print(" , ");
  Serial.print(IR_three[2]);
  Serial.print(" ,");
  Serial.print("  ->  ");

  if (IR_three[0] == -1) {
    angle = -1;
  } else {
    angle = ( ( 45 * IR_value[IR_three[0]] ) / IR_value[IR_three[2]] ) + 45 * IR_three[1];
  }
  Serial.print(angle);
}
