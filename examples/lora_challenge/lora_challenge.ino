//////////////////////////////////////////////////////
// This sample code is used for ShareCourse on MOSTLink protocol
//
//
//////////////////////////////////////////////////////

#include "MOSTLora.h"
#include "MLutility.h"
#include <DHT.h>

MOSTLora lora;
DHT dht(2, DHT22);

int szBuf = 0;
byte buf[100] = {0};
float fTemperature, fHumidity;

#define KEY_PUBLIC_CHALLENGE   "PublicKey"

// callback for CMD_REQ_DATA
void funcPacketReqData(unsigned char *data, int szData)
{
  debugSerial.print("ReqData= ");

  dht.readSensor(fHumidity, fTemperature, true);
  lora.sendPacketResData(fHumidity, fTemperature);
}

void setup() {
  Serial.begin(9600);  // use serial port for log monitor
  
  lora.begin();
  // custom LoRa config by your environment setting
  lora.writeConfig(915555, 0, 0, 7, 5);
  lora.setMode(E_LORA_POWERSAVING);         // power saving mode
  lora.setKeyHMAC(KEY_PUBLIC_CHALLENGE);

  // custom callback
  lora.setCallbackPacketReqData(funcPacketReqData);
  
  // init sensor for humidity & temperature
  dht.begin();
  int i = 0;
  boolean bReadDHT = false;
  while (!bReadDHT && i < 20) {
    bReadDHT = dht.readSensor(fHumidity, fTemperature, true);
    delay(1000);
    i++;
  }
    
  Serial.println("REQ_AUTH_JOIN");
  lora.sendPacketReqAuthJoin();
}

void loop() {
  lora.run();

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
