//////////////////////////////////////////////////////
// MOSTLink LoRa Arduino sample code
//
// Gateway request data to node, then node response it's 
// temperature and humidity to gateway
//
// DHT dht(2, DHT22);         // for DHT22
// DHT dht(2, DHT11);         // for DHT11
//////////////////////////////////////////////////////

#include "MOSTLora.h"
#include "DHT.h"

MOSTLora lora(7, 6, 5);     // LM-130 (new)
DHT dht(2, DHT22);          // DHT22

float fTemperature, fHumidity;

// callback for CMD_REQ_DATA
void funcPacketReqData(unsigned char *data, int szData)
{
  debugSerial.print(F("ReqData, answer= "));

  dht.readSensor(fHumidity, fTemperature, true);
  lora.sendPacketResData(fHumidity, fTemperature);
}

void setup() {
  // put your setup code here, to run once:
  // Serial for log monitor
  Serial.begin(9600);
  Serial.println(F("*** lora_02_DHT ***"));
  
  lora.begin(); 
  
  // config setting: frequency, group, data-rate, power, wakeup-time
  lora.writeConfig(915000, 0, 0, 7, 5);
  lora.setMode(E_LORA_POWERSAVING);
  
  // set polling request callback
  lora.setCallbackPacketReqData(funcPacketReqData);

  // init sensor for humidity & temperature
  dht.begin();
  int i = 0;
  boolean bReadDHT = false;
  while (!bReadDHT && i < 5) {
    bReadDHT = dht.readSensor(fHumidity, fTemperature, true);
    delay(1000);
    i++;
  }

  lora.sendPacketResData(fHumidity, fTemperature);
}

void loop() {
  lora.run();
  delay(100);
}

