#ifndef MLPACKETPARSER_H
#define MLPACKETPARSER_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "MLPacketComm.h"

typedef struct MLPayloadContext {
    uint8_t _cmdVersion;
    uint16_t _cmdId;
    /* REQ_SET_LORA_CONFIG */
    uint8_t _frequency[ML_FREQUENCY_LEN];
    uint8_t _dataRate;
    uint8_t _power;
    uint8_t _wakeupInterval;
    uint8_t _groupId;
    /* REQ_DATA */
    uint16_t _responseInterval;
    /* RES_DATA, RES_SET_LORA_CONFIG */
    uint8_t _errorCode;
    /* REQ_DATA, RES_DATA */
    uint8_t _dataLen;
    uint8_t _data[ML_MAX_DATA_SIZE];
    /* NTF_UPLOAD_VINDUINO_FIELD */
    uint8_t _apiKey[VINDUNO_API_KEY_LEN];
    float _soil1;
    float _soil2;
    float _soil3;
    float _soil4;
    float _systemVoltage;
    float _humidity;
    float _temperature;
    float _reserved;
    /* OPTION */
    uint8_t _optionResFreqFlag;
    uint8_t _resFrequency[ML_FREQUENCY_LEN];

    MLPayloadContext() {
        _cmdVersion = 0;
        _cmdId = 0;
        memset(_frequency, 0, ML_FREQUENCY_LEN);
        _dataRate = 0;
        _power = 0;
        _wakeupInterval = 0;
        _groupId = 0;
        _responseInterval = 0;
        _errorCode = 0;
        _dataLen = 0;
        memset(_data, 0, ML_MAX_DATA_SIZE);
        memset(_apiKey, 0, VINDUNO_API_KEY_LEN);
        _soil1 = 0;
        _soil2 = 0;
        _soil3 = 0;
        _soil4 = 0;
        _systemVoltage = 0;
        _humidity = 0;
        _temperature = 0;
        _reserved = 0;
    }
} MLPayloadCtx;

typedef struct MLPacketContext {
    uint8_t _version;
    uint8_t _ackBit;
    uint8_t _receiverFlag;
    uint8_t _packetType;
    uint8_t _direction;
    uint8_t _receiverID[ML_PK_ID_SIZE];
    uint8_t _senderID[ML_PK_ID_SIZE];
    MLPayloadCtx _mlPayloadCtx;

    MLPacketContext() {
        _version = 0;
        _ackBit = 0;
        _receiverFlag = 0;
        _packetType = 0;
        _direction = 0;
        memset(_receiverID, 0, ML_PK_ID_SIZE);
        memset(_senderID, 0, ML_PK_ID_SIZE);
    }
} MLPacketCtx;

class MLPacketParser {
    public:
        int mostloraPacketParse(MLPacketCtx *pkctx, const uint8_t *packet);
    private:
        int mostloraPayloadParse(MLPacketCtx *pkctx, const uint8_t *payload);
};

#endif /* MLPACKETPARSER_H */
