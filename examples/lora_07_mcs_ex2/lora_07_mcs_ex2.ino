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

#if defined(__LINKIT_ONE__)
#include <LEEPROM.h>
#else // __LINKIT_ONE__
#include <EEPROM.h>
#include <MemoryFree.h>

#endif // __LINKIT_ONE__

MOSTLora lora;
DHT dht(2, DHT22);      // DHT22, DHT11
//DHT dht(2, DHT11);       

#define PIN_LED_CONTROL  13
#define PIN_FAN_CONTROL  8
#define PIN_PUMP_CONTROL 12

int szBuf = 0;
byte buf[100] = {0};
char *strTmp = (char*)buf;
float fTemperature, fHumidity;

#define TAG_SO    0x1235
struct DataSo {
  int nTag;     // 0x1235: EEPROM stored correct value
  int soHot;
  int soWet;
  int timePump;   // pump active duration (unit: second)
};
DataSo dataSo;

unsigned long tsSensor = millis();
unsigned long tsStartPump = 0;

// TODO: fill your own deviceKey, deviceID from MCS
const char *strDevice = "DEyyEG5K,Y9LO0iRtIq3FFd6L";  // usgmostlink

const char *strCtrlFan = "CTRL_FAN";
const char *strCtrlLed = "CTRL_LED";
const char *strCtrlPump = "CTRL_PUMP";
const char *strCtrlUpdate = "CTRL_UPDATE";        // refresh sensor
const char *strCtrlSoHot = "CTRL_SO_HOT";   // so hot degree
const char *strCtrlSoWet = "CTRL_SO_WET";   // so wet
const char *strCtrlPumpTime = "CTRL_PUMP_TIME";
const char *strDispTemperature = "DISP_TEMPERATURE";
const char *strDispHumidity = "DISP_HUMIDITY";
const char *strDispLog = "DISP_LOG";

#define START_POS_EEPROM  0
// for EEPROM.put(START_POS_EEPROM, dataSo);
void putEEPROM(uint8_t *pSrc, int szData)
{
  int i;
  for (i = 0; i < szData; i++) {
    EEPROM.write(START_POS_EEPROM + i, pSrc[i]);
  }
}

//  EEPROM.get(START_POS_EEPROM, dataSo);
void getEEPROM(uint8_t *pDst, int szData)
{
  int i;
  for (i = 0; i < szData; i++) {
    pDst[i] = EEPROM.read(START_POS_EEPROM + i);
  }
}

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

    sendUplinkDHT();
    refreshControlState(); 
  }
  else if (parseDownlink(strCtrlSoHot, nVal)) {
    dataSo.soHot = nVal;
    // write EEPROM
    putEEPROM((uint8_t*)&dataSo, sizeof(DataSo));    
    sprintf(strTmp, "(Hot)T > %d", nVal);
    strValue = strTmp;
  }
  else if (parseDownlink(strCtrlSoWet, nVal)) {
    dataSo.soWet = nVal;
    // write EEPROM
    putEEPROM((uint8_t*)&dataSo, sizeof(DataSo));
    sprintf(strTmp, "(Wet)H > %d", nVal);    
    strValue = strTmp;
  }
  else if (parseDownlink(strCtrlPumpTime, nVal)) {
    debugSerial.println("get pump time");
    dataSo.timePump = nVal;
    // write EEPROM
    putEEPROM((uint8_t*)&dataSo, sizeof(DataSo));
    sprintf(strTmp, "Pump time: %d sec", nVal);
    strValue = strTmp;
  }
  else if (parseDownlink(strCtrlPump, nVal)) {
    digitalWrite(PIN_PUMP_CONTROL, nVal);
    if (1 == nVal) {
      tsStartPump = millis();
    }
  }  

  // send log to MCS (as LoRa ack)
  if (strValue.length() > 0) {
    // add timestamp
    unsigned long tsCurr = millis();
    sprintf(strTmp, " (@%d)", tsCurr);
    strValue += strTmp;
    sendUplink(strDispLog, strValue.c_str());
    debugSerial.println(strValue.c_str());    
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
//  debugSerial.print(strToken);
//  debugSerial.println(bRet);
  
  return bRet;
}

