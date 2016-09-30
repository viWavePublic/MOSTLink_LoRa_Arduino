//////////////////////////////////////////////////////
// This sample code is used for ShareCourse on MOSTLink protocol
//
//
//////////////////////////////////////////////////////

#include "MOSTLora.h"
#include "MLutility.h"
#include <LGPS.h>

MOSTLora lora;

void setup() {
  Serial.begin(9600);  // use serial port for log monitor

  LGPS.powerOn();
  lora.begin();
  // custom LoRa config by your environment setting
//  lora.writeConfig(915000, 0, 0, 7, 5);
  lora.setMode(E_LORA_WAKEUP);

  delay(1000);
  lora.sendData("Hi, GPS!");
}

char buf[256] = {0};
int szBuf = 0;
long tsGPS = 0;
long tsCurr = millis();
gpsSentenceInfoStruct info;

void loop() {
  if (lora.available()) {
    szBuf = lora.receData((unsigned char*)buf, 255);
    // reply to vinduino.io when receive "reply" message
    if (buf[0] == '/') {  // ack message
      char *strBuf = (char*)buf;
      strBuf[0] = '>';
      lora.sendData((byte*)buf, szBuf);
    }
  }
  delay(10);

  tsCurr = millis();
  if (tsCurr - tsGPS > 10000) {
    tsGPS = tsCurr;
    
    Serial.println("LGPS loop"); 
    LGPS.getData(&info);

    const char* strGPS = (const char*)info.GPGGA;
    double dbLat, dbLng;
    MLutility::parseGPGGA(strGPS, dbLat, dbLng);
    Serial.println(strGPS); 
    
    char strOut[255];
    sprintf(strOut, "GPS(%8.6f, %8.6f)", dbLat, dbLng); 
    lora.sendData((char*)strOut);
  }

  // serial command to send
  int countBuf = MLutility::readSerial(buf);
  if (countBuf > 0) {
    buf[countBuf] = 0;
    Serial.print("chat> ");
    Serial.println((char*)buf);
    lora.sendData((char*)buf);
  }
}

void inputBySerial()
{
  
}
