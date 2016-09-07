#include "MLPacketGen.h"

MLPayloadGen *createReqSetLoraConfigGen(uint8_t *frequency, uint8_t dataRate, uint8_t power, uint8_t wakeupInterval, uint8_t groupId,
        uint8_t optionFlags, uint8_t *optionData, uint8_t version) {
    MLReqSetLoraConfigGen *req = new MLReqSetLoraConfigGen(frequency, dataRate, power, wakeupInterval, groupId, optionFlags, optionData, version);
    return req;
}

MLPayloadGen* MLPayloadGen::createReqDataPayloadGen(uint16_t resInterval, uint8_t dataLen, uint8_t *data, uint8_t optionFlags, uint8_t *optionData, uint8_t version) {
    MLReqDataPayloadGen *req = new MLReqDataPayloadGen(resInterval, dataLen, data, optionFlags, optionData, version);
    return req;
}

MLPayloadGen *createResSetLoraConfigGen(uint8_t errorCode, uint8_t optionFlags, uint8_t *optionData, uint8_t version) {
    MLResSetLoraConfigGen *res = new MLResSetLoraConfigGen(errorCode, optionFlags, optionData, version);
    return res;
}

MLPayloadGen* MLPayloadGen::createResDataPayloadGen(uint8_t errorCode, uint8_t dataLen, uint8_t *data, uint8_t optionFlags, uint8_t *optionData, uint8_t version) {
    MLResDataPayloadGen *res = new MLResDataPayloadGen(errorCode, dataLen, data, optionFlags, optionData, version);
    return res;
}

MLPayloadGen* MLPayloadGen::createNotifyVindunoPayloadGen(uint8_t *apiKey, float soil_1, float soil_2, float soil_3, float soil_4, 
                float sysVoltage, float humidity, float temperature, float reserved, uint8_t optionFlags, uint8_t *optionData, uint8_t version) {
    MLNotifyVindunoPayloadGen *ntf = new MLNotifyVindunoPayloadGen(apiKey, soil_1, soil_2, soil_3, soil_4, sysVoltage, humidity, temperature, reserved, 
            optionFlags, optionData, version);
    return ntf;
}

MLReqSetLoraConfigGen::MLReqSetLoraConfigGen(uint8_t *frequency, uint8_t dataRate, uint8_t power, uint8_t wakeupInterval, uint8_t groupId, uint8_t optionFlags, uint8_t *optionData, uint8_t version) {
    _version = version;
    _cmdId = CMD_REQ_SET_LORA_CONFIG;
    memcpy(_frequency, frequency, ML_FREQUENCY_LEN);
    _dataRate = dataRate;
    _power = power;
    _wakeupInterval = wakeupInterval;
    _groupId = groupId;
    if (_optionFlags & 0x01) {
        _optionDataLen = 3;
        memcpy(_optionData, optionData, _optionDataLen);
    } else {
        _optionDataLen = 0;
    }
}

int MLReqSetLoraConfigGen::getPayload(uint8_t *payload) {
    uint8_t pos = 0;
    payload[pos++] = _version;
    payload[pos++] = _cmdId >> 8;
    payload[pos++] = _cmdId & 0x0F;
    memcpy(&payload[pos], _frequency, ML_FREQUENCY_LEN);
    pos += ML_FREQUENCY_LEN;
    payload[pos++] = _dataRate;
    payload[pos++] = _power;
    payload[pos++] = _wakeupInterval;
    payload[pos++] = _groupId;
    payload[pos++] = _optionFlags;
    if (_optionDataLen > 0)
        memcpy(&payload[pos], _optionData, _optionDataLen);
    pos += _optionDataLen;

    return pos;
}

MLReqDataPayloadGen::MLReqDataPayloadGen(uint16_t resInterval, uint8_t dataLen, uint8_t *data, uint8_t optionFlags, uint8_t *optionData, uint8_t version) {
    _version = version;
    _cmdId = CMD_REQ_DATA;
    _resInterval = resInterval;
    _dataLen = dataLen;
    memcpy(_data, data, _dataLen);
    _optionFlags = optionFlags;
    if (_optionFlags & 0x01) {
        _optionDataLen = 3;
        memcpy(_optionData, optionData, _optionDataLen);
    } else {
        _optionDataLen = 0;
    }
}

int MLReqDataPayloadGen::getPayload(uint8_t *payload) {
    uint8_t pos = 0;
    payload[pos++] = _version;
    payload[pos++] = _cmdId >> 8;
    payload[pos++] = _cmdId & 0x0F;
    payload[pos++] = _resInterval >> 8;
    payload[pos++] = _resInterval & 0x0F;
    payload[pos++] = _dataLen;
    memcpy(&payload[pos], _data, _dataLen);
    pos += _dataLen;
    payload[pos++] = _optionFlags;
    if (_optionDataLen > 0)
        memcpy(&payload[++pos], _optionData, _optionDataLen);
    pos += _optionDataLen;

    return pos;
}

