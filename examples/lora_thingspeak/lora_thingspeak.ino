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
#include "DHT.h"

MOSTLora lora;
DHT dht(2, DHT22);
float fTemperature, fHumidity;

const char *thinkSpeakApiKey = "W00UTJRN68Z7HJJN";    // ThingSpeak API-key

void setup() {
  Serial.begin(9600);  // use serial port for log monitor
  
  lora.begin();
  // custom LoRa config by your environment setting
  lora.writeConfig(915555, 0, 0, 7, 5);
  lora.setMode(E_LORA_WAKEUP);

  readSensorDHT(fHumidity, fTemperature);

  delay(1000);
  // test to vinduino.io
//  lora.sendPacketThingSpeak(thinkSpeakApiKey, 11, 22, 32, 46, 59, 60, 70, 85);
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
  delay(100);
}

boolean readSensorDHT(float &h, float &t)
{
    boolean bRet = false;
    h = dht.readHumidity();
    t = dht.readTemperature();
//dht.readHT(&t, &h)
    // check if returns are valid, if they are NaN (not a number) then something went wrong!
    if (isnan(t) || isnan(h)) 
    {
        Serial.println("Failed to read from DHT");
    } 
    else 
    {
        bRet = true;            
        Serial.print("Humidity: "); 
        Serial.print(h);
        Serial.print(" %\t");
        Serial.print("Temperature: "); 
        Serial.print(t);
        Serial.println(" *C");
    }
    delay(2000);
    return bRet;
}

