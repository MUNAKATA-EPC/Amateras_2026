void setup() {
  // put your setup code here, to run once:
  pinMode(10, OUTPUT);
  digitalWrite(10, LOW);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(analogRead(0));
  if(analogRead(0) > 750){
    digitalWrite(10, HIGH);
  }else{
    digitalWrite(10, LOW);
  }
}
