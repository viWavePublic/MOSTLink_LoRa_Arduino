/*
  Library for MOSTLink LoRa Shield Library
 
  MOSTLink LoRa Shield with Semtech sx1276 for LoRa technology.
  Support device: Arduino UNO, Linkit One, Vinduino
 
  2016 Copyright (c) viWave Co. Ltd., All right reserved.
  http://mostlink.viwave.com/

  Original Author: Macbear Chen, Auguest 12, 2016.
  --------------------------------------------------------------

*/

#ifndef __MOSTLora_h
#define __MOSTLora_h

#include <Arduino.h>

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_PRO) || defined(ARDUINO_AVR_NANO)

#elif defined(ARDUINO_AVR_MINI)

#define USE_VINDUINO       // for Vinduino project

#elif defined(__LINKIT_ONE__)   // for LinkIt ONE

#else

// unsupported board
#error "ARDUINO BOARD NOT SUPPORTED"

#endif


//#define USE_PIN_LED_LORA     9   // pin-LED for LoRa receive data

#ifndef USE_VINDUINO
#define DEBUG_LORA     // debug by Serial Monitor
#endif
//
enum E_LORA_MODE {
  E_UNKNOWN_LORA_MODE = -1,
  E_LORA_NORMAL = 0,
  E_LORA_WAKEUP = 1,
  E_LORA_POWERSAVING = 2,
  E_LORA_SETUP = 3,
};

// config data for lora (30 bytes)
struct DataLora {
  char tagBegin;      // 0x24
  // --------
  char module_no[4];  // 101H
  char ver_no[7];     // version
  unsigned char mac_addr[8];  // 8 bytes of device mac address
  unsigned char group_id;     // 0~255
  unsigned char freq[3];      // 915000KHz, 868000KHz
  char data_rate;     // (_0_)0.81K, (1)1.46K, (2)2.6K, (3)4.56K, (4) 9.11K, (5)18.23Kbps
  char power;         // 0 ~ 7
  char uart_baud;     // uart baud: (0)1200, (1)2400, (2)4800, (_3_)9600, (4)19200, (5)38400, (6)57600
  char uart_check;    // (_0_)none, (1)odd, (2)even
  char wakeup_time;   // (0)50ms, 100ms, 200ms, 400ms, 600ms, (_5_)1s, 1.5s, 2s, 2.5s, 3s, 4s, (0xb)5s
  // --------
  char tagEnd;        // 0x21
};

class MOSTLora
{
private:
  int _pinP1, _pinP2, _pinBZ;   // p1, p2, busy
  int _eMode;
  DataLora _data;
  unsigned char _receiverID[8];   // receiver ID
public:
  MOSTLora(int pinP1 = 13, int pinP2 = 12, int pinBusy = A2);

  void begin();

private:
  // setup(1,1), normal(0,0), wakeup(0,1), powersaving(1,0)
  void setMode(int p1, int p2);
public:
  void setMode(int mode);
  int getMode() { return _eMode; }
  unsigned char *getMacAddress() { return _data.mac_addr; }
  boolean available();
  boolean setReceiverID(const char *strID);
  
  static void printBinary(const byte *data, const int szData);
  boolean printConfig(DataLora &data);
  boolean printInfo();

  // config setting: drequency, group, data-rate, power, wakeup-time
  void readConfig();
  void writeConfig(long freq, unsigned char group_id = 0, char data_rate = 0, char power = 7, char wakeup_time = 5);
  boolean receConfig(DataLora &data);
    
  // send/rece data via LoRa
  int sendData(char *strData);
  int sendData(byte *data, int szData);
  int receData(byte *data, int szData);
  int parsePacket(byte *data, int szData);

  // LoRa busy state
  boolean isBusy();
  boolean waitUntilReady(unsigned long timeout);

  void run();
    
  /////////////////////////////////////////
  // command packet for MOST Link protocol
  /////////////////////////////////////////
    
  // RES_DATA command for humidity & temperature
  void sendPacketResData(float h, float t);
  void sendPacketResData2(float h, float t);
    
  // NTF_UPLOAD_VINDUINO_FIELD command for Vinduino project
  void sendPacketVinduino(char *apiKey, float f0, float f1, float f2, float f3, float f4, float f5, float f6, float f7);
  void sendPacketVinduino2(char *apiKey, float f0, float f1, float f2, float f3, float f4, float f5, float f6, float f7);
};

#endif // __MOSTLora_h

