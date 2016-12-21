//////////////////////////////////////////////////////
// This sample code is used for ThingSpeak project base on MOSTLink protocol
//
// https://thingspeak.com/login
//
//////////////////////////////////////////////////////

#include "MOSTLora.h"
#include <DHT.h>

MOSTLora lora;
DHT dht(2, DHT22);
float fTemperature, fHumidity;

const char *thingSpeakApiKey = "W00UTJRN68Z7HJJN";    // ThingSpeak API-key

// callback for REQ_DATA
void funcPacketReqData(unsigned char *data, int szData)
{
  debugSerial.println("ReqData= sendPacketThingSpeak");
  dht.readSensor(fHumidity, fTemperature, true);
  lora.sendPacketThingSpeak(thingSpeakApiKey, fHumidity, fTemperature, 33, 46, 59, 60, 70, 85);
}

void setup() {
#ifdef DEBUG_LORA
  Serial.begin(9600);  // use serial port for log monitor
  Serial.println(F("--- ThingSpeak ---"));  // app title
#endif // DEBUG_LORA
  
  lora.begin();
  // custom LoRa config by your environment setting
  lora.writeConfig(915555, 0, 0, 7, 5);
  lora.setMode(E_LORA_WAKEUP);

  delay(1000);
  
  // init sensor for humidity & temperature
  dht.begin();
  int i = 0;
  boolean bReadDHT = false;
  while (!bReadDHT && i < 20) {
    bReadDHT = dht.readSensor(fHumidity, fTemperature, true);
    delay(1000);
    i++;
  }

  // test to gateway
  const char *strTest = "Hi, ThingSpeak.";
  lora.sendData(strTest); 

  // custom callback
  lora.setCallbackPacketReqData(funcPacketReqData);
}

void loop() {
  lora.run();
  
  delay(100);
}

