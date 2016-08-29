#include "MLPacketParser.h"

int MOSTLoraPacketParser::mostlora_packet_parse(mostloraPkCtx *pkctx, const uint8_t *packet) {
    uint8_t nPacketLen;
    uint8_t flag;
    uint64_t nReceiverID = 0;
    uint64_t nSenderID = 0;
    uint8_t payload[ML_MAX_PK_LEN] = {0};
    int i;
    
    if(packet[ML_PK_PREAMBLE_1_POS] == 0xFB && packet[ML_PK_PREAMBLE_2_POS] == 0xFC){
        nPacketLen = packet[ML_PK_LENGTH_POS];
        flag = packet[ML_PK_FLAGS_POS];
        uint8_t pos = ML_PK_RECEIVER_ID_POS;
        //printf("%x\n", flag);
        if (flag >> 3 & 0x01) {
            //printf("uplink packet\n");
            nReceiverID = packet[pos++] << 56 | packet[pos++] << 48 | packet[pos++] << 40 | packet[pos++] << 32 | packet[pos++] << 24 | packet[pos++] << 16 | packet[pos++] << 8 | packet[pos++];
            nSenderID = packet[pos++] << 56 | packet[pos++] << 48 | packet[pos++] << 40 | packet[pos++] << 32 | packet[pos++] << 24 | packet[pos++] << 16 | packet[pos++] << 8 | packet[pos++];
        } else {
            //printf("downlink packet\n");
            nReceiverID = packet[pos++] << 56 | packet[pos++] << 48 | packet[pos++] << 40 | packet[pos++] << 32 | packet[pos++] << 24 | packet[pos++] << 16 | packet[pos++] << 8 | packet[pos++];
        }
        if (packet[pos] != getCrc(packet, pos++)) {
            //printf("%x\n", buf[pos]);
            printf("CRC error 1\n");
            return -1;
        } else {
            pkctx->length = nPacketLen;
            pkctx->flag = flag;
            pkctx->nReceiverID = nReceiverID;
            pkctx->nSenderID = nSenderID;
            if (packet[nPacketLen-1] == getCrc(packet, nPacketLen-1)) {
                printf("pLen: %d\n", nPacketLen-pos-1);
                printf("buf[%d]: %x\n", pos, packet[pos]);
                memcpy(payload, packet+pos, nPacketLen-pos-1);
                pkctx->nDataLen = nPacketLen-pos-1;
                mostlora_payload_parse(pkctx, payload);
                for (i=0; i<pkctx->nDataLen; i++) {
                    printf("Data[%d] = %x\n", i , pkctx->data[i]);
                }
            } else {
                printf("CRC)error 2\n");
            }
            return 0;
        }
    } else
        return -1;
}

uint8_t MOSTLoraPacketParser::getCrc(const uint8_t *dataBuffer, const uint8_t length) {									
    uint8_t crc = 0;

    for (uint8_t i=0; i<length; i++) {
        crc^=dataBuffer[i];
    }
    printf("%x\n", crc);
    return crc;
}

int MOSTLoraPacketParser::mostlora_payload_parse(mostloraPkCtx *pkctx, const uint8_t *payload) {
    uint8_t nCommandVer;
    uint16_t nCommandID;
    uint16_t nResInterval;
    uint8_t nErrCode;

    nCommandID = payload[CMD_ID_POS] << 8 | payload[CMD_ID_POS + 1];
    if (nCommandID >= 0x0001 && nCommandID <= 0x02FF) {
        switch (nCommandID) {
            case ML_CMD_REQ_DATA:
                nResInterval = payload[CMD_REQ_D_RES_INT_POS] << 8 | payload[CMD_REQ_D_RES_INT_POS+1];
                pkctx->nDataLen = payload[CMD_REQ_D_DATA_LEN_POS];
                memcpy(pkctx->data, payload+CMD_REQ_D_DATA_POS, pkctx->nDataLen);
                break;
            case ML_CMD_RES_DATA:
                nErrCode = payload[CMD_RES_D_ERR_CODE_POS];
                pkctx->nDataLen = payload[CMD_RES_D_DATA_LENGTH];
                memcpy(pkctx->data, payload+CMD_REQ_D_DATA_POS, pkctx->nDataLen);
                break;
            default:
                break;
        }
    } else {
        memcpy(pkctx->data, payload+CMD_PARAMETERS_POS, pkctx->nDataLen);
    }
    return 0;
}

