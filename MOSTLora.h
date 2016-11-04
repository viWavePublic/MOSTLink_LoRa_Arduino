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

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO)

#elif defined(ARDUINO_AVR_PRO)  // vinduino pro mini

#define USE_VINDUINO  // for Vinduino project (unmark the line)

#elif defined(ARDUINO_AVR_MINI) //

#elif defined(__LINKIT_ONE__)   // for LinkIt ONE

#else

// unsupported board
#error "ARDUINO BOARD NOT SUPPORTED"

#endif


//#define USE_VINDUINO  // for Vinduino project (unmark the line)
#ifdef USE_VINDUINO     // for Vinduino project

#else // USE_VINDUINO
#define DEBUG_LORA     // debug by Serial Monitor
#endif // USE_VINDUINO

// debug LoRa to serial-monitor
#ifdef DEBUG_LORA
#define debugSerial Serial
#else
// not ready yet
class DummySerial {
    
};
//#define debugSerial DummySerial

#endif


//#define USE_PIN_LED_LORA     13   // pin-LED for LoRa receive data


// 4 mode: normal, wakeup, powersaving, setup
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
    
    boolean isValid() const {      // begin(0x24), end(0x21)
        return (tagBegin == 0x24 && tagEnd == 0x21);
    }
};

class MOSTLora
{
#define MAX_SIZE_BUF        99
private:
  int _pinP1, _pinP2, _pinBZ;   // p1, p2, busy
  int _eMode;
  DataLora _data;
  unsigned char _receiverID[8];   // receiver ID
  
  int _szBuf;
  unsigned char _buf[MAX_SIZE_BUF + 1];
public:
//    MOSTLora(int pinP1 = 13, int pinP2 = 12, int pinBusy = A2);
    MOSTLora(int pinP1 = 7, int pinP2 = 6, int pinBusy = 5);

  void begin();
  unsigned char *getBuffer() { return _buf; }

private:
  // setup(1,1), normal(0,0), wakeup(0,1), powersaving(1,0)
  void setMode(int p1, int p2);
public:
  void setMode(int mode);
  int getMode() { return _eMode; }
  unsigned char *getMacAddress() { return _data.mac_addr; }
  boolean available();
  boolean setReceiverID(const char *strID);
  
  boolean printConfig(DataLora &data);
  boolean printInfo();

  // config setting: drequency, group, data-rate, power, wakeup-time
  boolean readConfig();
  boolean writeConfig(long freq, unsigned char group_id = 0, char data_rate = 0, char power = 7, char wakeup_time = 5);
  boolean receConfig(DataLora &data);
    
  // send/rece data via LoRa
  int sendData(const char *strData);
  int sendData(byte *data, int szData);
  int receData();
  int parsePacket();

  // LoRa busy state
  boolean isBusy();
  boolean waitUntilReady(unsigned long timeout);

  void run();
    
  /////////////////////////////////////////
  // command packet for MOST Link protocol
  /////////////////////////////////////////
  int sendPacket(byte *pPacket, int szPacket);
    
  // RES_DATA command for humidity & temperature
  void sendPacketResData(float h, float t);
  void sendPacketResData_old(float h, float t);
  
  // REQ_SOS/RES_SOS for Lt300 tracker node
  void sendPacketReqSOS(long datetime, char statusGPS, double lat, double lng, char battery);
  void sendPacketResSOS();

    // auth related: challenge-reponse
    void sendPacketReqAuthJoin();       // send REQ_AUTH_JOIN to gateway, then will receive REQ_AUTH_CHALLENGE
    void sendPacketResAuthResponse(uint8_t *data, int szData);   // received REQ_AUTH_CHALLENGE, then RES_AUTH_RESPONSE
    
  // NOTIFY_LOCATION
  void sendPacketNotifyLocation(unsigned long date_time, unsigned long lat, unsigned long lng);
    
    
  // NTF_UPLOAD_VINDUINO_FIELD command for Vinduino project
  void sendPacketVinduino(const char *apiKey, float f0, float f1, float f2, float f3, float f4, float f5, float f6, float f7);
  void sendPacketVinduino_old(const char *apiKey, float f0, float f1, float f2, float f3, float f4, float f5, float f6, float f7);
    
  void sendPacketThingSpeak(const char *apiKey, float f0, float f1, float f2, float f3, float f4, float f5, float f6, float f7);
};

#endif // __MOSTLora_h

