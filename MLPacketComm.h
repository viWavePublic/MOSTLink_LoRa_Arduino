#ifndef MLPACKETCOMM_H
#define MLPACKETCOMM_H

#define ML_MAX_PAYLOAD_SIZE    84
#define ML_MAX_DATA_SIZE    69
#define ML_MAX_OPTION_DATA_SIZE 3
#define ML_FREQUENCY_LEN    3

#define ML_PK_HEADER_SIZE 15
#define ML_PK_ID_SIZE 8
#define ML_PK_HEADER_CRC_SIZE 1
#define ML_PK_CRC_SIZE 1

#define CMD_REQ_SET_LORA_CONFIG 0x0001
#define CMD_REQ_DATA            0x0004
#define CMD_NOTIFY_LOCATION     0x0005
#define CMD_REQ_LOCATION        0x0006
#define CMD_SET_CONFIG_GEOF     0x0007
#define CMD_GET_CONFIG_GEOF     0x0008
#define CMD_REQ_AUTH_JOIN       0x0009
#define CMD_REQ_AUTH_CHALLENGE  0x000A

#define CMD_REQ_LOGIN_MCS       0x000C
#define CMD_REQ_LOGOUT          0x000D
#define CMD_REQ_STILL_ALIVE     0x000E
#define CMD_SEND_MCS_COMMAND    0x000F
#define CMD_NOTIFY_MCS_COMMAND  0x0010

#define CMD_RES_SET_LORA_CONFIG 0x0200
#define CMD_ANS_DATA            0x0202
#define CMD_RET_CONFIG_GEOF     0x0203
#define CMD_ANS_AUTH_RESPONSE   0x0204
#define CMD_RET_AUTH_TOKEN      0x0205


#define CMD_NTF_UPLOAD_VINDUINO_FIELD 0x1002
#define VINDUNO_API_KEY_LEN     16

#define ML_PK_PARSE_OK  0
#define ML_PK_PARSE_NOT_VAILD_DATA -1
#define ML_PK_PARSE_HEADER_CRC_ERR -2
#define ML_PK_PARSE_PACKET_CRC_ERR -3

#define ML_PK_DOWNLINK 0
#define ML_PK_UPLINK 1

#define ML_PK_PREAMBLE_1_POS 0
#define ML_PK_PREAMBLE_2_POS 1
#define ML_PK_VERSION_POS 2
#define ML_PK_LENGTH_POS 3
#define ML_PK_FLAGS_POS 4
#define ML_PK_ID_POS 5
#define ML_PK_HEADER_CRC_POS 13
#define ML_PK_PAYLOAD_START_POS 14

/* Payload */
#define CMD_VERSION_POS 0
#define CMD_ID_POS 1
#define CMD_PARAMETERS_POS 3

#define CMD_REQ_SET_FREQ_POS 3
#define CMD_REQ_SET_DATA_RATE_POS 6
#define CMD_REQ_SET_POWER_POS 7
#define CMD_REQ_SET_WAKEUP_INT_POS 8
#define CMD_REQ_SET_GROUP_ID_POS 9

#define CMD_REQ_D_RES_INT_POS 3
#define CMD_REQ_D_DATA_LEN_POS 5
#define CMD_REQ_D_DATA_POS 6

#define CMD_RES_SET_ERR_CODE_POS 3

#define CMD_ANS_D_ERR_CODE_POS 3
#define CMD_ANS_D_DATA_LENGTH 4
#define CMD_ANS_D_DATA_POS 5

#define CMD_NTF_VINDUINO_API_KEY_POS 3
#define CMD_NTF_VINDUINO_SOIL1_POS 19
#define CMD_NTF_VINDUINO_SOIL2_POS 23
#define CMD_NTF_VINDUINO_SOIL3_POS 27
#define CMD_NTF_VINDUINO_SOIL4_POS 31
#define CMD_NTF_VINDUINO_SYS_VOL_POS 35
#define CMD_NTF_VINDUINO_HUMIDITY_POS 39
#define CMD_NTF_VINDUINO_TEMP_POS 43
#define CMD_NTF_VINDUINO_RESV_POS 47

#define CMD_OPTION_RES_FREQ_NO_RES 0
#define CMD_OPTION_RES_FREQ_WITH_RES 1

#define MAX_VAL_UINT8 0xFF
#define MAX_VAL_UINT16 0xFFFF

namespace {
    uint8_t getCrc(const uint8_t *dataBuffer, const uint8_t length) {									
        uint8_t crc = 0;
        for (uint8_t i=0; i<length; i++) {
            crc^=dataBuffer[i];
        }
        //printf("%x\n", crc);
        return crc;
    }
}

#endif /* MLPACKETCOMM_H */