MLResSetLoraConfigGen::MLResSetLoraConfigGen(uint8_t errorCode, uint8_t optionFlags, uint8_t *optionData, uint8_t version) {
    _version = version;
    _cmdId = CMD_RES_SET_LORA_CONFIG;
    _errorCode = errorCode;
    _optionFlags = optionFlags;
    if (_optionFlags & 0x01) {
        _optionDataLen = 3;
        memcpy(_optionData, optionData, _optionDataLen);
    } else {
        _optionDataLen = 0;
    }
}

int MLResSetLoraConfigGen::getPayload(uint8_t *payload) {
    uint8_t pos = 0;
    payload[pos++] = _version;
    payload[pos++] = _cmdId >> 8;
    payload[pos++] = _cmdId & 0x0F;
    payload[pos++] = _errorCode;
    payload[pos++] = _optionFlags;
    if (_optionDataLen > 0)
        memcpy(&payload[++pos], _optionData, _optionDataLen);
    pos += _optionDataLen;

    return pos;
}

MLResDataPayloadGen::MLResDataPayloadGen(uint8_t errorCode, uint8_t dataLen, uint8_t *data, uint8_t optionFlags, uint8_t *optionData, uint8_t version) {
    _version = version;
    _cmdId = CMD_RES_DATA;
    _errorCode = errorCode;
    _dataLen = dataLen;
    memcpy(_data, data, _dataLen);
    _optionFlags = optionFlags;
    if (_optionFlags & 0x01) {
        _optionDataLen = 3;
        memcpy(_optionData, optionData, _optionDataLen);
    } else {
        _optionDataLen = 0;
    }
}

int MLResDataPayloadGen::getPayload(uint8_t *payload) {
    uint8_t pos = 0;
    payload[pos++] = _version;
    payload[pos++] = _cmdId >> 8;
    payload[pos++] = _cmdId & 0x0F;
    payload[pos++] = _errorCode;
    payload[pos++] = _dataLen;
    memcpy(&payload[pos], _data, _dataLen);
    pos += _dataLen;
    payload[pos++] = _optionFlags;
    if (_optionDataLen > 0)
        memcpy(&payload[pos], _optionData, _optionDataLen);
    pos += _optionDataLen;

    return pos;
}

MLNotifyVindunoPayloadGen::MLNotifyVindunoPayloadGen(uint8_t *apiKey, float soil_1, float soil_2, float soil_3, float soil_4, 
                float sysVoltage, float humidity, float temperature, float reserved, uint8_t optionFlags, uint8_t *optionData, uint8_t version) {
    _version = version;
    _cmdId = CMD_NTF_UPLOAD_VINDUINO_FIELD;
    memcpy(_apiKey, apiKey, VINDUNO_API_KEY_LEN);
    _soil_1 = soil_1;
    _soil_2 = soil_2;
    _soil_3 = soil_3;
    _soil_4 = soil_4;
    _sysVoltage = sysVoltage;
    _humidity = humidity;
    _temperature = temperature;
    _reserved = reserved;
    _optionFlags = optionFlags;
    if (_optionFlags & 0x01) {
        _optionDataLen = 3;
        memcpy(_optionData, optionData, _optionDataLen);
    } else {
        _optionDataLen = 0;
    }
}

