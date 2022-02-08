//SK_WatchDog
//v1.0.0
#include <GyverWDT.h>
   
//general pin
const int usbVotagePin = 7;
const int ledPin = 6;
const int rebootPin = 5;

unsigned int globalState[15]={
  0, //0 состояние 0 - загрузка SK, 1 - рабочий режим , 2 - режим отладки(подключен USB)
  0, //1 статус подключения кабеля для отладки 
  0, //2 статус активности SK (если SK активен 1 если нет то 0)
  0, //3 команды принятые 
  0, //4 последний номер пакета 
  0, //5 питание модема (не реализованно! 1 - включен, 0 - выключен)
  0, //6 Напряжение питания основное (не реализованно!)
  0, //7 Напряжение питания резервное (не реализованно!)
  0, //8 Номер рабочего канала питания(не реализованно! 0 - основной, 1 - резервный )
  0, //9 (Запрет перезагрузки 0 - работает 1 - запрет)
  0, //10 Обратная связь от переключателя модема (0 - М1, 1 - М2)
  0, //11 (не реализованно!)
  0, //12 (не реализованно!)
  100, //13  версия прошивки вач дога 
  0  //14 CRC
};

void setup(){
  keyInit();
  Serial.begin(57600);
  ledInit();
  gpioInit();
  pinMode(usbVotagePin, INPUT);
  Watchdog.enable(RST_MODE, WDT_TIMEOUT_4S);
}




void loop(){
  usbVoltageWatcher(); // смотрим наличие напряжения на разъеме USB
  serialTimer(1000); // 
  skWatcher(); // Сотрим состояние SK
  ledWatcher();
  statusWatcher(); // Смотрим показатели и меняем состояние
  modemSwichWatcher();
  comandWatcher();
  watchDogStopReboot();
  Watchdog.reset();
}

void usbVoltageWatcher(){
  if(digitalRead(usbVotagePin)){
    globalState[1] = 1;
  }else{
    globalState[1] = 0;
  }  
}


void statusWatcher(){
  if(globalState[1] == 1){ // подключен кабель отладки
    globalState[0] = 2; //ставим статус работы по отладочному кабелю 
  }else if(globalState[2] == 1 || globalState[9] == 1){ //SK работает (либо комп активен либо защита отключена)
    globalState[0] = 1; // ставим статус нормальной работы 
  }else{
    globalState[0] = 0;  //SK не работает пытаемся загрузить
  }
}

void skWatcher(){
  static unsigned long t = 0;
  static bool timerFlag = false;
  const long timer = 10000;
  if(globalState[2] == 1){
    t = millis();
  }else if(millis() - t > timer && globalState[0] != 2){ 
    if(globalState[9] == 0){
      rebootSK();
    }
    t = millis();
  }
}


void comandWatcher(){
/*
Лист директив:
 1) 69 - Перезагрузка микроконтроллера
 2) 11 - отключение перезагрузки вач догом
 3) 12 - включение перезагрузки вач догом(отмена директивы 11)
 4) 13 - выбор модема М1
 5) 14 - выбор модема М2

*/
  

  switch (globalState[3]){
    case 69:
     while(true){}
    break;
    case 11:
     //
    break;
    case 12:
     //
    break;
    case 13:
     modemSwitch(1); //Выбран 1М
    break;
    case 14:
     modemSwitch(2); //Выбран 2М
    break;
  }
  globalState[3] = 0; // очистка буфера команд 

  
/*
   while(globalState[3] == 69){} //перезагружаем МК
  if(globalState[3] == 11){
    globalState[9] = 1;
    //защита отключена 
  }else if(globalState[3] == 12){
    globalState[9] = 0;
    //защита включена
  }else if(globalState[3] == 13){
    modemSwitch(1);
    //Выбран 1М
  }else if(globalState[3] == 14){
    modemSwitch(2);
    //Выбран 2М
  }

  */
}
