//////////////////////////////////////////////////////
// Simple chat sample between Gateway <-> Lora
//
//    Gateway: chat by Send TEXT(HEX)
//    Lora Node: chat by Serial Monitor
//////////////////////////////////////////////////////

#include "MOSTLora.h"
#include "MLutility.h"

MOSTLora lora;

byte buf[100] = {0};

// callback for rece data
void funcCustomRece(unsigned char *data, int szData)
{ 
  debugSerial.print("funcCustomRece= ");
  MLutility::printBinary(data, szData);
}

// callback for CMD_REQ_DATA
void funcPacketReqData(unsigned char *data, int szData)
{
  memcpy(buf, data, szData);  
  buf[szData] = 0;
  debugSerial.print("ReqData= ");
  debugSerial.println((const char*)buf);
}

void setup() {
  Serial.begin(9600);  // use serial port for log monitor
  Serial.println(F("*** lora_03_chat ***"));
  
  lora.begin();
  // config setting: frequency, group, data-rate, power, wakeup-time
  lora.writeConfig(915555, 0, 0, 7, 5);
  lora.setMode(E_LORA_NORMAL);         // E_LORA_NORMAL

  delay(1000);
  lora.sendData("Hi, how are you.");

  // custom callback
  lora.setCallbackReceData(funcCustomRece);
  lora.setCallbackPacketReqData(funcPacketReqData);
}

void loop() {
  lora.run();
  delay(100);

  // command to send
  if (Serial.available())
    inputBySerial();
}

void inputBySerial()
{
  int countBuf = MLutility::readSerial((char*)buf);
  if (countBuf > 0) {
    buf[countBuf] = 0;
    Serial.print("chat> ");
    Serial.println((char*)buf);
    lora.sendData((char*)buf);
  }  
}
