//////////////////////////////////////////////////////
// ThingSpeak project base on MOSTLink protocol
// upload sensors to cloud server
// https://thingspeak.com/login
//
// DHT dht(2, DHT22);         // for DHT22
// DHT dht(2, DHT11);         // for DHT11
//////////////////////////////////////////////////////

#include "MOSTLora.h"

MOSTLora lora;

float fTemperature, fHumidity;
const char *thingSpeakApiKey = "W00UTJRN68Z7HJJN";    // ThingSpeak API-key

/////////////////////////////////////////////
// DHT sensor related:
#if defined(__LINKIT_ONE__)
#include "LDHT.h"
LDHT dht(2, DHT22);      // DHT22, DHT11
#else     // __LINKIT_ONE__
#include "DHT.h"
DHT dht(2, DHT22);      // DHT22, DHT11
#endif    // __LINKIT_ONE__
/////////////////////////////////////////////
// read DHT sensor: Temperature and Humidity
bool readSensorDHT(float &fHumi, float &fTemp, bool bShowResult)
{
    bool bRet = true;
#if defined(__LINKIT_ONE__)  
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    if(dht.read())
    {
        fTemp = dht.readTemperature();
        fHumi = dht.readHumidity();
    }
#else     // __LINKIT_ONE__
    bRet = dht.readSensor(fHumi, fTemp);
#endif    // __LINKIT_ONE__

    if (bShowResult) {
        debugSerial.print(F("ts("));
        debugSerial.print(millis());
        debugSerial.print(F("), "));
        if (bRet) {
            debugSerial.print(F("Humidity: "));
            debugSerial.print(fHumi);
            debugSerial.print(F(" %, Temperature: "));
            debugSerial.print(fTemp);
            debugSerial.println(F(" *C"));
        }
        else {
            debugSerial.println(F("DHT Fail."));
        }
    }
    return bRet;
}
/////////////////////////////////////////////
// callback for REQ_DATA
void funcPacketReqData(unsigned char *data, int szData)
{
#ifdef DEBUG_LORA
  debugSerial.println(F("ReqData, sendPacketThingSpeak= "));
#endif // DEBUG_LORA

  readSensorDHT(fHumidity, fTemperature, true);
  lora.sendPacketThingSpeak(thingSpeakApiKey, fHumidity, fTemperature, 33, 46, 59, 60, 70, 85);
}

void setup() {
  debugSerial.begin(9600);  // use serial port for log monitor
  debugSerial.println(F("*** lora_04_thingspeak ***"));  // app title
  
  lora.begin();
  
  // config setting: frequency, group, data-rate, power, wakeup-time
  lora.writeConfig(915000, 0, 0, 7, 5);
  lora.setMode(E_LORA_WAKEUP);

  delay(1000);
  
  // init sensor for humidity & temperature
  dht.begin();
  int i = 0;
  boolean bReadDHT = false;
  while (!bReadDHT && i < 8) {
    bReadDHT = readSensorDHT(fHumidity, fTemperature, true);
    delay(600);
    i++;
  }

  // custom callback
  lora.setCallbackPacketReqData(funcPacketReqData);
}

void loop() {
  lora.run();
  delay(100);
}

