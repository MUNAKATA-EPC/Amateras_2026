String i = "";
void setup() {
  // put your setup code here, to run once:
Serial1.begin(38400);
Serial.begin(38400);
}

void loop() {
  // put your main code here, to run repeatedly:
if(Serial1.available() > 0){
  i = Serial1.read();
  Serial.println(i);
}
}
