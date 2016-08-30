/* viWave LoRa Module by GlobalSat

	MOST-Link protocol
	
	
 */

#include "MOSTLora.h"
#include "MLpacket.h"

#ifdef USE_LINKIT_ONE
    #define loraSerial Serial1       // for LinkIt ONE

#else // USE_LINKIT_ONE
    #ifdef DEBUG_LORA

        // for arduino Uno
        #include <SoftwareSerial.h>
        const int pinLoraRX = 4;//10;
        const int pinLoraTX = 5;//11;
        SoftwareSerial loraSerial(pinLoraRX, pinLoraTX);    // RX, TX
//    pinMode(pinLoraRX, INPUT);
//    pinMode(pinLoraTX, OUTPUT);
    #else // DEBUG_LORA

        #define loraSerial Serial       // for Vinduino

    #endif // DEBUG_LORA

#endif // USE_LINKIT_ONE

#ifdef DEBUG_LORA
#define debugSerial Serial
#else

class DummySerial {
    
};
#define debugSerial DummySerial

#endif

const int pinP1 = 13;
const int pinP2 = 12;
const int pinLedIO = 9;
const int pinBZ = A2;
 
MOSTLora::MOSTLora()
{
    _eMode = E_UNKNOWN_LORA_MODE;
}

void MOSTLora::begin()
{
#ifdef DEBUG_LORA
  debugSerial.println("=== MOSTLink LoRa v1.0 ===");
  debugSerial.print("CPU clock: ");
  debugSerial.println(F_CPU);
#endif // DEBUG_LORA
    
  pinMode(pinP1, OUTPUT);
  pinMode(pinP2, OUTPUT);

  pinMode(pinLedIO, OUTPUT);
  pinMode(pinBZ, INPUT);

  loraSerial.begin(9600);

  readConfig();
    
  setMode(E_LORA_WAKEUP);   // E_LORA_NORMAL
}

void MOSTLora::setMode(int mode)
{
  if (_eMode == mode)
    return;

  _eMode = mode;
  if (E_LORA_NORMAL == _eMode)
    setMode(0, 0);
  else if (E_LORA_WAKEUP == _eMode)
    setMode(0, 1);
  else if (E_LORA_POWERSAVING == _eMode)
    setMode(1, 0);
  else if (E_LORA_SETUP == _eMode)
    setMode(1, 1);
}

// setup(1,1), normal(0,0), wakeup(0,1), powersaving(1,0)
void MOSTLora::setMode(int p1, int p2)
{
  digitalWrite(pinP1, p1);  // setup(1,1), normal(0,0)
  digitalWrite(pinP2, p2);
  delay(200);
}

boolean MOSTLora::available()
{
  return loraSerial.available();
}

void MOSTLora::printBinary(const byte *data, const int szData)
{
#ifdef DEBUG_LORA
  int i;
  for (i = 0; i < szData; i++) {
    if (data[i] < 16)
      debugSerial.print("0"); 
    
    debugSerial.print(data[i], HEX); 
  }
  debugSerial.print(" (");
  debugSerial.print(szData, DEC);
  debugSerial.print(" bytes)\n");
#endif // DEBUG_LORA
}

boolean MOSTLora::printConfig(DataLora &data)
{
  boolean bRet = false;
#ifdef DEBUG_LORA
  int i;
  if (data.tagBegin != 0x24 || data.tagEnd != 0x21) {
    debugSerial.print("++++++ incorrect config");
  }
  bRet = true;
  debugSerial.print("*** Module:");
  debugSerial.write(data.module_no, 4);

  debugSerial.print(", Version:");
  debugSerial.write(data.ver_no, 7);

  debugSerial.print(", MAC:");
  for (i = 0; i < 8; i++) {
    debugSerial.print(data.mac_addr[i], HEX);
  }

  debugSerial.print("\n    group:");
  debugSerial.print(data.group_id, DEC);

  long nFrequency;
  nFrequency = ((long)data.freq[0] << 16) + ((long)data.freq[1] << 8) + data.freq[2];
  debugSerial.print(", frequency:");
  debugSerial.print(nFrequency);
  
  debugSerial.print(", data rate:");
  debugSerial.print(data.data_rate, DEC);
  
  debugSerial.print(", power:");
  debugSerial.print(data.power, DEC);

  debugSerial.print(", \n    uart baud:");
  debugSerial.print(data.uart_baud, DEC);
  debugSerial.print(", uart check:");
  debugSerial.print(data.uart_check, DEC);
  
  debugSerial.print(", wakeup time:");
  debugSerial.print(data.wakeup_time, DEC);

  char *pData = (char*)&data;
  debugSerial.println("\n");
#endif // DEBUG_LORA
  return bRet;
}

