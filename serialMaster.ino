//Модуль реализации протокола связи


void serialTimer(int timer){
  static unsigned long t = 0;
  if(millis() - t > timer){
    serialMaster();
    t = millis();
  }
}


void serialMaster(){
  static int lengthGlobalState = sizeof(globalState) / sizeof(globalState[0]);
  String acc = "$";
  unsigned int crc = 0;
  static int lengthBufferComand = 3;
  unsigned int bufferComand[lengthBufferComand]; // буфер команд управления всегда+1 для CRC
  static unsigned long errorTimer = 0;

  
  for(byte i = 0; i < lengthGlobalState; i++){ // расчет CRC
    crc += globalState[i];
  }
  
  for(byte i = 0; i < lengthGlobalState; i++){ // формируем посылку 
    if(lengthGlobalState - 1 == i){
      acc+= crc;
      acc+= ";";
    }else{
      acc+= globalState[i];
      acc+= " ";
    }  
  }
  
  Serial.println(acc); // отправляем посылку

   if(parsePacket((int*)bufferComand)){
    globalState[2] = 1; //данные были приняты и они правильной структуры
    crc = 0; //очищаем преведущую CRC
    for(byte i = 0; i < lengthBufferComand - 1; i++){ // расчет CRC
      crc += bufferComand[i];
    }
    if(bufferComand[lengthBufferComand - 1] == crc){ //проверка контрольной суммы пройдена
      globalState[3] = bufferComand[0]; // команда на выполнение 
      globalState[4] = bufferComand[1]; // номер пакета 
      //обрабатываем принятые данные 
    }else{
       //uart.println("Error CRC !!");//обработка ошибки
    } 
  }else{
    globalState[2] = 0; // данные не были приняты
  }
  
}



boolean parsePacket(int *intArray) {
    if (Serial.available()) {
        uint32_t timeoutTime = millis();
        int value = 0;
        byte index = 0;
        boolean parseStart = 0;

        while (millis() - timeoutTime < 100) {
            if (Serial.available()) {
                timeoutTime = millis();
                if (Serial.peek() == '$') {
                    parseStart = true;
                    Serial.read();
                    continue;
                }
                if (parseStart) {
                    if (Serial.peek() == ' ') {
                        intArray[index] = value / 10;
                        value = 0;
                        index++;
                        Serial.read();
                        continue;
                    }
                    if (Serial.peek() == ';') {
                        intArray[index] = value / 10;
                        Serial.read();
                        return true;
                    }
                    value += Serial.read() - '0';
                    value *= 10;
                }
                else
                {
                    Serial.read(); //возможно не будет работать нужна очистка 
                }
            }
        }
    }
    return false;
}
