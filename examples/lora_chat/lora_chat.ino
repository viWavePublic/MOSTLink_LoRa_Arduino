//////////////////////////////////////////////////////
// This sample code is used for ShareCourse on MOSTLink protocol
//
//
//////////////////////////////////////////////////////

#include "MOSTLora.h"

MOSTLora lora;

int szBuf = 0;
byte buf[256] = {0};

void setup() {
  Serial.begin(9600);  // use serial port for log monitor
  
  lora.begin();
  // custom LoRa config by your environment setting
//  lora.writeConfig(915000, 0, 0, 7, 5);
  lora.setMode(E_LORA_NORMAL);         // E_LORA_NORMAL

  delay(1000);
  lora.sendData("Hi, how are you.");
}

void loop() {
  if (lora.available()) {
    szBuf = lora.receData();
    // reply to vinduino.io when receive "reply" message
    if (buf[0] == '/') {  // ack message
      char *strBuf = (char*)buf;
      strBuf[0] = '>';
      lora.sendData((byte*)buf, szBuf);
    }
  }
  delay(10);

  // command to send
  if (Serial.available())
    inputBySerial();
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
    Serial.print("chat> ");
    Serial.println((char*)buf);
    lora.sendData((char*)buf);
  }  
}