boolean MOSTLora::printInfo()
{
    boolean bRet = MOSTLora::printConfig(_data);
    char *strMode = "Unknown Mode";
    switch (_eMode) {
        case E_LORA_NORMAL:
            strMode = "--- Normal Mode ---";
            break;
        case E_LORA_WAKEUP:
            strMode = "--- Wakeup Mode ---";
            break;
        case E_LORA_POWERSAVING:
            strMode = "--- Power Saving Mode ---";
            break;
        case E_LORA_SETUP:
            strMode = "--- Setup Mode ---";
            break;
    }
#ifdef DEBUG_LORA
    debugSerial.println(strMode);
#endif // DEBUG_LORA
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
  setMode(E_LORA_SETUP);    // setup(1,1), normal(0,0)
  
  uint8_t cmdRead[] = {0xFF,0x4C,0xCF,0x52,0xA1,0x52,0xF0};
  sendData(cmdRead, 7);

  // receive setting
  delay(1000);
  receConfig(_data);
}

void MOSTLora::writeConfig(long freq, unsigned char group_id, char data_rate, char power, char wakeup_time)
{
  waitUntilReady(5000);
  setMode(E_LORA_SETUP);    // setup(1,1), normal(0,0)
  
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
  delay(2000);
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
#ifdef DEBUG_LORA
    debugSerial.print(szRece);
    debugSerial.println(") ------ Fail to get config!");
#endif // DEBUG_LORA
  }
  return bRet;
}

int MOSTLora::sendData(char *strData)
{
  int nRet = loraSerial.print(strData);
  delay(100);
#ifdef DEBUG_LORA
  debugSerial.print(nRet);
  debugSerial.print(") Send String: ");
  debugSerial.println(strData);
#endif // DEBUG_LORA
  return nRet;
}

int MOSTLora::sendData(byte *data, int szData)
{
  int nRet = loraSerial.write(data, szData);
  delay(100);
#ifdef DEBUG_LORA
  debugSerial.print(nRet);
  debugSerial.print(") Send: ");
  printBinary(data, szData);
#endif // DEBUG_LORA
  return nRet;
}

int MOSTLora::receData(byte *data, int szData)
{
  int nRssi = 0;
  if (!loraSerial.available())
    return 0;
  int i, nCountBuf = 0;
//  while (loraSerial.available() && (nCountBuf < szData)) {
  for (i = 0; i < 6; i++) {
      int nCharRead = 0;
      while (loraSerial.available() && (nCountBuf < szData)) {
        if (0 == nCountBuf) {
          digitalWrite(pinLedIO, HIGH);   // turn the LED on (HIGH is the voltage level)
        }
       
        int c = loraSerial.read();
        data[nCountBuf] = c;

        nCountBuf++;
        nCharRead++;
        delay(1);
      }

      delay(300);
      if (nCharRead > 0) {
#ifdef DEBUG_LORA
          debugSerial.print(nCharRead);
          debugSerial.print(") ");
#endif // DEBUG_LORA
          if (E_LORA_WAKEUP == _eMode) {      // get RSSI at last character
              nCountBuf--;
#ifdef DEBUG_LORA
              nRssi = data[nCountBuf];
              debugSerial.print(nRssi);
              debugSerial.print(" rssi. ");
#endif // DEBUG_LORA
          }
      }
  }
  if (nCountBuf > 0) {
    data[nCountBuf] = 0;
    digitalWrite(pinLedIO, LOW);    // turn the LED off by making the voltage LOW
      
#ifdef DEBUG_LORA
    debugSerial.print("\nRece: ");
    printBinary(data, nCountBuf);
    debugSerial.println((char*)data);
#endif // DEBUG_LORA

  }
  // parse downlink packet
  parsePacket(data, szData);
  return nCountBuf;
}

int MOSTLora::parsePacket(byte *data, int szData)
{
  int nRet = -1;
  if (data[0] == '$') {
    
  }
  else if (data[0] == 0xFB && data[1] == 0xFC) {    // for MOST Link protocol
    // downlink header
      MLDownlink header;
      const int szHeader = sizeof(MLDownlink);
      if (szData > szHeader) {
          memcpy(&header, data, szHeader);
          nRet = 0;
      }
  }
  return nRet;
}

boolean MOSTLora::isBusy()
{
  int nBusy = analogRead(pinBZ);
/*  debugSerial.print("busy:");
  debugSerial.print(nBusy, DEC);
  debugSerial.println(".");
  */
  return (nBusy < 512);
}

boolean MOSTLora::waitUntilReady(unsigned long timeout)
{
  boolean bRet = true;
  unsigned long tsStart = millis();
  while (isBusy()) {
    delay(100);
    if (timeout < millis() - tsStart) {
      bRet = false;
      break;
    }
  }
#ifdef DEBUG_LORA
  debugSerial.print((millis() - tsStart));
  debugSerial.println(" Ready");
#endif // DEBUG_LORA
  return bRet;
}

