int Switch = 0;
void setup() {
  // put your setup code here
  Serial1.begin(38400);
  Serial.begin(38400);
  pinMode(0 ,INPUT);
}
void loop() {
  if( digitalRead(0) == HIGH ){
    Switch = 1;
  }else{
    Switch = 10;
  }
  Serial1.write(Switch);
  Serial.println(Switch);
}
