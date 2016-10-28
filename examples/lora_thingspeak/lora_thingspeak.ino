//////////////////////////////////////////////////////
// This sample code is used for Vinduino project base on MOSTLink protocol
//
// Note:
//    If you use Vinduino board, check MOSTLora library folder.
//    in "MostLora.h" header file, you must unmark "#define USE_VINDUINO".
//    Then MOST Link library would work correctly. 
//
//////////////////////////////////////////////////////

#include "MOSTLora.h"
#include "MLPacketComm.h"
#include <DHT.h>

MOSTLora lora;
DHT dht(2, DHT22);
float fTemperature, fHumidity;

const char *thinkSpeakApiKey = "W00UTJRN68Z7HJJN";    // ThingSpeak API-key

void setup() {
#ifdef DEBUG_LORA
  Serial.begin(9600);  // use serial port for log monitor
  Serial.println("--- App: lora_thingspeak ---");  // use serial port
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
    bReadDHT = readSensorDHT(fHumidity, fTemperature);
    delay(1000);
    i++;
  }

  // test to gateway
  const char *strTest = "Hello LORA.";
  lora.sendData(strTest); 
}

void loop() {
  if (lora.available()) {
    int szRece = lora.receData();
    // reply to vinduino.io when receive "REQ_DATA" by MOSTLink-protocol
    if (lora.parsePacket() == CMD_REQ_DATA) {
      readSensorDHT(fHumidity, fTemperature);
      lora.sendPacketThingSpeak(thinkSpeakApiKey, fHumidity, fTemperature, 33, 46, 59, 60, 70, 85);
    }
  }
//  readSensorDHT(fHumidity, fTemperature);
      
  delay(100);
}

boolean readSensorDHT(float &h, float &t)
{
    boolean bRet = false;
    if (dht.read()) {
        h = dht.readHumidity();
        t = dht.readTemperature();    
    }
    else {
      h = 0;
      t = 0;
    }
    if (h > 0)
      bRet = true;
#ifdef DEBUG_LORA
    if (bRet) {
        Serial.print("Humidity: "); 
        Serial.print(h);
        Serial.print(" %\t");
        Serial.print("Temperature: "); 
        Serial.print(t);
        Serial.println(" *C");      
    }
    else {
        Serial.println("DHT Read Fail.");    
    }
#endif // DEBUG_LORA
    return bRet;
}
