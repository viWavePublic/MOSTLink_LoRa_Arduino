
const int pinLed8 = 11;

#include "MOSTLora.h"

int count = 0;
const int pinTouch = 8;  // 12;
bool bPressTouch = false;
//Grove_LED_Bar bar(7, 6, false);

MOSTLora lora;

int szBuf = 0;
byte buf[256] = {0};
int tsLast;
int tsIdle = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(pinTouch, INPUT);
  pinMode(pinLed8, OUTPUT);

  digitalWrite(pinLed8, LOW);
  digitalWrite(pinTouch, LOW);

  Serial.begin(9600);

  lora.begin();
  lora.setMode(0, 1);
  
  char strHello[] = "Hello World...";
  int szHello = strlen(strHello);
  lora.waitUntilReady(3000);
//  lora.sendData((unsigned char*)strHello, szHello);
  lora.sendData(strHello);
}

void loop() {
  if (lora.available()) {
    szBuf = lora.receData(buf, 255);
  }
  lora.isBusy();

  // command to send
  if (Serial.available())
    inputBySerial();

  checkTouch();
  //digitalWrite(pinLed8, bPressTouch);   // turn the LED on (HIGH is the voltage level)
}

void inputBySerial()
{
  int countBuf = 0;
  while (Serial.available()) {
    int c = Serial.read();
    buf[countBuf] = c;

    countBuf++;    
    delay(20);
  }
  if (countBuf > 0) {
    buf[countBuf] = 0;
    Serial.print("Command> ");
    Serial.println((char*)buf);
    if (!parseCommand((char*)buf)) {
//      lora.sendData(buf, countBuf);
      lora.sendData((char*)buf);
    }
  }  
}

boolean parseCommand(char *strCmd)
{
  if (strCmd[0] != '/' || strlen(strCmd) < 2)
    return false;

  int nCmd = strCmd[1];
  switch (nCmd) {
    case '0':
      lora.writeConfig(915000, 0, 0, 7, 5);
      break;
    case '1':
      lora.writeConfig(915000, 0, 1, 7, 5);
      break;
    case '2':
      lora.writeConfig(915000, 0, 2, 7, 5);
      break;
    case '3':
      lora.writeConfig(868000, 0, 0, 7, 5);
      break;
    case '4':
      lora.writeConfig(868000, 0, 2, 7, 5);
      break;
    case 'f':
      lora.writeConfig(868000, 0, 2, 7, 5);
      break;
    default:
      lora.writeConfig(915000, 0, 0, 7, 5);
      break;
  }
  
  lora.setMode(0, 0);
  return true;
}


void checkTouch()
{
  const int nSensorTouch = digitalRead(pinTouch);
  if (!bPressTouch) {
    if (nSensorTouch > 0) {   // key-down
      Serial.println("touch-DOWN");
      bPressTouch = true;
      count = (count + 1) % 11;
//      bar.setLevel(count);

      const char *strData = "ABC...123";
//      const char *strData = "Hello Hello Hello 0123456789!!!";
      lora.sendData((byte*)strData, strlen(strData));
    }
  }
  else {
    if (nSensorTouch == 0) {  // key-up
      Serial.println("touch-UP");
      bPressTouch = false; 
    }   
  }  
}


