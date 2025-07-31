double x, y, edge;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  //edge
  edge = IR_edge();
  Serial.print("edge : ");
  Serial.println(edge);
  /*
  //x
  x = 100 * cos(edge * (PI / 180));
  Serial.print("  x : ");
  Serial.print(x);

  //y
  y = 100 * sin(edge * (PI / 180));
  Serial.print("  y : ");
  Serial.println(y);
  */
}



double IR_edge() {
  int IR_data[8] = {
    analogRead(1),
    analogRead(2),
    analogRead(3),
    analogRead(4),
    analogRead(5),
    analogRead(8),
    analogRead(9),
    analogRead(10),
  };
  double IR_edge,min_IR;
  int min_pin = 0, min1_pin, min2_pin;

  //最も小さいIRと周辺の2つのIRを見つける
  min_IR = IR_data[0];
  for (int i = 0; i++; i <= 7) {
    if (IR_data[i] < min_IR) {
      min_IR = IR_data[i];
      min_pin = i;
    }
  }
  min1_pin = min_pin - 1;
  if (min1_pin == -1) min1_pin = 7;
  min2_pin = min_pin + 1;
  if (min2_pin == 8) min2_pin = 0;

  //角度を求めていく
  switch (min_pin) {
    case 0:
      IR_edge = 45 * (IR_data[min2_pin] / (IR_data[min1_pin] + IR_data[min2_pin])) + 90;
      break;
    case 1:
      IR_edge = 45 * (IR_data[min2_pin] / (IR_data[min1_pin] + IR_data[min2_pin])) + 45;
      break;
    case 2:
      IR_edge = 45 * (IR_data[min2_pin] / (IR_data[min1_pin] + IR_data[min2_pin])) + 0;
      break;
    case 3:
      IR_edge = 45 * (IR_data[min2_pin] / (IR_data[min1_pin] + IR_data[min2_pin])) + 315;
      break;
    case 4:
      IR_edge = 45 * (IR_data[min2_pin] / (IR_data[min1_pin] + IR_data[min2_pin])) + 270;
      break;
    case 5:
      IR_edge = 45 * (IR_data[min2_pin] / (IR_data[min1_pin] + IR_data[min2_pin])) + 225;
      break;
    case 6:
      IR_edge = 45 * (IR_data[min2_pin] / (IR_data[min1_pin] + IR_data[min2_pin])) + 180;
      break;
    case 7:
      IR_edge = 45 * (IR_data[min2_pin] / (IR_data[min1_pin] + IR_data[min2_pin])) + 135;
      break;
  }
  IR_edge = IR_edge - 22.5;
  if (IR_edge < 0) IR_edge = 360 - IR_edge;

  return (IR_edge);
}
