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
DHT dht(2, DHT22);      // DHT22, DHT11
//DHT dht(2, DHT11);       

#define PIN_LED_CONTROL  13
#define PIN_FAN_CONTROL  8

int szBuf = 0;
byte buf[100] = {0};
char strTmp[32] = {0};
float fTemperature, fHumidity;
int soHot = 40, soWet = 60;
unsigned long tsSensor = millis();

//const char *strDevice = "DF4dQDv7,48MRQ7vjehmKHSm0";  // LoRa Node #2
const char *strDevice = "DyAwB8Ed,gWAVV00hHXAZ0lhv";    // LoRa Node #1
const char *strCtrlFan = "CTRL_FAN";
const char *strCtrlLed = "CTRL_LED";
const char *strCtrlUpdate = "CTRL_UPDATE";        // refresh sensor
const char *strCtrlSoHot = "CTRL_SO_HOT";   // so hot degree
const char *strCtrlSoWet = "CTRL_SO_WET";   // so wet
const char *strDispTemperature = "DISP_TEMPERATURE";
const char *strDispHumidity = "DISP_HUMIDITY";
const char *strDispLog = "DISP_LOG";

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
  if (parseDownlink(strCtrlFan, nVal)) {
    digitalWrite(PIN_FAN_CONTROL, nVal);
    strValue = strCtrlFan;
    if (0 == nVal)
      strValue += " off";
    else
      strValue += " on";
  }
  else if (parseDownlink(strCtrlLed, nVal)) {
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
  else if (parseDownlink(strCtrlSoHot, nVal)) {
    soHot = nVal;
    sprintf(strTmp, "(Hot)T > %d", nVal);
    strValue = strTmp;
  }
  else if (parseDownlink(strCtrlSoWet, nVal)) {
    soWet = nVal;
    sprintf(strTmp, "(Wet)H > %d", nVal);    
    strValue = strTmp;
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

  pinMode(PIN_LED_CONTROL, OUTPUT);
  pinMode(PIN_FAN_CONTROL, OUTPUT);
  digitalWrite(PIN_LED_CONTROL, LOW);
  digitalWrite(PIN_FAN_CONTROL, HIGH);
    
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
  sendUplink(strCtrlFan, digitalRead(PIN_FAN_CONTROL));
  sendUplink(strCtrlLed, digitalRead(PIN_LED_CONTROL));
  sendUplink(strCtrlSoHot, soHot);
  sendUplink(strCtrlSoWet, soWet);
  sendUplink(strCtrlUpdate, "0");
}

void loop() {
  lora.run();
  delay(100);

  unsigned long tsCurr = millis();
  if (tsCurr > tsSensor + 5000) {
    tsSensor = tsCurr;
    dht.readSensor(fHumidity, fTemperature, true);
    if (fTemperature > soHot && 0 == digitalRead(PIN_FAN_CONTROL)) {
      digitalWrite(PIN_FAN_CONTROL, 1);
      sendUplink(strCtrlFan, "1");
      debugSerial.print(F("temperature > "));
      debugSerial.print(soHot);
      debugSerial.println(F(", active fan."));
    }
    if (fHumidity > soWet && 0 == digitalRead(PIN_LED_CONTROL)) {
      digitalWrite(PIN_LED_CONTROL, 1);
      sendUplink(strCtrlLed, "1");
      debugSerial.print(F("humidity > "));
      debugSerial.print(soWet);
      debugSerial.println(F(", active led"));
    }
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
