#ifndef MLPACKETGEN_H
#define MLPACKETGEN_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define ML_MAX_PAYLOAD_SIZE    84
#define ML_MAX_DATA_SIZE    69
#define ML_MAX_OPTION_DATA_SIZE 3

#define CMD_REQ_SET_LORA_CONFIG 0x0001
#define CMD_REQ_DATA            0x0004
#define CMD_RES_SET_LORA_CONFIG 0x0200
#define CMD_RES_DATA            0x0202

#define CMD_NTF_UPLOAD_VINDUINO_FIELD 0x1002
#define VINDUNO_API_KEY_LEN     16

class MLPayloadGen {
    public:
        virtual int getPayload(uint8_t *payload) = 0;
        static MLPayloadGen *createReqDataPayloadGen(uint16_t resInterval, uint8_t dataLen, uint8_t *data, uint8_t option, uint8_t *optionData, uint8_t version=0x0A);
        static MLPayloadGen *createResDataPayloadGen(uint8_t errorCode, uint8_t dataLen, uint8_t *data, uint8_t option, uint8_t *optionData, uint8_t version=0x0A);
        static MLPayloadGen *createNotifyVindunoPayloadGen(uint8_t *apiKey, uint32_t soil_1, uint32_t soil_2, uint32_t soil_3, uint32_t soli_4, 
                uint32_t sysVoltage, uint32_t humidity, uint32_t teamerature, uint32_t resvered, uint8_t option, uint8_t *optionData, uint8_t version=0x0A);
    protected:
        uint8_t _version;
        uint16_t _cmdId;
        uint8_t _optionFlags;
        uint8_t _optionData[ML_MAX_OPTION_DATA_SIZE];
        uint8_t _optionDataLen;
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
        MLNotifyVindunoPayloadGen(uint8_t *apiKey, uint32_t soil_1, uint32_t soil_2, uint32_t soil_3, uint32_t soli_4, 
                uint32_t sysVoltage, uint32_t humidity, uint32_t temperature, uint32_t reserved, uint8_t option, uint8_t *optionData, uint8_t version);
        int getPayload(uint8_t *payload);
    private:
        uint8_t _apiKey[VINDUNO_API_KEY_LEN]; 
        uint32_t _soil_1; 
        uint32_t _soil_2; 
        uint32_t _soil_3; 
        uint32_t _soil_4; 
        uint32_t _sysVoltage; 
        uint32_t _humidity; 
        uint32_t _temperature; 
        uint32_t _reserved;
};

class MLPacketGen {
    public:
        MLPacketGen(uint8_t ackBit, uint8_t receiverFlag, uint8_t packetType, uint8_t direction, uint64_t receiverID, uint64_t senderID = 0, uint8_t version = 0x0A);
        void setMLPayloadGen(MLPayloadGen *mlpayloadGen);
        int getMLPacket(uint8_t *mlpacket);
    private:
        int getMLPayload(uint8_t *payload);
        uint8_t getCrc(const uint8_t *dataBuf, const uint8_t nLen);
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
