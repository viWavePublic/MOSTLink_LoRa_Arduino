//////////////////////////////////////////////////////
// This sample code is used for ShareCourse on MOSTLink protocol
//
//
//////////////////////////////////////////////////////

#include "MOSTLora.h"
#include "MLutility.h"
#include "MLPacketComm.h"

MOSTLora lora;

int szBuf = 0;
byte buf[256] = {0};

void setup() {
  Serial.begin(9600);  // use serial port for log monitor
  
  lora.begin();
  // custom LoRa config by your environment setting
  lora.writeConfig(915555, 0, 0, 7, 5);
  lora.setMode(E_LORA_POWERSAVING);         // E_LORA_NORMAL

  delay(1000);
  Serial.println("REQ_AUTH_JOIN");
  lora.sendPacketReqAuthJoin();
}

void loop() {
  if (lora.available()) {
    szBuf = lora.receData();
    if (szBuf >= 2) {
      Serial.print(szBuf);  // use serial port
      Serial.println(") Parse rece <<<");  // use serial port
      const unsigned int nCmdID = lora.parsePacket();
      if (nCmdID == CMD_REQ_DATA) {  // REQ_DATA
      }
      else if (nCmdID == CMD_REQ_AUTH_CHALLENGE) {  // REQ_AUTH_CHALLENGE
      }
    }
  }  
  delay(10);

  // command to send
  if (Serial.available())
    inputBySerial();
}

void inputBySerial()
{
  int countBuf = MLutility::readSerial((char*)buf);
  if (countBuf > 0) {
    buf[countBuf] = 0;
    Serial.println("REQ_AUTH_JOIN");
    lora.sendPacketReqAuthJoin();
  }  
}
