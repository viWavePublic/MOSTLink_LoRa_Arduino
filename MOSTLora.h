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

#include "LoraBase.h"

// callback function
typedef void (* CALLBACK_ReceData) (unsigned char *data, int szData);
typedef void (* CALLBACK_ParseCommand) (int cmdID);


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

class MOSTLora : public LoraBase
{
private:
  DataLora _data;
  unsigned char _receiverID[8];   // receiver ID
    
    boolean _bPacketAES;    // send packet by AES encrypt
//  String _keyHMAC;   // key of HMAC (challenge-response)
    const char *_keyHMAC;   // 16 bytes
    const char *_keyAES;    // 16 bytes (AES128)
    const char *_ivAES;     // 4 bytes

    // callback
    CALLBACK_ReceData _cbReceData;          // common rece data
    CALLBACK_ReceData _cbPacketReqData;             // REQ_DATA
    CALLBACK_ReceData _cbPacketReqAuthChallenge;    // REQ_AUTH_CHALLENGE
    CALLBACK_ReceData _cbPacketNotifyMcsCommand;    // NOTIFY_MCS_COMMAND
    CALLBACK_ReceData _cbPacketNotifyMydevicesCommand;    // NOTIFY_MYDEVICES_COMMAND
    CALLBACK_ParseCommand _cbParseMOSTLink;         // MOSTLink cmdID
    
public:
//    MOSTLora(int pinP1 = 13, int pinP2 = 12, int pinBusy = A2);
    MOSTLora(int pinP1 = 7, int pinP2 = 6, int pinBusy = 5);

  void begin(long speed = 9600);

private:
    int parsePacket();
    
    boolean printConfig(DataLora &data);
    boolean printInfo();
    
public:
  unsigned char *getMacAddress() { return _data.mac_addr; }
  boolean setReceiverID(const char *strID);

  // config setting: drequency, group, data-rate, power, wakeup-time
  boolean readConfig(const int nRetry = 1);
  boolean writeConfig(long freq, unsigned char group_id = 0, char data_rate = 0, char power = 7, char wakeup_time = 5);
  boolean receConfig(DataLora &data);
    
    // send/rece data via LoRa
    int receData();

    void setCallbackReceData(CALLBACK_ReceData cbFunc);
    void setCallbackPacketReqData(CALLBACK_ReceData cbFunc);
    void setCallbackPacketReqAuthChallenge(CALLBACK_ReceData cbFunc);
    void setCallbackPacketNotifyMcsCommand(CALLBACK_ReceData cbFunc);
    void setCallbackPacketNotifyMydevicesCommand(CALLBACK_ReceData cbFunc);
    void setCallbackParseMOSTLink(CALLBACK_ParseCommand cbFunc);
    
    void setKeyHMAC(const char *strKey);
    void setKeyAES(const char *strKey);
    void setIvAES(const char *strIv);
    void setFlagAES(boolean bOn) {
        _bPacketAES = bOn;
    }
    boolean getFlagAES() {
        return _bPacketAES;
    }
    
  /////////////////////////////////////////
  // command packet for MOST Link protocol
  /////////////////////////////////////////
  int sendPacket(byte *pPacket, int szPacket);
    
  // ANS_DATA command for humidity & temperature
  void sendPacketAnsData(float h, float t);
  
  // REQ_SOS/RES_SOS for Lt300 tracker node
  void sendPacketReqSOS(long datetime, char statusGPS, double lat, double lng, char battery);
  void sendPacketAnsSOS();

    // auth related: challenge-reponse
    void sendPacketReqAuthJoin();       // send REQ_AUTH_JOIN to gateway, then will receive REQ_AUTH_CHALLENGE
    void sendPacketAnsAuthResponse(uint8_t *data, int szData);   // received REQ_AUTH_CHALLENGE, then ANS_AUTH_RESPONSE
    
    // MCS related:
    void sendPacketReqLoginMCS(uint8_t *data, int szData);
    void sendPacketSendMCSCommand(uint8_t *data, int szData);

    // myDevices(Cayenne) related:
    void sendPacketReqLoginMydevices(const char *username, const char *pwd, const char *clientID);
    void sendPacketReqLoginMydevices(uint8_t *data, int szData);
    void sendPacketSendMydevicesCommand(uint8_t *data, int szData);
    
  // NOTIFY_LOCATION
  void sendPacketNotifyLocation(unsigned long date_time, unsigned long lat, unsigned long lng);
    
    
  // NTF_UPLOAD_VINDUINO_FIELD command for Vinduino project
  void sendPacketVinduino(const char *apiKey, float f0, float f1, float f2, float f3, float f4, float f5, float f6, float f7);
    
  // NTF_UPLOAD_THINKSPEAK_FIELD command for thingSpeak project
  void sendPacketThingSpeak(const char *apiKey, float f0, float f1, float f2, float f3, float f4, float f5, float f6, float f7);
};

#endif // __MOSTLora_h

