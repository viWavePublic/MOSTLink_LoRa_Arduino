#include "MLPacketParser.h"
#include "MLPacketGen.h"
#include "MLPacketGen2.h"
#include "MLPacketGen3.h"
#include "MLutility.h"

#define _DEBUG_MSG_ 1

int MLPacketParser::mostloraPacketParse(MLPacketGen *mlpacket, const uint8_t *packet) {
    uint8_t nPacketLen;
    uint8_t payload[ML_MAX_PAYLOAD_SIZE] = {0};
    
    if(packet[ML_PK_PREAMBLE_1_POS] == 0xFB && packet[ML_PK_PREAMBLE_2_POS] == 0xFC){
        if (packet[ML_PK_HEADER_CRC_POS] != getCrc(packet, ML_PK_HEADER_SIZE-ML_PK_HEADER_CRC_SIZE-ML_PK_CRC_SIZE)) {
#if _DEBUG_MSG_
            printf("CRC error 1\n");
#endif
            return ML_PK_PARSE_HEADER_CRC_ERR;
        } else {
            nPacketLen = packet[ML_PK_LENGTH_POS];
            uint8_t version = packet[ML_PK_VERSION_POS];
            uint8_t ackBit = packet[ML_PK_FLAGS_POS] & 0x01;
            uint8_t receiverFlag = (packet[ML_PK_FLAGS_POS] >> 1) & 0x01;
            uint8_t packetType = (packet[ML_PK_FLAGS_POS] >>2) & 0x01;
            uint8_t direction = (packet[ML_PK_FLAGS_POS] >> 3) & 0x01;
            uint8_t id[ML_PK_ID_SIZE];
            memcpy(id, &packet[ML_PK_ID_POS], ML_PK_ID_SIZE);
            mlpacket->setMLPacket(ackBit, receiverFlag, packetType, direction, id, version);
            uint8_t packetCRCPos = nPacketLen - 1;
            int payloadLen = nPacketLen-ML_PK_PAYLOAD_START_POS;
#if _DEBUG_MSG_
            printf("packetLen:%d, payloadLen:%d\n", nPacketLen, payloadLen);
#endif
            if (packet[packetCRCPos] == getCrc(packet, nPacketLen-1)) {
                memcpy(payload, packet+ML_PK_PAYLOAD_START_POS, payloadLen);
                mostloraPayloadParse(mlpacket, payload, payloadLen);
            } else {
#if _DEBUG_MSG_
                printf("CRC error 2\n");
#endif                
                return ML_PK_PARSE_PACKET_CRC_ERR;
            }
            return ML_PK_PARSE_OK;
        }
    } else
        return ML_PK_PARSE_NOT_VAILD_DATA;
}

