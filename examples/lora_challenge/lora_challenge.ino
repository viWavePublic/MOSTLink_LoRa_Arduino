//////////////////////////////////////////////////////
// This sample code is used for ShareCourse on MOSTLink protocol
//
//
//////////////////////////////////////////////////////

#include "MOSTLora.h"
#include "MLutility.h"

MOSTLora lora;

//#define USE_DHT
#ifdef USE_DHT
#include <DHT.h>
DHT dht(2, DHT22);
#endif // USE_DHT
int szBuf = 0;
byte buf[100] = {0};
float fTemperature = 23.6, fHumidity = 51.5;

#define KEY_PUBLIC_CHALLENGE   "PublicKey"

// callback for CMD_REQ_DATA
void funcPacketReqData(unsigned char *data, int szData)
{
  debugSerial.print("ReqData= ");

#ifdef USE_DHT
 dht.readSensor(fHumidity, fTemperature, true);
#endif // USE_DHT

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
  
#ifdef USE_DHT
  // init sensor for humidity & temperature
  dht.begin();
  int i = 0;
  boolean bReadDHT = false;
  while (!bReadDHT && i < 20) {
    bReadDHT = dht.readSensor(fHumidity, fTemperature, true);
    delay(600);
    i++;
  }
#endif // USE_DHT
    
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
