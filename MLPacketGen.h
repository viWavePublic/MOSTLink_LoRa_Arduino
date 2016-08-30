#ifndef MLPACKETGEN_H
#define MLPACKETGEN_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CMD_REQ_SET_LORA_CONFIG 0x0001
#define CMD_REQ_DATA            0x0004
#define CMD_RES_SET_LORA_CONFIG 0x0200
#define CMD_RES_DATA            0x0202

class MLPayloadGen {
    public:
        virtual int getPayload(uint8_t *payload) = 0;
        virtual int setPayload(uint8_t *data, uint8_t dataLen, uint16_t *param, uint8_t version=0x0A) = 0;
    protected:
        uint8_t _version;
        uint16_t _cmdId;
        uint8_t _dataLen;
        uint8_t _data[74];
};

class MLReqDataPayloadGen : public MLPayloadGen {
    public:
        MLReqDataPayloadGen(uint16_t cmdId);
        int setPayload(uint8_t *data, uint8_t dataLen, uint16_t *param, uint8_t version=0x0A);
        int getPayload(uint8_t *payload);
    private:
        uint16_t _resInterval;
};

class MLResDataPayloadGen : public MLPayloadGen {
    public:
        MLResDataPayloadGen(uint16_t cmdId);
        int setPayload(uint8_t *data, uint8_t dataLen, uint16_t *param, uint8_t version=0x0A);
        int getPayload(uint8_t *payload);
    private:
        uint8_t _errorCode;
};

class PayloadCreator {
    public:
        virtual MLPayloadGen* createMLPayload(uint16_t cmdId) = 0;
};

class MLPayloadCreator : public PayloadCreator {
    public:
        MLPayloadGen* createMLPayload(uint16_t cmdId);
};

class MLPacketGen {
    public:
        MLPacketGen(uint8_t ackBit, uint8_t receiverFlag, uint8_t packetType, uint8_t direction, uint64_t receiverID, uint64_t senderID = 0, uint8_t version = 0x0A);
        void setMLPayloadGen(uint16_t cmdId);
        MLPayloadGen* getMLPayloadGen();
        int getMLPacket(uint8_t *mlpacket);
    private:
        int getMLPayload(uint8_t *payload);
        uint8_t getCrc(const uint8_t *dataBuf, const uint8_t nLen);
        MLPayloadGen* createMLPayload(uint16_t cmdId);
    private:
        uint8_t _ackBit;
        uint8_t _receiverFlag;
        uint8_t _packetType;
        uint8_t _direction;
        uint64_t _receiverID;
        uint64_t _senderID;
        uint8_t _version;
        PayloadCreator *_payloadCreator;
        MLPayloadGen *_mlPayloadGen;
};

#endif /* MLPACKETGEN_H */
