//////////////////////////////////////////////////////
// ShareCourse on MOSTLink protocol with MCS
// 
// Green House system by LoRa and MOSTLink
// 1. DHT22 sensor for humidity and temperature
// 2. Led for control humidity
// 3. Fan for cool down
//
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

#define PIN_SENSOR_LIGHT  A0
#define PIN_LIT_LED       3

int szBuf = 0;
byte buf[100] = {0};
char strTmp[32] = {0};
float fTemperature, fHumidity;
unsigned long tsSensor = millis();

const char *strDevice = "D4AKFZ6S,6LTy6N1OkRlhDfk4";    // LoRa Node #1
const char *strCtrlLed = "CTRL_LED";
const char *strCtrlUpdate = "CTRL_UPDATE";        // refresh sensor
const char *strDispTemperature = "DISP_TEMPERATURE";
const char *strDispHumidity = "DISP_HUMIDITY";
const char *strDispLog = "DISP_LOG";
const char *strDispReed = "DISP_REED";
const char *strDispLight = "DISP_LIGHT";

// callback for rece data
void funcPacketReqData(unsigned char *data, int szData)
{
  memcpy(buf, data, szData);  
  buf[szData] = 0;
  debugSerial.print(F("ReqData= "));
  debugSerial.println((const char*)buf);
}

void funcPacketNotifyMcsCommand(unsigned char *data, int szData)
{
  memcpy(buf, data, szData);  
  buf[szData] = 0;
  debugSerial.print(F("NotifyMcsCommand= "));
  debugSerial.println((const char*)buf);

  int nVal = 0;
  String strValue;
  const char *strCtrl = NULL;
  if (parseDownlink(strCtrlLed, nVal)) {
    digitalWrite(PIN_LED_CONTROL, nVal);
    strValue = strCtrlLed;
    if (0 == nVal)
      strValue += " off";
    else
      strValue += " on";    
  }
  else if (parseDownlink(strCtrlUpdate, nVal)) {
    dht.readSensor(fHumidity, fTemperature, true);

    sendUplink(strDispHumidity, fHumidity);
    sendUplink(strDispTemperature, fTemperature);
    refreshControlState();
  }

  // send log to MCS (as LoRa ack)
  if (strValue.length() > 0) {
    // add timestamp
    unsigned long tsCurr = millis();
    sprintf(strTmp, " (@%d)", tsCurr);
    strValue += strTmp;
    sendUplink(strDispLog, strValue.c_str());    
  }
}
// parse downlink command from MCS
boolean parseDownlink(const char *strToken, int &nVal) {
  boolean bRet = false;
  const char *strBuf = (const char *)buf;
  if (strstr(strBuf, strToken) == strBuf) {
    const char *strVal = strBuf + strlen(strToken) + 1;
    nVal = atoi(strVal);
    bRet = true;
  }
  return bRet;
}
// send uplink command to MCS
void sendUplink(const char *strID, const char *strValue)
{
  String strCmd = strDevice;
  strCmd += ",";
  strCmd += strID;
  strCmd += ",,";
  strCmd += strValue;
  
  lora.sendPacketSendMCSCommand((uint8_t*)strCmd.c_str(), strCmd.length());
  delay(500);
}
void sendUplink(const char *strID, int nVal) {
  sprintf(strTmp, "%d", nVal);
  sendUplink(strID, strTmp);
}
void sendUplink(const char *strID, float fVal) {
  dtostrf(fVal, 1, 1, strTmp);
//  sprintf(strTmp, "%3.1f", fVal);
  sendUplink(strID, strTmp);
}
//
void setup() {
  Serial.begin(9600);  // use serial port for log monitor

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
  lora.writeConfig(915555, 0, 0, 7, 5);
  lora.setMode(E_LORA_POWERSAVING);         // E_LORA_POWERSAVING

  delay(1000);

  // custom callback
  lora.setCallbackPacketReqData(funcPacketReqData);
  lora.setCallbackPacketNotifyMcsCommand(funcPacketNotifyMcsCommand);

  // init sensor for humidity & temperature
  dht.begin();
  int i = 0;
  boolean bReadDHT = false;
  while (!bReadDHT && i < 8) {
    delay(700);
    bReadDHT = dht.readSensor(fHumidity, fTemperature, true);
    i++;
  }

  // login MCS
  lora.sendPacketReqLoginMCS((uint8_t*)strDevice, strlen(strDevice));

  // init MCS control state:
  refreshControlState();
}

void refreshControlState() {
  sendUplink(strCtrlLed, digitalRead(PIN_LED_CONTROL));
  
  int nReedState = digitalRead(PIN_SENSOR_REED);
  digitalWrite(PIN_REED_LED, nReedState);
  sendUplink(strDispReed, nReedState);
  
  int nLit = analogRead(PIN_SENSOR_LIGHT);
  sendUplink(strDispLight, nLit);

  sendUplink(strCtrlUpdate, "0");
}

void loop() {
  lora.run();
  delay(100);

  int nReedState = digitalRead(PIN_SENSOR_REED);
  digitalWrite(PIN_REED_LED, nReedState);
    
  unsigned long tsCurr = millis();
  if (tsCurr > tsSensor + 5000) {
    tsSensor = tsCurr;
    dht.readSensor(fHumidity, fTemperature, true);

    int nLit = analogRead(PIN_SENSOR_LIGHT);    
    Serial.print(F("Lit: "));
    Serial.println(nLit);

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
        lora.sendPacketReqLoginMCS((uint8_t*)strDevice, strlen(strDevice));
      }
      else if (buf[1] == '2') {
        sendUplink(strDispTemperature, fTemperature);
      }
      else if (buf[1] == '3') {
        sendUplink(strDispHumidity, fHumidity);
      }
      else if (buf[1] == '4') {
        refreshControlState();
      }
      else if (buf[1] == 's') {
        dht.readSensor(fHumidity, fTemperature, true);
      }
      else if (buf[1] == '9') {
        lora.sendPacketSendMCSCommand((buf + 2), strlen((char*)buf + 2));
      }
    }
  }  
}
