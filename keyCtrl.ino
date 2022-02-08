void keyInit(){
  pinMode(rebootPin, OUTPUT);
  digitalWrite(rebootPin, HIGH);
}


void rebootSK(){
  digitalWrite(rebootPin, LOW);
  delay(500);
  digitalWrite(rebootPin, HIGH);
}