int MLPacketParser::mostloraPayloadParse(MLPacketGen *mlpacket, const uint8_t *payload, const int szPayload) {
    uint8_t frequency[ML_FREQUENCY_LEN] = {0};
    uint8_t dataRate;
    uint8_t power;
    uint8_t wakeupInterval;
    uint8_t groupId;
    uint16_t responseInterval;
    uint8_t dataLen;
    uint8_t data[ML_MAX_DATA_SIZE] = {0};
    uint8_t errorCode;
    uint8_t apikey[VINDUINO_API_KEY_LEN] = {0};
    float soil1,soil2,soil3,soil4,systemVoltage,humidity,temperature,reserved;

    uint8_t cmdVersion = payload[CMD_VERSION_POS];
    uint16_t cmdId = payload[CMD_ID_POS+1] << 8 | payload[CMD_ID_POS];
    uint8_t optionFlagsPos = 0;
    switch (cmdId) {
        case CMD_REQ_SET_LORA_CONFIG:
            memcpy(frequency, payload+CMD_REQ_SET_FREQ_POS, ML_FREQUENCY_LEN);
            dataRate = payload[CMD_REQ_SET_DATA_RATE_POS];
            power = payload[CMD_REQ_SET_POWER_POS];
            wakeupInterval = payload[CMD_REQ_SET_WAKEUP_INT_POS];
            groupId = payload[CMD_REQ_SET_GROUP_ID_POS];
            mlpacket->setMLPayloadGen(new MLReqSetLoraConfigGen(frequency, dataRate, power, wakeupInterval, groupId));
            optionFlagsPos = CMD_REQ_SET_GROUP_ID_POS + 1;
            break;
        case CMD_REQ_DATA:
            responseInterval = payload[CMD_REQ_D_RES_INT_POS+1] << 8 | payload[CMD_REQ_D_RES_INT_POS];
            dataLen = payload[CMD_REQ_D_DATA_LEN_POS];
            memcpy(data, payload+CMD_REQ_D_DATA_POS, dataLen);
            mlpacket->setMLPayloadGen(new MLReqDataPayloadGen(responseInterval, dataLen, data));
            optionFlagsPos = CMD_REQ_D_DATA_POS + dataLen;
            break;
        case CMD_ANS_SET_LORA_CONFIG:
            errorCode = payload[CMD_ANS_SET_ERR_CODE_POS];
            mlpacket->setMLPayloadGen(new MLAnsSetLoraConfigGen(errorCode));
            optionFlagsPos = CMD_ANS_SET_ERR_CODE_POS + 1;
            break;
        case CMD_ANS_DATA:
            errorCode = payload[CMD_ANS_D_ERR_CODE_POS];
            dataLen = payload[CMD_ANS_D_DATA_LENGTH];
            memcpy(data, payload+CMD_ANS_D_DATA_POS, dataLen);
            mlpacket->setMLPayloadGen(new MLAnsDataPayloadGen(errorCode, dataLen, data));
            optionFlagsPos = CMD_ANS_D_DATA_POS + dataLen;
            break;
        case CMD_REQ_AUTH_JOIN:
            mlpacket->setMLPayloadGen(new MLReqAuthJoinPayloadGen());
            break;
        case CMD_REQ_AUTH_CHALLENGE:
            memcpy(data, payload + 3, 4);
            mlpacket->setMLPayloadGen(new MLReqAuthChallengePayloadGen(data));
            break;
        case CMD_ANS_AUTH_RESPONSE:
            mlpacket->setMLPayloadGen(new MLAnsAuthResponsePayloadGen(data));
            break;
        case CMD_NOTIFY_MCS_COMMAND:
            dataLen = payload[3];
            memcpy(data, payload + 4, dataLen);
            mlpacket->setMLPayloadGen(new MLNotifyMcsCommandPayloadGen(dataLen, data));
            break;
        case CMD_NOTIFY_MYDEVICES_COMMAND:
            dataLen = payload[3];
            memcpy(data, payload + 4, dataLen);
            mlpacket->setMLPayloadGen(new MLNotifyMydevicesCommandPayloadGen(dataLen, data));
            break;
        case CMD_NTF_UPLOAD_VINDUINO_FIELD:
            memcpy(apikey, payload+CMD_NTF_VINDUINO_API_KEY_POS, VINDUINO_API_KEY_LEN);
            soil1 = MLutility::convertHexToDec(payload + CMD_NTF_VINDUINO_SOIL1_POS, 4, true);
            soil2 = MLutility::convertHexToDec(payload + CMD_NTF_VINDUINO_SOIL2_POS, 4, true);
            soil3 = MLutility::convertHexToDec(payload + CMD_NTF_VINDUINO_SOIL3_POS, 4, true);
            soil4 = MLutility::convertHexToDec(payload + CMD_NTF_VINDUINO_SOIL4_POS, 4, true);
            systemVoltage = MLutility::convertHexToDec(payload + CMD_NTF_VINDUINO_SYS_VOL_POS, 4, true);
            humidity = MLutility::convertHexToDec(payload + CMD_NTF_VINDUINO_HUMIDITY_POS, 4, true);
            temperature = MLutility::convertHexToDec(payload + CMD_NTF_VINDUINO_TEMP_POS, 4, true);
            reserved = MLutility::convertHexToDec(payload + CMD_NTF_VINDUINO_RESV_POS, 4, true);
/*
            soil1 = payload[CMD_NTF_VINDUINO_SOIL1_POS+3] << 24 | payload[CMD_NTF_VINDUINO_SOIL1_POS+2] << 16
                | payload[CMD_NTF_VINDUINO_SOIL1_POS+1] << 8 | payload[CMD_NTF_VINDUINO_SOIL1_POS];
            soil2 = payload[CMD_NTF_VINDUINO_SOIL2_POS+3] << 24 | payload[CMD_NTF_VINDUINO_SOIL2_POS+2] << 16
                | payload[CMD_NTF_VINDUINO_SOIL2_POS+1] << 8 | payload[CMD_NTF_VINDUINO_SOIL2_POS];
            soil3 = payload[CMD_NTF_VINDUINO_SOIL3_POS+3] << 24 | payload[CMD_NTF_VINDUINO_SOIL3_POS+2] << 16
                | payload[CMD_NTF_VINDUINO_SOIL3_POS+1] << 8 | payload[CMD_NTF_VINDUINO_SOIL3_POS];
            soil4 = payload[CMD_NTF_VINDUINO_SOIL4_POS+3] << 24 | payload[CMD_NTF_VINDUINO_SOIL4_POS+2] << 16
                | payload[CMD_NTF_VINDUINO_SOIL4_POS+1] << 8 | payload[CMD_NTF_VINDUINO_SOIL4_POS];
            systemVoltage = payload[CMD_NTF_VINDUINO_SYS_VOL_POS+3] << 24 | payload[CMD_NTF_VINDUINO_SYS_VOL_POS+2] << 16
                | payload[CMD_NTF_VINDUINO_SYS_VOL_POS+1] << 8 | payload[CMD_NTF_VINDUINO_SYS_VOL_POS];
            humidity = payload[CMD_NTF_VINDUINO_HUMIDITY_POS+3] << 24 | payload[CMD_NTF_VINDUINO_HUMIDITY_POS+2] << 16
                | payload[CMD_NTF_VINDUINO_HUMIDITY_POS+1] << 8 | payload[CMD_NTF_VINDUINO_HUMIDITY_POS];
            temperature = payload[CMD_NTF_VINDUINO_TEMP_POS+3] << 24 | payload[CMD_NTF_VINDUINO_TEMP_POS+2] << 16
                | payload[CMD_NTF_VINDUINO_TEMP_POS+1] << 8 | payload[CMD_NTF_VINDUINO_TEMP_POS];
            reserved = payload[CMD_NTF_VINDUINO_RESV_POS+3] << 24 | payload[CMD_NTF_VINDUINO_RESV_POS+2] << 16
                | payload[CMD_NTF_VINDUINO_RESV_POS+1] << 8 | payload[CMD_NTF_VINDUINO_RESV_POS];
 */
            mlpacket->setMLPayloadGen(new MLNotifyVindunoPayloadGen(apikey, soil1, soil2, soil3, soil4, systemVoltage, humidity, temperature, reserved));
            optionFlagsPos = CMD_NTF_VINDUINO_RESV_POS + 4;
            break;
        case CMD_ANS_NDCALL:
        case CMD_REQ_SOS:
            if (szPayload >= 19)
            {
                MLPayloadGen *pPayloadGPS;
                if (CMD_ANS_NDCALL == cmdId)
                    pPayloadGPS = new MLAnsNDCallPayloadGen();
                else
                    pPayloadGPS = new MLReqSOSPayloadGen();
                pPayloadGPS->setPayload(payload, szPayload);
                mlpacket->setMLPayloadGen(pPayloadGPS);
            }
            break;
        case CMD_ANS_SOS:
        {
            MLAnsSOSPayloadGen *pPayloadSOS = new MLAnsSOSPayloadGen();
            pPayloadSOS->setPayload(payload, szPayload);
            mlpacket->setMLPayloadGen(pPayloadSOS);
        }
            // MOSTLink 1.5
        case CMD_REP_LOCATION:
        {
            MLReportLocationPayloadGen *pPayloadLoc = new MLReportLocationPayloadGen();
            pPayloadLoc->setPayload(payload, szPayload);
            mlpacket->setMLPayloadGen(pPayloadLoc);
        }
            break;
        case CMD_REP_BEACON:
        {
            MLReportBeaconPayloadGen *pPayloadBeacon = new MLReportBeaconPayloadGen();
            pPayloadBeacon->setPayload(payload, szPayload);
            mlpacket->setMLPayloadGen(pPayloadBeacon);
        }
            break;
        case CMD_REQ_ALARM_GPS:
        {
            MLAlarmGPSPayloadGen *pPayloadLoc = new MLAlarmGPSPayloadGen();
            pPayloadLoc->setPayload(payload, szPayload);
            mlpacket->setMLPayloadGen(pPayloadLoc);
        }
            break;
        case CMD_REQ_ALARM_BEACON:
        {
            MLReqAlarmBeaconPayloadGen *pPayloadBeacon = new MLReqAlarmBeaconPayloadGen();
            pPayloadBeacon->setPayload(payload, szPayload);
            mlpacket->setMLPayloadGen(pPayloadBeacon);
        }
            break;
        default:
            mlpacket->setMLPayloadGen(new MLPayloadGen(cmdId));
            break;
    }
    uint8_t optionResFreqFlag = payload[optionFlagsPos++] & 0x01;
    uint8_t resFrequency[ML_FREQUENCY_LEN] = {0};
    if (CMD_OPTION_ANS_FREQ_WITH_RES == optionResFreqFlag) {
        MLPayloadGen* mlPayload = mlpacket->getMLPayload();
        if (mlPayload)
            mlPayload->setMLPayloadOption(optionResFreqFlag, resFrequency);
    }

    return 0;
}
