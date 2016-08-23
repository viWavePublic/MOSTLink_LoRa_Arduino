/* viWave LoRa Module by GlobalSat

	MOST-Link protocol
	
	
 */

#include "MOSTLora.h"

#ifdef USE_ARDUINO_UNO         // for arduino Uno
#include <SoftwareSerial.h>
SoftwareSerial mySerial(5, 4); // RX, TX
#else // USE_ARDUINO_UNO
#define mySerial Serial1       // for LinkIt ONE
#endif // USE_ARDUINO_UNO

const int pinP1 = 13;
const int pinP2 = 12;
const int pinLedIO = 11;
const int pinBZ = A2;
 
MOSTLora::MOSTLora()
{

}

void MOSTLora::begin()
{
  Serial.println("=== MOSTLink LoRa v1.0 ===");
  
  pinMode(pinP1, OUTPUT);
  pinMode(pinP2, OUTPUT);

  pinMode(pinLedIO, OUTPUT);
  pinMode(pinBZ, INPUT);

  mySerial.begin(9600);

  readConfig(); 
}

// setup(1,1), normal(0,0), wakeup(0,1), powersaving(1,0)
void MOSTLora::setMode(int p1, int p2)
{
  digitalWrite(pinP1, p1);  // setup(1,1), normal(0,0)
  digitalWrite(pinP2, p2);   
}

boolean MOSTLora::available()
{
  return mySerial.available();
}

void MOSTLora::printBinary(const byte *data, const int szData)
{
  int i;
  for (i = 0; i < szData; i++) {
    if (data[i] < 16)
      Serial.print("0"); 
    
    Serial.print(data[i], HEX); 
  }
  Serial.print(" (");
  Serial.print(szData, DEC);
  Serial.print(" bytes)\n");
}

boolean MOSTLora::printConfig(DataLora &data)
{
  int i;
  boolean bRet = false;
  if (data.tagBegin != 0x24 || data.tagEnd != 0x21) {
    
  }
  bRet = true;
  Serial.print("*** Module:");
  Serial.write(data.module_no, 4);

  Serial.print(", Version:");
  Serial.write(data.ver_no, 7);

  Serial.print(", MAC:");
  for (i = 0; i < 8; i++) {
    Serial.print(data.mac_addr[i], HEX);
  }

  Serial.print("\n    group:");
  Serial.print(data.group_id, DEC);

  long nFrequency;
  nFrequency = ((long)data.freq[0] << 16) + ((long)data.freq[1] << 8) + data.freq[2];
  Serial.print(", frequency:");
  Serial.print(nFrequency);
  
  Serial.print(", data rate:");
  Serial.print(data.data_rate, DEC);
  
  Serial.print(", power:");
  Serial.print(data.power, DEC);

  Serial.print(", \n    uart baud:");
  Serial.print(data.uart_baud, DEC);
  Serial.print(", uart check:");
  Serial.print(data.uart_check, DEC);
  
  Serial.print(", wakeup time:");
  Serial.print(data.wakeup_time, DEC);

  char *pData = (char*)&data;
  Serial.println("\n");
  return bRet;
}

boolean MOSTLora::setHostMAC(char *strMac)
{
  if (strMac == NULL || strlen(strMac) != 16)
    return false;

  boolean bRet = true;
  char strHex[16];
  int i;
  for (i = 0; i < 16; i++) {
    if (!isHexadecimalDigit(strMac[i])) {
      bRet = false;
      break;
    }
    strHex[i] = toupper(strMac[i]);
  }
  if (bRet) {
    char strVal[4] = {0};
    for (i = 0; i < 8; i++) {
      strVal[0] = strHex[i * 2];
      strVal[1] = strHex[i * 2 + 1];
      
      _macHost[i] = strtoul (strVal, NULL, 16);
    }
  }
  
  return bRet;
}

/////////////////////////
// config setting 
/////////////////////////
void MOSTLora::readConfig()
{
  waitUntilReady(5000);
  setMode(1, 1);    // setup(1,1), normal(0,0)
  delay(100);
  
  uint8_t cmdRead[] = {0xFF,0x4C,0xCF,0x52,0xA1,0x52,0xF0};
  sendData(cmdRead, 7);

  // receive setting
  delay(100);
  receConfig(_data);
}

void MOSTLora::writeConfig(long freq, unsigned char group_id, char data_rate, char power, char wakeup_time)
{
  waitUntilReady(5000);
  setMode(1, 1);    // setup(1,1), normal(0,0)
  delay(100);
  
  byte cmdWrite[16] = {0xFF,0x4C,0xCF,0x52,0xA1,0x57,0xF1};
  cmdWrite[7] = (freq >> 16) & 0xff;
  cmdWrite[8] = (freq >> 8) & 0xff;
  cmdWrite[9] = freq & 0xff;
  cmdWrite[10] = group_id;
  cmdWrite[11] = data_rate;
  cmdWrite[12] = power;
  cmdWrite[13] = 3;               // uart baud
  cmdWrite[14] = 0;               // uart check
  cmdWrite[15] = wakeup_time;     // wakeup time
  
  sendData(cmdWrite, 16);
  
  // receive setting
  delay(100);
  receConfig(_data);  
}

boolean MOSTLora::receConfig(DataLora &data)
{
  boolean bRet = false;
  int szData = sizeof(DataLora);
  int szRece = receData((uint8_t*)&data, szData);
  if (szData == szRece) {
    bRet = true;
    printConfig(data);
  }
  else {
    Serial.println("------ Fail to get config!"); 
  }
  return bRet;
}

int MOSTLora::sendData(char *strData)
{
  int nRet = mySerial.print(strData);
  Serial.print(nRet);
  Serial.print(") Send String: ");
  Serial.println(strData);
  return nRet;
}

int MOSTLora::sendData(byte *data, int szData)
{
  int nRet = mySerial.write(data, szData);
  Serial.print(nRet);
  Serial.print(") Send: ");
  printBinary(data, szData);
  return nRet;
}

int MOSTLora::receData(byte *data, int szData)
{
  int nCountBuf = 0;
  while (mySerial.available() && (nCountBuf < szData)) {
    if (0 == nCountBuf) {
      digitalWrite(pinLedIO, HIGH);   // turn the LED on (HIGH is the voltage level)
    }
   
    int c = mySerial.read();
    data[nCountBuf] = c;

    nCountBuf++;
    delay(1);
  }
  if (nCountBuf > 0) {
    digitalWrite(pinLedIO, LOW);    // turn the LED off by making the voltage LOW

    data[nCountBuf] = 0;
    Serial.print("Rece: ");
    printBinary(data, nCountBuf);
    Serial.println((char*)data);
  }
  // parse downlink packet
  parsePacket(data, szData);
  return nCountBuf;
}

int MOSTLora::parsePacket(byte *data, int szData)
{
  if (data[0] == '$') {
    
  }

  
}

boolean MOSTLora::isBusy()
{
  int nBusy = analogRead(pinBZ);
/*  Serial.print("busy:");
  Serial.print(nBusy, DEC);
  Serial.println(".");
  */
  return (nBusy < 512);
}

boolean MOSTLora::waitUntilReady(unsigned long timeout)
{
  boolean bRet = true;
  unsigned long tsStart = millis();
  while (isBusy()) {
    delay(200);
    Serial.print("...!");
    if (timeout < millis() - tsStart) {
      bRet = false;
      break;
    }
  }
  return bRet;
}