//
void setup() {
#ifdef DEBUG_LORA
  Serial.begin(9600);  // use serial port for log monitor
  Serial.println(F("*** lora_07_mcs_ex2 ***"));
#endif // DEBUG_LORA

  // read EEPROM
  getEEPROM((uint8_t*)&dataSo, sizeof(DataSo));
  if (dataSo.nTag != TAG_SO) {
    debugSerial.println(F("=== init so hot/wet!"));
    dataSo.nTag = TAG_SO;
    dataSo.soHot = 30;
    dataSo.soWet = 65;
    dataSo.timePump = 3;
    // write EEPROM
    putEEPROM((uint8_t*)&dataSo, sizeof(DataSo));
  }
  else {
    debugSerial.print(F("=== load: hot>"));
    debugSerial.print(dataSo.soHot);
    debugSerial.print(F(", wet>"));
    debugSerial.print(dataSo.soWet);    
    debugSerial.print(F(", pump sec:"));
    debugSerial.println(dataSo.timePump);      
  }

  pinMode(PIN_LED_CONTROL, OUTPUT);
  pinMode(PIN_FAN_CONTROL, OUTPUT);
  pinMode(PIN_PUMP_CONTROL, OUTPUT);
  digitalWrite(PIN_LED_CONTROL, LOW);
  digitalWrite(PIN_FAN_CONTROL, HIGH);
  digitalWrite(PIN_PUMP_CONTROL, LOW);
    
  lora.begin();
  // custom LoRa config by your environment setting
  // config setting: frequency, group, data-rate, power, wakeup-time
  lora.writeConfig(915222, 0, 0, 7, 5);
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

// send uplink command to MCS
void sendUplink(const char *strID, const char *strValue)
{
  String strCmd = strDevice;
  strCmd += ",";
  strCmd += MLutility::generateChannelData(strID, strValue);
  
  lora.sendPacketSendMCSCommand((uint8_t*)strCmd.c_str(), strCmd.length());
  delay(500);
}

void refreshControlState() {
  String strCmd = strDevice;
  strCmd += ",";
  strCmd += MLutility::generateChannelData(strCtrlSoHot, dataSo.soHot);
  strCmd += "\n";
  strCmd += MLutility::generateChannelData(strCtrlSoWet, dataSo.soWet);

  lora.sendPacketSendMCSCommand((uint8_t*)strCmd.c_str(), strCmd.length());
  delay(800);

  strCmd = strDevice;
  strCmd += ",";
  strCmd += MLutility::generateChannelData(strCtrlFan, digitalRead(PIN_FAN_CONTROL));
  strCmd += "\n";
  strCmd += MLutility::generateChannelData(strCtrlLed, digitalRead(PIN_LED_CONTROL));
  strCmd += "\n";
  strCmd += MLutility::generateChannelData(strCtrlUpdate, "0");
  
  lora.sendPacketSendMCSCommand((uint8_t*)strCmd.c_str(), strCmd.length());
  delay(800);

  // for pump
  strCmd = strDevice;
  strCmd += ",";
  strCmd += MLutility::generateChannelData(strCtrlPump, digitalRead(PIN_PUMP_CONTROL));
  strCmd += "\n";
  strCmd += MLutility::generateChannelData(strCtrlPumpTime, dataSo.timePump);
  
  lora.sendPacketSendMCSCommand((uint8_t*)strCmd.c_str(), strCmd.length());
  delay(800);
}

void sendUplinkDHT() {
  String strCmd = strDevice;
  strCmd += ",";
  strCmd += MLutility::generateChannelData(strDispHumidity, fHumidity);
  strCmd += "\n";
  strCmd += MLutility::generateChannelData(strDispTemperature, fTemperature);

  lora.sendPacketSendMCSCommand((uint8_t*)strCmd.c_str(), strCmd.length());
  delay(600);

  Serial.print(strCmd.length());
  Serial.println(F(" count chars"));
}

void loop() {
  lora.run();
  delay(100);

  unsigned long tsCurr = millis();
  if (tsCurr > tsSensor + 5000) {
    tsSensor = tsCurr;
    dht.readSensor(fHumidity, fTemperature, true);
    if (fTemperature > dataSo.soHot && 0 == digitalRead(PIN_FAN_CONTROL)) {
      digitalWrite(PIN_FAN_CONTROL, 1);
      sendUplink(strCtrlFan, "1");
      sendUplinkDHT();      
      debugSerial.print(F("temperature > "));
      debugSerial.print(dataSo.soHot);
      debugSerial.println(F(", active fan."));
    }
    if (fHumidity > dataSo.soWet && 0 == digitalRead(PIN_LED_CONTROL)) {
      digitalWrite(PIN_LED_CONTROL, 1);
      sendUplinkDHT();      
      sendUplink(strCtrlLed, "1");
      debugSerial.print(F("humidity > "));
      debugSerial.print(dataSo.soWet);
      debugSerial.println(F(", active led"));
    }
    Serial.print(F("timestamp: "));
    Serial.println(tsCurr);
  }
  if (tsStartPump > 0 && tsCurr - tsStartPump > dataSo.timePump * 1000)
  {
    tsStartPump = 0;
    digitalWrite(PIN_PUMP_CONTROL, 0);
    sendUplink(strCtrlPump, "0");
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
        sendUplinkDHT();
      }
      else if (buf[1] == '3') {
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