int MLNotifyVindunoPayloadGen::getPayload(uint8_t *payload) {
    uint8_t pos = 0;
    payload[pos++] = _version;
    payload[pos++] = _cmdId >> 8;
    payload[pos++] = _cmdId & 0x0F;
    memcpy(&payload[pos], _apiKey, VINDUNO_API_KEY_LEN);
    pos += VINDUNO_API_KEY_LEN;
    payload[pos++] = ((uint32_t)_soil_1 & 0xFF000000) >> 24;
    payload[pos++] = ((uint32_t)_soil_1 & 0x00FF0000) >> 16;
    payload[pos++] = ((uint32_t)_soil_1 & 0x0000FF00) >> 8;
    payload[pos++] = ((uint32_t)_soil_1 & 0x000000FF);
    payload[pos++] = ((uint32_t)_soil_2 & 0xFF000000) >> 24;
    payload[pos++] = ((uint32_t)_soil_2 & 0x00FF0000) >> 16;
    payload[pos++] = ((uint32_t)_soil_2 & 0x0000FF00) >> 8;
    payload[pos++] = ((uint32_t)_soil_2 & 0x000000FF);
    payload[pos++] = ((uint32_t)_soil_3 & 0xFF000000) >> 24;
    payload[pos++] = ((uint32_t)_soil_3 & 0x00FF0000) >> 16;
    payload[pos++] = ((uint32_t)_soil_3 & 0x0000FF00) >> 8;
    payload[pos++] = ((uint32_t)_soil_3 & 0x000000FF);
    payload[pos++] = ((uint32_t)_soil_4 & 0xFF000000) >> 24;
    payload[pos++] = ((uint32_t)_soil_4 & 0x00FF0000) >> 16;
    payload[pos++] = ((uint32_t)_soil_4 & 0x0000FF00) >> 8;
    payload[pos++] = ((uint32_t)_soil_4 & 0x000000FF);
    payload[pos++] = ((uint32_t)_sysVoltage & 0xFF000000) >> 24;
    payload[pos++] = ((uint32_t)_sysVoltage & 0x00FF0000) >> 16;
    payload[pos++] = ((uint32_t)_sysVoltage & 0x0000FF00) >> 8;
    payload[pos++] = ((uint32_t)_sysVoltage & 0x000000FF);
    payload[pos++] = ((uint32_t)_humidity & 0xFF000000) >> 24;
    payload[pos++] = ((uint32_t)_humidity & 0x00FF0000) >> 16;
    payload[pos++] = ((uint32_t)_humidity & 0x0000FF00) >> 8;
    payload[pos++] = ((uint32_t)_humidity & 0x000000FF);
    payload[pos++] = ((uint32_t)_temperature & 0xFF000000) >> 24;
    payload[pos++] = ((uint32_t)_temperature & 0x00FF0000) >> 16;
    payload[pos++] = ((uint32_t)_temperature & 0x0000FF00) >> 8;
    payload[pos++] = ((uint32_t)_temperature & 0x000000FF);
    payload[pos++] = ((uint32_t)_reserved & 0xFF000000) >> 24;
    payload[pos++] = ((uint32_t)_reserved & 0x00FF0000) >> 16;
    payload[pos++] = ((uint32_t)_reserved & 0x0000FF00) >> 8;
    payload[pos++] = ((uint32_t)_reserved & 0x000000FF);
    payload[pos++] = _optionFlags;
    if (_optionDataLen > 0)
        memcpy(&payload[pos], _optionData, _optionDataLen);
    pos += _optionDataLen;
    return pos;
}

MLPacketGen::MLPacketGen(uint8_t ackBit, uint8_t receiverFlag, uint8_t packetType, uint8_t direction, uint64_t receiverID, uint64_t senderID, uint8_t version) {
    _ackBit = ackBit;
    _receiverFlag = receiverFlag;
    _packetType = packetType;
    _direction = direction;
    _receiverID = receiverID;
    _senderID = senderID;
    _version = version;
    _mlPayloadGen = NULL;
}

void MLPacketGen::setMLPayloadGen(MLPayloadGen *mlpayloadGen) {
    _mlPayloadGen = mlpayloadGen;
}

int MLPacketGen::getMLPayload(uint8_t *payload) {
    if (_mlPayloadGen == NULL)
        return 0;
    else
        return _mlPayloadGen->getPayload(payload);
}

int MLPacketGen::getMLPacket(uint8_t *mlpacket) {
    uint8_t pos = 0;
    uint8_t mlpayload[84];

    mlpacket[0] = 0xFB;
    mlpacket[1] = 0xFC;
    mlpacket[2] = _version;
    mlpacket[4] = (_direction & 0x01) << 3 | (_packetType & 0x01) << 2 | (_receiverFlag & 0x01) << 1 | (_ackBit & 0x01);
    mlpacket[5] = (_receiverID & 0xFF00000000000000) >> 56;
    mlpacket[6] = (_receiverID & 0x00FF000000000000) >> 48;
    mlpacket[7] = (_receiverID & 0x0000FF0000000000) >> 40;
    mlpacket[8] = (_receiverID & 0x000000FF00000000) >> 32;
    mlpacket[9] = (_receiverID & 0x00000000FF000000) >> 24;
    mlpacket[10] = (_receiverID & 0x0000000000FF0000) >> 16;
    mlpacket[11] = (_receiverID & 0x000000000000FF00) >> 8;
    mlpacket[12] = _receiverID & 0x00000000000000FF;
    pos = 13;
    if (_direction == 1) {
        mlpacket[13] = (_senderID & 0xFF00000000000000) >> 56;
        mlpacket[14] = (_senderID & 0x00FF000000000000) >> 48;
        mlpacket[15] = (_senderID & 0x0000FF0000000000) >> 40;
        mlpacket[16] = (_senderID & 0x000000FF00000000) >> 32;
        mlpacket[17] = (_senderID & 0x00000000FF000000) >> 24;
        mlpacket[18] = (_senderID & 0x0000000000FF0000) >> 16;
        mlpacket[19] = (_senderID & 0x000000000000FF00) >> 8;
        mlpacket[20] = _senderID & 0x00000000000000FF;
        pos = 21;
    }
    uint8_t payloadLen = getMLPayload(mlpayload);
    if (payloadLen <=0)
        return -1;
    uint8_t totalLen = payloadLen + pos + 2;
    mlpacket[3] = totalLen;
    mlpacket[pos] = getCrc(mlpacket, pos);
    memcpy(&mlpacket[pos+1], mlpayload, payloadLen);
    mlpacket[totalLen-1] = getCrc(mlpacket, totalLen-1);
    return totalLen;
}

