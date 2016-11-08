//////////////////////////////////////////////////////
// This sample code is used for ShareCourse on MOSTLink protocol
//
//
//////////////////////////////////////////////////////

#include "MOSTLora.h"
#include "MLutility.h"
#include <LGPS.h>

MOSTLora lora;

const int pinTouch = A0;  // 12;
bool bPressTouch = false;
void checkTouch()
{
//  const int nTouchVal = digitalRead(pinTouch);
  const int nTouchVal = analogRead(pinTouch);
  if (!bPressTouch) {
    if (nTouchVal >= 512) {   // key-down
      Serial.print(nTouchVal, DEC);
      Serial.println(") touch-DOWN");
      bPressTouch = true;
//      count = (count + 1) % 11;
//      bar.setLevel(count);

      const char *strData = "ABC...123";
//      const char *strData = "Hello Hello Hello 0123456789!!!";
//      lora.sendData((byte*)strData, strlen(strData));
//      sendGPS();
    }
  }
  else {
    if (nTouchVal == 0) {  // key-up
      Serial.println("touch-UP");
      bPressTouch = false; 
    }   
  }  
}

// callback for REQ_DATA
void funcCustomPacketReqData(unsigned char *data, int szData)
{
  debugSerial.println("ReqData= sendGPS");
  sendGPS();
}

void setup() {
  pinMode(pinTouch, INPUT);
  digitalWrite(pinTouch, LOW);
  
  Serial.begin(9600);  // use serial port for log monitor

  LGPS.powerOn();
  lora.begin();
  // custom LoRa config by your environment setting
  lora.writeConfig(915555, 0, 0, 7, 5);
  lora.setMode(E_LORA_WAKEUP);

  delay(1000);
  lora.sendData("Hi, GPS!");

  // custom callback
  lora.setCallbackPacketReqData(funcCustomPacketReqData);
}

byte buf[99] = {0};
int szBuf = 0;
long tsGPS = 0;
long tsCurr = millis();
gpsSentenceInfoStruct info;

void loop() {
  lora.run();
  if (lora.available()) {
    szBuf = lora.receData();
    if (szBuf >= 2) {
      Serial.print(szBuf);  // use serial port
      Serial.println(") Parse rece <<<");  // use serial port
      if (lora.parsePacket() == 0x0004) {  // REQ_DATA
      Serial.println("---> this node");  // use serial port
        sendGPS();
      }
    }
  }
  delay(10);

  tsCurr = millis();
  if (tsCurr - tsGPS > 100000) {
    tsGPS = tsCurr;
//    sendGPS();
  }

  // serial command to send
  int countBuf = MLutility::readSerial((char*)buf);
  if (countBuf > 0) {
    buf[countBuf] = 0;
    Serial.print("chat> ");
    Serial.println((char*)buf);
    lora.sendData((char*)buf);
  }
  // touch event
  checkTouch();
}

void sendGPS()
{
    Serial.println("LGPS loop"); 
    LGPS.getData(&info);

    const char* strGPS = (const char*)info.GPGGA;
    unsigned long ts;
    char gpsStatus;
    double dbLat, dbLng;

    MLutility::parseGPGGA(strGPS, ts, dbLat, dbLng, gpsStatus);
    Serial.println(strGPS); 
    
    char strOut[255];
    sprintf(strOut, "ts: %d, GPS(%8.6f, %8.6f)", ts, dbLat, dbLng); 
    Serial.println(strOut); 
//    lora.sendData((char*)strOut);  

    lora.sendPacketReqSOS(1234321, 3, dbLat, dbLng, 88);
}

