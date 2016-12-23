//////////////////////////////////////////////////////
// ThingSpeak project base on MOSTLink protocol
// upload sensors to cloud server
// https://thingspeak.com/login
//
// DHT dht(2, DHT22);         // for DHT22
// DHT dht(2, DHT11);         // for DHT11
//////////////////////////////////////////////////////

#include "MOSTLora.h"
#include "DHT.h"

MOSTLora lora;
DHT dht(2, DHT22);

float fTemperature, fHumidity;
const char *thingSpeakApiKey = "W00UTJRN68Z7HJJN";    // ThingSpeak API-key

// callback for REQ_DATA
void funcPacketReqData(unsigned char *data, int szData)
{
#ifdef DEBUG_LORA
  debugSerial.println(F("ReqData, sendPacketThingSpeak= "));
#endif // DEBUG_LORA
  
  dht.readSensor(fHumidity, fTemperature, true);
  lora.sendPacketThingSpeak(thingSpeakApiKey, fHumidity, fTemperature, 33, 46, 59, 60, 70, 85);
}

void setup() {
#ifdef DEBUG_LORA
  Serial.begin(9600);  // use serial port for log monitor
  Serial.println(F("*** lora_04_thingspeak ***"));  // app title
#endif // DEBUG_LORA
  
  lora.begin();
  
  // config setting: drequency, group, data-rate, power, wakeup-time
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

  // custom callback
  lora.setCallbackPacketReqData(funcPacketReqData);
}

void loop() {
  lora.run();
  delay(100);
}

