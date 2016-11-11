#include "MLPacketParser.h"

int MLPacketParser::mostloraPacketParse(MLPacketCtx *pkctx, const uint8_t *packet) {
    uint8_t nPacketLen;
    uint8_t payload[ML_MAX_PAYLOAD_SIZE] = {0};
    
    if(packet[ML_PK_PREAMBLE_1_POS] == 0xFB && packet[ML_PK_PREAMBLE_2_POS] == 0xFC){
        if (packet[ML_PK_HEADER_CRC_POS] != getCrc(packet, ML_PK_HEADER_SIZE-ML_PK_HEADER_CRC_SIZE-ML_PK_CRC_SIZE)) {
//            printf("CRC error 1\n");
            return ML_PK_PARSE_HEADER_CRC_ERR;
        } else {
            nPacketLen = packet[ML_PK_LENGTH_POS];
            pkctx->_version = packet[ML_PK_VERSION_POS];
            pkctx->_ackBit = packet[ML_PK_FLAGS_POS] & 0x01;
            pkctx->_receiverFlag = (packet[ML_PK_FLAGS_POS] >> 1) & 0x01;
            pkctx->_packetType = (packet[ML_PK_FLAGS_POS] >>2) & 0x01;
            pkctx->_direction = (packet[ML_PK_FLAGS_POS] >> 3) & 0x01;
            memcpy(pkctx->_id, &packet[ML_PK_ID_POS], ML_PK_ID_SIZE);
            uint8_t packetCRCPos = nPacketLen - 1;
            uint8_t payloadLen = nPacketLen-ML_PK_HEADER_SIZE;
//            printf("packetLen:%d, payloadLen:%d\n", nPacketLen, payloadLen);
            if (packet[packetCRCPos] == getCrc(packet, nPacketLen-1)) {
                memcpy(payload, packet+ML_PK_PAYLOAD_START_POS, payloadLen);
                mostloraPayloadParse(pkctx, payload);
            } else {
//                printf("CRC error 2\n");
                return ML_PK_PARSE_PACKET_CRC_ERR;
            }
            return 0;
        }
    } else
        return ML_PK_PARSE_NOT_VAILD_DATA;
}

