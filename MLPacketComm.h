#ifndef MLPACKETCOMM_H
#define MLPACKETCOMM_H

#define ML_MAX_PAYLOAD_SIZE    84
#define ML_MAX_DATA_SIZE    69
#define ML_MAX_OPTION_DATA_SIZE 3
#define ML_FREQUENCY_LEN    3

#define CMD_REQ_SET_LORA_CONFIG 0x0001
#define CMD_REQ_DATA            0x0004
#define CMD_RES_SET_LORA_CONFIG 0x0200
#define CMD_RES_DATA            0x0202

#define CMD_NTF_UPLOAD_VINDUINO_FIELD 0x1002
#define VINDUNO_API_KEY_LEN     16

#define ML_PK_PARSE_OK  0
#define ML_PK_PARSE_NOT_VAILD_DATA -1
#define ML_PK_PARSE_HEADER_CRC_ERR -2
#define ML_PK_PARSE_PACKET_CRC_ERR -3

#define ML_PK_DOWNLINK 0
#define ML_PK_UPLINK 1

#define ML_PK_DOWNLINK_CRC_POS 13
#define ML_PK_UPLINK_CRC_POS 21

#define ML_PK_PREAMBLE_1_POS 0
#define ML_PK_PREAMBLE_2_POS 1
#define ML_PK_VERSION_POS 2
#define ML_PK_LENGTH_POS 3
#define ML_PK_FLAGS_POS 4
#define ML_PK_RECEIVER_ID_POS 5
#define ML_PK_SENDER_ID_POS 13

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

#define CMD_RES_D_ERR_CODE_POS 3
#define CMD_RES_D_DATA_LENGTH 4
#define CMD_RES_D_DATA_POS 5

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

namespace {
    uint8_t getCrc(const uint8_t *dataBuffer, const uint8_t length) {									
        uint8_t crc = 0;
        for (uint8_t i=0; i<length; i++) {
            crc^=dataBuffer[i];
        }
        printf("%x\n", crc);
        return crc;
    }
}

#endif /* MLPACKETCOMM_H */
