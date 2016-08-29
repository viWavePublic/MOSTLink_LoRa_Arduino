#ifndef MLPACKETGEN_H
#define MLPACKETGEN_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

class MLPayloadGen {
    public:
        MLPayloadGen(uint16_t cmdId, uint16_t resInterval,uint8_t dataLen, uint8_t *data, uint8_t version=0x0A);
        int getPayload(uint8_t *payload);

    private:
        uint8_t _version;
        uint16_t _cmdId;
        uint16_t _resInterval;
        uint8_t _dataLen;
        uint8_t _data[74];
};

class MLPacketGen {
    public:
        MLPacketGen(uint8_t ackBit, uint8_t receiverFlag, uint8_t packetType, uint8_t direction, uint64_t receiverID, uint64_t senderID = 0, uint8_t version = 0x0A);
        int setMLPayload(uint16_t cmdId, uint16_t resInterval,uint8_t dataLen, uint8_t *data, uint8_t version = 0x0A);
        int getMLPacket(uint8_t *mlpacket);
        ~MLPacketGen() {
            delete _mlPayloadGen;
            _mlPayloadGen = NULL;
        }
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
