//////////////////////////////////////////////////////
// Starter on MOSTLink protocol with myDevices Cayenne
// 
// Sensor connect to these pins on Arduino UNO:
//     1. humidity-temperature sensor (D2)
//
// myDevices Cayenne
//     1. sign in http://mydevices.com/cayenne/signin/
//     2. [Add New...]->[Device/Widget]->CAYENNE API(Bring Your Own Thing)
//     3. copy/paste: MQTT USERNAME, PASSWORD, CLIENT ID
//     4. Vertify and Upload this code to Arduino UNO
//
// Command in Serial monitor
//     "/1" relogin myDevices Cayenne
//     "/2" send uplink DHT to myDevices
//////////////////////////////////////////////////////

#include "MOSTLora.h"
#include "DHT.h"
#include "MLutility.h"

MOSTLora lora;
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

int szBuf = 0;
byte buf[100] = {0};
char strTmp[32] = {0};
float fTemperature, fHumidity;
unsigned long tsSensor = millis();

// ****** NOTICE ******
// TODO: Before you run your application, you need a myDevices Cayenne account.
const char *MQTTUsername = "66d320a0-c737-11e6-aeea-ffda3f4916c6";
const char *MQTTPassword = "84b09083b226ae4eaa2adf559350bd2ab56b98f6";
const char *ClientID =     "ffa54f30-d1ad-11e6-b089-9f6bfa78ab33";

/////////////////////////////////////////////
// callback for CMD_REQ_DATA
void funcPacketReqData(unsigned char *data, int szData)
{
  MLutility::blinkSOS(50);
  memcpy(buf, data, szData);  
  buf[szData] = 0;
  debugSerial.print(F("ReqData= "));
  debugSerial.println((const char*)buf);

  sendUplinkDHT();
}

/////////////////////////////////////////////
void setup() {

  debugSerial.begin(9600);  // use serial port for log monitor
  debugSerial.println(F("*** lora_08_mydevices_starter ***"));

   
  lora.begin();
  // custom LoRa config by your environment setting
  // config setting: frequency, group, data-rate, power, wakeup-time
  lora.writeConfig(915000, 0, 0, 7, 5);
  lora.setMode(E_LORA_POWERSAVING);         // E_LORA_POWERSAVING

  delay(1000);

  // custom callback
  lora.setCallbackPacketReqData(funcPacketReqData);

  // init sensor for humidity & temperature
  dht.begin();
  int i = 0;
  boolean bReadDHT = false;
  while (!bReadDHT && i < 8) {
    delay(700);
    bReadDHT = dht.readSensor(fHumidity, fTemperature);
    i++;
  }

  // login myDevices
  lora.sendPacketReqLoginMydevices(MQTTUsername, MQTTPassword, ClientID);
}

/////////////////////////////////////////////

void sendUplinkDHT() {
  readSensorDHT(fHumidity, fTemperature, true);

  String strCmd = "2,temp,f,";
  int nVal = dht.convertCtoF(fTemperature);
  itoa(nVal, strTmp, 10);
  strCmd += strTmp;
  lora.sendPacketSendMydevicesCommand((uint8_t*)strCmd.c_str(), strCmd.length());
  delay(500);
  
  strCmd = "3,rel_hum,p,";
  itoa((int)fHumidity, strTmp, 10);
  strCmd += strTmp;
  lora.sendPacketSendMydevicesCommand((uint8_t*)strCmd.c_str(), strCmd.length());
  delay(500);
}

/////////////////////////////////////////////

void loop() {
  lora.run();
  delay(100);
   
  unsigned long tsCurr = millis();
  if (tsCurr > tsSensor + 5000) {
    tsSensor = tsCurr;
    readSensorDHT(fHumidity, fTemperature, true);
    fTemperature = dht.convertCtoF(fTemperature);

    debugSerial.print(fTemperature);
    debugSerial.println(F(" *F"));
  }

  // command to send (for debug)
  if (debugSerial.available())
    inputBySerial();
}

void inputBySerial()
{
  int countBuf = MLutility::readSerial((char*)buf);
  if (countBuf > 1) {
    buf[countBuf] = 0;
    if (buf[0] == '/')
    {
      if (buf[1] == '1') {
        // login myDevices    
        lora.sendPacketReqLoginMydevices(MQTTUsername, MQTTPassword, ClientID);
      }
      else if (buf[1] == '2') {
        sendUplinkDHT();        // test uplink: 
      }
      else if (buf[1] == '3') {
      }
      else if (buf[1] == '4') {
      }
      else if (buf[1] == 's') {
      }
    }
  }  
}
