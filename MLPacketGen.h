#ifndef MLPACKETGEN_H
#define MLPACKETGEN_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "MLPacketComm.h"

class MLPayloadGen {
    public:
        virtual int getPayload(uint8_t *payload) = 0;
        static MLPayloadGen *createReqSetLoraConfigGen(uint8_t *frequency, uint8_t dataRate, uint8_t power, uint8_t wakeupInterval, uint8_t groupId, uint8_t option, uint8_t *optionData, uint8_t version=0x0A);
        static MLPayloadGen *createReqDataPayloadGen(uint16_t resInterval, uint8_t dataLen, uint8_t *data, uint8_t option, uint8_t *optionData, uint8_t version=0x0A);
        static MLPayloadGen *createResSetLoraConfigGen(uint8_t errorCode, uint8_t option, uint8_t *optionData, uint8_t version);
        static MLPayloadGen *createResDataPayloadGen(uint8_t errorCode, uint8_t dataLen, uint8_t *data, uint8_t option, uint8_t *optionData, uint8_t version=0x0A);
        static MLPayloadGen *createNotifyVindunoPayloadGen(uint8_t *apiKey, float soil_1, float soil_2, float soil_3, float soli_4, 
                float sysVoltage, float humidity, float teamerature, float resvered, uint8_t option, uint8_t *optionData, uint8_t version=0x0A);
    protected:
        uint8_t _version;
        uint16_t _cmdId;
        uint8_t _optionFlags;
        uint8_t _optionData[ML_MAX_OPTION_DATA_SIZE];
        uint8_t _optionDataLen;
};

class MLReqSetLoraConfigGen : public MLPayloadGen {
    public:
        MLReqSetLoraConfigGen(uint8_t *frequency, uint8_t dataRate, uint8_t power, uint8_t wakeupInterval, uint8_t groupId, uint8_t optionFlags, uint8_t *optionData, uint8_t version);
        int getPayload(uint8_t *payload);
    private:
        uint8_t _frequency[ML_FREQUENCY_LEN];
        uint8_t _dataRate;
        uint8_t _power;
        uint8_t _wakeupInterval;
        uint8_t _groupId;
};

class MLReqDataPayloadGen : public MLPayloadGen {
    public:
        MLReqDataPayloadGen(uint16_t resInterval, uint8_t dataLen, uint8_t *data, uint8_t option, uint8_t *optionData, uint8_t version);
        int getPayload(uint8_t *payload);
    private:
        uint16_t _resInterval;
        uint8_t _dataLen;
        uint8_t _data[ML_MAX_DATA_SIZE];
};

class MLResSetLoraConfigGen : public MLPayloadGen {
    public:
        MLResSetLoraConfigGen(uint8_t errorCode, uint8_t option, uint8_t *optionData, uint8_t version);
        int getPayload(uint8_t *payload);
    private:
        uint8_t _errorCode;
};

class MLResDataPayloadGen : public MLPayloadGen {
    public:
        MLResDataPayloadGen(uint8_t errorCode, uint8_t dataLen, uint8_t *data, uint8_t option, uint8_t *optionData, uint8_t version);
        int getPayload(uint8_t *payload);
    private:
        uint8_t _errorCode;
        uint8_t _dataLen;
        uint8_t _data[ML_MAX_DATA_SIZE];
};

class MLNotifyVindunoPayloadGen : public MLPayloadGen {
    public:
        MLNotifyVindunoPayloadGen(uint8_t *apiKey, float soil_1, float soil_2, float soil_3, float soli_4, 
                float sysVoltage, float humidity, float temperature, float reserved, uint8_t option, uint8_t *optionData, uint8_t version);
        int getPayload(uint8_t *payload);
    private:
        uint8_t _apiKey[VINDUNO_API_KEY_LEN]; 
        float _soil_1;
        float _soil_2;
        float _soil_3;
        float _soil_4;
        float _sysVoltage;
        float _humidity;
        float _temperature;
        float _reserved;
};

class MLPacketGen {
    public:
        MLPacketGen(uint8_t ackBit, uint8_t receiverFlag, uint8_t packetType, uint8_t direction, uint64_t receiverID, uint64_t senderID = 0, uint8_t version = 0x0A);
        void setMLPayloadGen(MLPayloadGen *mlpayloadGen);
        int getMLPacket(uint8_t *mlpacket);
    private:
        int getMLPayload(uint8_t *payload);
    private:
        uint8_t _ackBit;
        uint8_t _receiverFlag;
        uint8_t _packetType;
        uint8_t _direction;
        uint64_t _receiverID;
        uint64_t _senderID;
        uint8_t _version;
        MLPayloadGen *_mlPayloadGen;
};

#endif /* MLPACKETGEN_H */
