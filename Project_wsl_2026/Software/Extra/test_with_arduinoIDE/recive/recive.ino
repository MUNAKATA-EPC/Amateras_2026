
void setup() {
  // put your setup code here, to run once:
  Serial1.begin(38400);
  Serial.begin(38400);
  pinMode(0, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if( Serial1.available() > 0 ){

    uint8_t recive = Serial1.read()
    Serial.print("Data -> ");
    Serial.println(recive);
    digitalWrite(0, HIGH);
    
  }else{

    digitalWrite(0, LOW);

  }

  delay(100);
}
