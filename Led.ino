void ledInit(){
   pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void ledWatcher(){
  switch (globalState[0]) {
    case 0:
     blinkLed(800);
    break;
    case 1:
     digitalWrite(ledPin, HIGH);
    break;
    case 2:
      ledBlink_2(200, 1000);
    break;
    case 3:
      digitalWrite(ledPin, HIGH);
    break;
  }
}


void blinkLed(int timer){
  static unsigned long t = 0;
  static bool timerFlag = false;

  if(millis() - t > timer){
    timerFlag = !timerFlag;
    digitalWrite(ledPin, timerFlag);
    t = millis();
  }
}

void ledBlink_2(int timer_1, int timer_2){
  static unsigned long t = 0;
  static bool timerFlag = false;
  static int blinkCount = 2 ;
  static bool timerFlag_2 = false;

  
  if(millis() - t > timer_1 && !timerFlag){
    t = millis();
    
    timerFlag_2 = !timerFlag_2;
    digitalWrite(ledPin, timerFlag_2);
    if(!timerFlag_2){
      blinkCount--;
    }
    if(blinkCount == 0){
      timerFlag = true;
    }  
    }else if(millis() - t > timer_2 && timerFlag){
      t = millis();
      timerFlag = false;
      blinkCount = 2;
    }
}