int MLPacketParser::mostloraPayloadParse(MLPacketCtx *pkctx, const uint8_t *payload) {
    pkctx->_mlPayloadCtx._cmdVersion = payload[CMD_VERSION_POS];
    pkctx->_mlPayloadCtx._cmdId = payload[CMD_ID_POS+1] << 8 | payload[CMD_ID_POS];
    uint8_t optionFlagsPos = 0;
    switch (pkctx->_mlPayloadCtx._cmdId) {
        case CMD_REQ_SET_LORA_CONFIG:
            memcpy(pkctx->_mlPayloadCtx._frequency, payload+CMD_REQ_SET_FREQ_POS, ML_FREQUENCY_LEN);
            pkctx->_mlPayloadCtx._dataRate = payload[CMD_REQ_SET_DATA_RATE_POS];
            pkctx->_mlPayloadCtx._power = payload[CMD_REQ_SET_POWER_POS];
            pkctx->_mlPayloadCtx._wakeupInterval = payload[CMD_REQ_SET_WAKEUP_INT_POS];
            pkctx->_mlPayloadCtx._groupId = payload[CMD_REQ_SET_GROUP_ID_POS];
            optionFlagsPos = CMD_REQ_SET_GROUP_ID_POS + 1;
            break;
        case CMD_REQ_DATA:
            pkctx->_mlPayloadCtx._responseInterval = payload[CMD_REQ_D_RES_INT_POS+1] << 8 | payload[CMD_REQ_D_RES_INT_POS];
            pkctx->_mlPayloadCtx._dataLen = payload[CMD_REQ_D_DATA_LEN_POS];
            memcpy(pkctx->_mlPayloadCtx._data, payload+CMD_REQ_D_DATA_POS, pkctx->_mlPayloadCtx._dataLen);
            optionFlagsPos = CMD_REQ_D_DATA_POS + pkctx->_mlPayloadCtx._dataLen;
            break;
        case CMD_RES_SET_LORA_CONFIG:
            pkctx->_mlPayloadCtx._errorCode = payload[CMD_RES_SET_ERR_CODE_POS];
            optionFlagsPos = CMD_RES_SET_ERR_CODE_POS + 1;
            break;
        case CMD_RES_DATA:
            pkctx->_mlPayloadCtx._errorCode = payload[CMD_RES_D_ERR_CODE_POS];
            pkctx->_mlPayloadCtx._dataLen = payload[CMD_RES_D_DATA_LENGTH];
            memcpy(pkctx->_mlPayloadCtx._data, payload+CMD_REQ_D_DATA_POS, pkctx->_mlPayloadCtx._dataLen);
            optionFlagsPos = CMD_REQ_D_DATA_POS + pkctx->_mlPayloadCtx._dataLen;
            break;
        case CMD_NTF_UPLOAD_VINDUINO_FIELD:
            memcpy(pkctx->_mlPayloadCtx._apiKey, payload+CMD_NTF_VINDUINO_API_KEY_POS, VINDUNO_API_KEY_LEN);
            pkctx->_mlPayloadCtx._soil1 = payload[CMD_NTF_VINDUINO_SOIL1_POS+3] << 24 | payload[CMD_NTF_VINDUINO_SOIL1_POS+2] << 16
                | payload[CMD_NTF_VINDUINO_SOIL1_POS+1] << 8 | payload[CMD_NTF_VINDUINO_SOIL1_POS];
            pkctx->_mlPayloadCtx._soil2 = payload[CMD_NTF_VINDUINO_SOIL2_POS+3] << 24 | payload[CMD_NTF_VINDUINO_SOIL2_POS+2] << 16
                | payload[CMD_NTF_VINDUINO_SOIL2_POS+1] << 8 | payload[CMD_NTF_VINDUINO_SOIL2_POS];
            pkctx->_mlPayloadCtx._soil3 = payload[CMD_NTF_VINDUINO_SOIL3_POS+3] << 24 | payload[CMD_NTF_VINDUINO_SOIL3_POS+2] << 16
                | payload[CMD_NTF_VINDUINO_SOIL3_POS+1] << 8 | payload[CMD_NTF_VINDUINO_SOIL3_POS];
            pkctx->_mlPayloadCtx._soil4 = payload[CMD_NTF_VINDUINO_SOIL4_POS+3] << 24 | payload[CMD_NTF_VINDUINO_SOIL4_POS+2] << 16
                | payload[CMD_NTF_VINDUINO_SOIL4_POS+1] << 8 | payload[CMD_NTF_VINDUINO_SOIL4_POS];
            pkctx->_mlPayloadCtx._systemVoltage = payload[CMD_NTF_VINDUINO_SYS_VOL_POS+3] << 24 | payload[CMD_NTF_VINDUINO_SYS_VOL_POS+2] << 16
                | payload[CMD_NTF_VINDUINO_SYS_VOL_POS+1] << 8 | payload[CMD_NTF_VINDUINO_SYS_VOL_POS];
            pkctx->_mlPayloadCtx._humidity = payload[CMD_NTF_VINDUINO_HUMIDITY_POS+3] << 24 | payload[CMD_NTF_VINDUINO_HUMIDITY_POS+2] << 16
                | payload[CMD_NTF_VINDUINO_HUMIDITY_POS+1] << 8 | payload[CMD_NTF_VINDUINO_HUMIDITY_POS];
            pkctx->_mlPayloadCtx._temperature = payload[CMD_NTF_VINDUINO_TEMP_POS+3] << 24 | payload[CMD_NTF_VINDUINO_TEMP_POS+2] << 16
                | payload[CMD_NTF_VINDUINO_TEMP_POS+1] << 8 | payload[CMD_NTF_VINDUINO_TEMP_POS];
            pkctx->_mlPayloadCtx._reserved = payload[CMD_NTF_VINDUINO_RESV_POS+3] << 24 | payload[CMD_NTF_VINDUINO_RESV_POS+2] << 16
                | payload[CMD_NTF_VINDUINO_RESV_POS+1] << 8 | payload[CMD_NTF_VINDUINO_RESV_POS];
            optionFlagsPos = CMD_NTF_VINDUINO_RESV_POS + 4;
            break;
        default:
            break;
    }
    pkctx->_mlPayloadCtx._optionResFreqFlag = payload[optionFlagsPos++] & 0x01;
    if (CMD_OPTION_RES_FREQ_WITH_RES == pkctx->_mlPayloadCtx._optionResFreqFlag)
        memcpy(pkctx->_mlPayloadCtx._resFrequency, &payload[optionFlagsPos], ML_FREQUENCY_LEN);

    return 0;
}
