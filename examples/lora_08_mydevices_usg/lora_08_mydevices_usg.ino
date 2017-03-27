//////////////////////////////////////////////////////
// USG(CES) on MOSTLink protocol with myDevices Cayenne
// 
// Sensor connect to these pins on Arduino UNO:
//     1. blue-led(D3): show light-sensor(A0) value
//     2. red-led(D13): show reed-sensor(D12) state
//     3. green-led(D8): control by MCS
//     4. humidity-temperature sensor (D2)
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
//     "/4" send uplink control-status to myDevices
//////////////////////////////////////////////////////

#include "MOSTLora.h"
#include "DHT.h"
#include "MLutility.h"

#if !defined(__LINKIT_ONE__)
#include <MemoryFree.h>
#endif // __LINKIT_ONE__

MOSTLora lora;
DHT dht(2, DHT11);       

#define PIN_LED_CONTROL   8

#define PIN_SENSOR_REED   12
#define PIN_REED_LED      13

#define ID_UPDATE         15

#define PIN_SENSOR_LIGHT  A0
#define PIN_LIT_LED       3

int szBuf = 0;
byte buf[100] = {0};
char strTmp[32] = {0};
float fTemperature, fHumidity;
unsigned long tsSensor = millis();

// ****** NOTICE ******
// TODO: Before you run your application, you need a myDevices Cayenne account.
const char *MQTTUsername = "b1ccd4a0-d30f-11e6-97cc-8758d0339dd8";
const char *MQTTPassword = "dd183e02322d645286cb9076bf76cfb2f2a094c4";
const char *ClientID =     "c4cb8060-d30f-11e6-ae8c-f90e83be502a";

// callback for rece packet: notify myDevices Command
void funcPacketNotifyMydevicesCommand(unsigned char *data, int szData)
{
  memcpy(buf, data, szData);  
  buf[szData] = 0;
#ifdef DEBUG_LORA
  debugSerial.print(F("NotifyMydevicesCommand= "));
  debugSerial.println((const char*)buf);
#endif // DEBUG_LORA

  const char *strCommon = strstr(buf, ",");
  int nChannel = 0;
  int nVal;
  if (strCommon != NULL) {
    nChannel = atoi(buf);
    nVal = atoi(strCommon + 1);
#ifdef DEBUG_LORA
    debugSerial.print(F("#"));
    debugSerial.print(nChannel);
    debugSerial.print(F("="));
    debugSerial.println(nVal);
#endif // DEBUG_LORA

    if (nChannel == PIN_LED_CONTROL) {
      digitalWrite(PIN_LED_CONTROL, nVal);
    }
    else if (nChannel == ID_UPDATE) {
      sendUplinkDHT();
      refreshControlState();      
    }
  }
}

//
void setup() {
#ifdef DEBUG_LORA
  debugSerial.begin(9600);  // use serial port for log monitor
#endif // DEBUG_LORA

  pinMode(PIN_SENSOR_REED, INPUT);
  pinMode(PIN_REED_LED, OUTPUT);
  
  pinMode(PIN_SENSOR_LIGHT, INPUT);
  pinMode(PIN_LIT_LED, OUTPUT);
  
  pinMode(PIN_LED_CONTROL, OUTPUT);

  digitalWrite(PIN_LED_CONTROL, HIGH);
  digitalWrite(PIN_REED_LED, HIGH);
  digitalWrite(PIN_LIT_LED, HIGH);
    
  lora.begin();
  // custom LoRa config by your environment setting
  // config setting: frequency, group, data-rate, power, wakeup-time
  lora.writeConfig(915000, 0, 0, 7, 5);
  lora.setMode(E_LORA_POWERSAVING);         // E_LORA_POWERSAVING

  delay(1000);

  // custom callback
  lora.setCallbackPacketNotifyMydevicesCommand(funcPacketNotifyMydevicesCommand);

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

void refreshControlState() {
  String strCmd;
  int nVal = digitalRead(PIN_LED_CONTROL);
  itoa(PIN_LED_CONTROL, strTmp, 10);
  strCmd = strTmp;
  strCmd += ",null,null,";
  itoa(nVal, strTmp, 10);
  strCmd += strTmp;
  lora.sendPacketSendMydevicesCommand((uint8_t*)strCmd.c_str(), strCmd.length());
  delay(500);
  
  nVal = digitalRead(PIN_SENSOR_REED);
  itoa(PIN_SENSOR_REED, strTmp, 10);
  strCmd = strTmp;
  strCmd += ",null,null,";
  itoa(nVal, strTmp, 10);
  strCmd += strTmp;
  lora.sendPacketSendMydevicesCommand((uint8_t*)strCmd.c_str(), strCmd.length());
  delay(500);
  
  int nLit = analogRead(PIN_SENSOR_LIGHT);    
  strCmd = "20,lum,lux,";
  itoa(nLit, strTmp, 10);
  strCmd += strTmp;
  lora.sendPacketSendMydevicesCommand((uint8_t*)strCmd.c_str(), strCmd.length());
  delay(500);

  itoa(ID_UPDATE, strTmp, 10);
  strCmd = strTmp;
  strCmd += ",null,null,0";
  lora.sendPacketSendMydevicesCommand((uint8_t*)strCmd.c_str(), strCmd.length());
  delay(500);  
}

void sendUplinkDHT() {
  dht.readSensor(fHumidity, fTemperature);

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

void loop() {
  lora.run();
  delay(100);

// reed sensor
  int nReedState = digitalRead(PIN_SENSOR_REED);
  digitalWrite(PIN_REED_LED, nReedState);
  
  int nLit = analogRead(PIN_SENSOR_LIGHT);    
  int nLitLed = map(nLit, 0, 1023, 0, 255);   // analog light-sensor convert to blue-led (PWM)
  analogWrite(PIN_LIT_LED, nLitLed);
    
  unsigned long tsCurr = millis();
  if (tsCurr > tsSensor + 5000) {
    tsSensor = tsCurr;
    dht.readSensor(fHumidity, fTemperature);
    fTemperature = dht.convertCtoF(fTemperature);

    Serial.print(fTemperature);
    Serial.print(F(" *F, "));
    Serial.print(F("LightSensor: "));
    Serial.print(nLit);
    Serial.print(F(", BlueLed(PWM): "));
    Serial.println(nLitLed);

    Serial.print(F("timestamp: "));
    Serial.println(tsCurr);
  }

  // command to send (for debug)
  if (Serial.available())
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
        refreshControlState();  // test uplink:
      }
      else if (buf[1] == 's') {
      }
    }
  }  
}
