#ifndef MOSTLINK_LORAPACKET_H
#define MOSTLINK_LORAPACKET_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define ML_PK_PREAMBLE_1_POS 0
#define ML_PK_PREAMBLE_2_POS 1
#define ML_PK_VERSION_POS 2
#define ML_PK_LENGTH_POS 3
#define ML_PK_FLAGS_POS 4
#define ML_PK_RECEIVER_ID_POS 5
#define ML_PK_SENDER_ID_POS 13

#define ML_MAX_PK_LEN 84
#define ML_MAC_CMD_PARAM_LEN 74

#define CMD_VERSION_POS 0
#define CMD_ID_POS 1
#define CMD_PARAMETERS_POS 3

#define CMD_REQ_D_RES_INT_POS 3
#define CMD_REQ_D_DATA_LEN_POS 5
#define CMD_REQ_D_DATA_POS 6

#define CMD_RES_D_ERR_CODE_POS 3
#define CMD_RES_D_DATA_LENGTH 4
#define CMD_RES_D_DATA_POS 5

#define ML_CMD_REQ_DATA 0x0004
#define ML_CMD_RES_SET_LORA_CONFIG 0x0200
#define ML_CMD_RES_DATA 0x0202

typedef struct mostloraPacketContext {
    uint8_t length;
    uint8_t flag;
    uint64_t nReceiverID;
    uint64_t nSenderID;
    uint8_t nDataLen;
    uint8_t data[ML_MAC_CMD_PARAM_LEN];

    mostloraPacketContext() {
        length = 0;
        flag = 0;
        nReceiverID = 0;
        nSenderID = 0;
        nDataLen = 0;
        memset(data, 0, sizeof(data));
    }
} mostloraPkCtx;

class MOSTLoraPacketParser {
    public:
        int mostlora_packet_parse(mostloraPkCtx *pkctx, const uint8_t *packet);
    private:
        uint8_t getCrc(const uint8_t *dataBuf, const uint8_t nLen);
        int mostlora_payload_parse(mostloraPkCtx *pkctx, const uint8_t *payload);
};

#endif /* MOSTLINK_LORAPACKET_H */
