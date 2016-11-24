#include "MLPacketGen.h"

MLReqSetLoraConfigGen::MLReqSetLoraConfigGen(uint8_t *frequency, uint8_t dataRate, uint8_t power, uint8_t wakeupInterval, uint8_t groupId) : MLPayloadGen(CMD_REQ_SET_LORA_CONFIG) {
    memcpy(_frequency, frequency, ML_FREQUENCY_LEN);
    _dataRate = dataRate;
    _power = power;
    _wakeupInterval = wakeupInterval;
    _groupId = groupId;
}

int MLReqSetLoraConfigGen::getPayload(uint8_t *payload) {
    uint8_t pos = 0;
    payload[pos++] = _version;
    payload[pos++] = _cmdId & 0xFF;
    payload[pos++] = _cmdId >> 8;
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

MLReqDataPayloadGen::MLReqDataPayloadGen(uint16_t resInterval, uint8_t dataLen, uint8_t *data) : MLPayloadGen(CMD_REQ_DATA) {
    _resInterval = resInterval;
    _dataLen = dataLen;
    memcpy(_data, data, _dataLen);
}

int MLReqDataPayloadGen::getPayload(uint8_t *payload) {
    uint8_t pos = 0;
    payload[pos++] = _version;
    payload[pos++] = _cmdId & 0xFF;
    payload[pos++] = _cmdId >> 8;
    payload[pos++] = _resInterval & 0xFF;
    payload[pos++] = _resInterval >> 8;
    payload[pos++] = _dataLen;
    memcpy(&payload[pos], _data, _dataLen);
    pos += _dataLen;
    payload[pos++] = _optionFlags;
    if (_optionDataLen > 0)
        memcpy(&payload[++pos], _optionData, _optionDataLen);
    pos += _optionDataLen;

    return pos;
}

MLNotifyLocationGen::MLNotifyLocationGen(uint32_t dateTime, mllocation location, uint8_t notifyType, uint8_t gpsStatus) : MLPayloadGen(CMD_NOTIFY_LOCATION) {
    _dateTime = dateTime;
    _location.longtitude = location.longtitude;
    _location.latitude = location.latitude;
    _gpsStatus = gpsStatus;
}

int MLNotifyLocationGen::getPayload(uint8_t *payload) {
    uint8_t pos = 0;
    payload[pos++] = _version;
    payload[pos++] = _cmdId & 0xFF;
    payload[pos++] = _cmdId >> 8;
    memcpy(payload + pos, &_dateTime, 4);
    pos = pos + 4;
    memcpy(payload + pos, &_location, 8);
    pos = pos + 8;
    payload[pos++] = _notifyType;
    payload[pos++] = _gpsStatus;
    payload[pos++] = _optionFlags;
    if (_optionDataLen > 0)
        memcpy(&payload[++pos], _optionData, _optionDataLen);
    pos += _optionDataLen;

    return pos;
}

MLReqLocationGen::MLReqLocationGen(int32_t resInterval) : MLPayloadGen(CMD_REQ_LOCATION) {
    _resInterval = resInterval;
}

int MLReqLocationGen::getPayload(uint8_t *payload) {
    uint8_t pos = 0;
    payload[pos++] = _version;
    payload[pos++] = _cmdId & 0xFF;
    payload[pos++] = _cmdId >> 8;
    memcpy(payload + pos, &_resInterval, 4);
    pos = pos + 4;
    payload[pos++] = _optionFlags;
    if (_optionDataLen > 0)
        memcpy(&payload[++pos], _optionData, _optionDataLen);
    pos += _optionDataLen;

    return pos;
}

MLSetGeoFenceConfigGen::MLSetGeoFenceConfigGen(uint16_t geofRadius, uint16_t resInterval) : MLPayloadGen(CMD_SET_CONFIG_GEOF) {
    _geofRadius = geofRadius;
    _resInterval = resInterval;
}

int MLSetGeoFenceConfigGen::getPayload(uint8_t *payload) {
    uint8_t pos = 0;
    payload[pos++] = _version;
    payload[pos++] = _cmdId & 0xFF;
    payload[pos++] = _cmdId >> 8;
    payload[pos++] = _geofRadius & 0xFF;
    payload[pos++] = _geofRadius >> 8;
    payload[pos++] = _resInterval & 0xFF;
    payload[pos++] = _resInterval >> 8;
    payload[pos++] = _optionFlags;
    if (_optionDataLen > 0)
        memcpy(&payload[++pos], _optionData, _optionDataLen);
    pos += _optionDataLen;

    return pos;
}

int MLGetGeoFenceConfigGen::getPayload(uint8_t *payload) {
    uint8_t pos = 0;
    payload[pos++] = _version;
    payload[pos++] = _cmdId & 0xFF;
    payload[pos++] = _cmdId >> 8;
    payload[pos++] = _optionFlags;
    if (_optionDataLen > 0)
        memcpy(&payload[++pos], _optionData, _optionDataLen);
    pos += _optionDataLen;

    return pos;
}

MLResSetLoraConfigGen::MLResSetLoraConfigGen(uint8_t errorCode) : MLPayloadGen(CMD_RES_SET_LORA_CONFIG) {
    _errorCode = errorCode;
}

int MLResSetLoraConfigGen::getPayload(uint8_t *payload) {
    uint8_t pos = 0;
    payload[pos++] = _version;
    payload[pos++] = _cmdId & 0xFF;
    payload[pos++] = _cmdId >> 8;
    payload[pos++] = _errorCode;
    payload[pos++] = _optionFlags;
    if (_optionDataLen > 0)
        memcpy(&payload[++pos], _optionData, _optionDataLen);
    pos += _optionDataLen;

    return pos;
}

MLResDataPayloadGen::MLResDataPayloadGen(uint8_t errorCode, uint8_t dataLen, uint8_t *data) : MLPayloadGen(CMD_RES_DATA) {
    _errorCode = errorCode;
    _dataLen = dataLen;
    memcpy(_data, data, _dataLen);
}

int MLResDataPayloadGen::getPayload(uint8_t *payload) {
    uint8_t pos = 0;
    payload[pos++] = _version;
    payload[pos++] = _cmdId & 0xFF;
    payload[pos++] = _cmdId >> 8;
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

MLRetConfigGeofGen::MLRetConfigGeofGen(uint16_t geofRadius, uint16_t resInterval, mllocation location) : MLPayloadGen(CMD_RET_CONFIG_GEOF) {
    _geofRadius = geofRadius;
    _resInterval = resInterval;
    _location.longtitude = location.longtitude;
    _location.latitude = location.latitude;
}

int MLRetConfigGeofGen::getPayload(uint8_t *payload) {
    uint8_t pos = 0;
    payload[pos++] = _version;
    payload[pos++] = _cmdId & 0xFF;
    payload[pos++] = _cmdId >> 8;
    payload[pos++] = _geofRadius & 0xFF;
    payload[pos++] = _geofRadius >> 8;
    payload[pos++] = _resInterval & 0xFF;
    payload[pos++] = _resInterval >> 8;
    memcpy(payload + pos, &_location, 8);
    pos = pos + 8;

    payload[pos++] = _optionFlags;
    if (_optionDataLen > 0)
        memcpy(&payload[pos], _optionData, _optionDataLen);
    pos += _optionDataLen;
    return pos;
}

MLNotifyVindunoPayloadGen::MLNotifyVindunoPayloadGen(uint8_t *apiKey, float soil_1, float soil_2, float soil_3, float soil_4, 
                float sysVoltage, float humidity, float temperature, float reserved) : MLPayloadGen(CMD_NTF_UPLOAD_VINDUINO_FIELD) {
    memcpy(_apiKey, apiKey, VINDUNO_API_KEY_LEN);
    _soil_1 = soil_1;
    _soil_2 = soil_2;
    _soil_3 = soil_3;
    _soil_4 = soil_4;
    _sysVoltage = sysVoltage;
    _humidity = humidity;
    _temperature = temperature;
    _reserved = reserved;
}

int MLNotifyVindunoPayloadGen::getPayload(uint8_t *payload) {
    uint8_t pos = 0;
    payload[pos++] = _version;
    payload[pos++] = _cmdId & 0xFF;
    payload[pos++] = _cmdId >> 8;
    memcpy(&payload[pos], _apiKey, VINDUNO_API_KEY_LEN);
    pos += VINDUNO_API_KEY_LEN;
    
    // assign 8 data to payload
    float arrDat[8] = {_soil_1, _soil_2, _soil_3, _soil_4, _sysVoltage, _humidity, _temperature, _reserved};
    int i = 0;
    for (i = 0; i < 8; i++) {
        memcpy(payload + pos, arrDat + i, 4);
        pos = pos + 4;
    }
    
    payload[pos++] = _optionFlags;
    if (_optionDataLen > 0)
        memcpy(&payload[pos], _optionData, _optionDataLen);
    pos += _optionDataLen;
    return pos;
}

MLPacketGen::MLPacketGen(uint8_t version) {
    _ackBit = MAX_VAL_UINT8;
    _receiverFlag = MAX_VAL_UINT8;
    _packetType = MAX_VAL_UINT8;
    _direction = MAX_VAL_UINT8;
    memset(_id, 0, ML_PK_ID_SIZE);
    _version = version;
    _mlPayloadGen = NULL;
}

MLPacketGen::MLPacketGen(uint8_t ackBit, uint8_t receiverFlag, uint8_t packetType, uint8_t direction, uint8_t *id, uint8_t version) {
    _ackBit = ackBit;
    _receiverFlag = receiverFlag;
    _packetType = packetType;
    _direction = direction;
    memcpy(_id, id, ML_PK_ID_SIZE);
    _version = version;
    _mlPayloadGen = NULL;
}

void MLPacketGen::setMLPacket(uint8_t ackBit, uint8_t receiverFlag, uint8_t packetType, uint8_t direction, uint8_t *id, uint8_t version) {
    _ackBit = ackBit;
    _receiverFlag = receiverFlag;
    _packetType = packetType;
    _direction = direction;
    memcpy(_id, id, ML_PK_ID_SIZE);
    _version = version;
}

void MLPacketGen::setMLPayloadGen(MLPayloadGen *mlpayloadGen) {
    if (_mlPayloadGen != NULL)
        delete _mlPayloadGen;
    
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
    uint8_t mlpayload[ML_MAX_PAYLOAD_SIZE];

    mlpacket[ML_PK_PREAMBLE_1_POS] = 0xFB;
    mlpacket[ML_PK_PREAMBLE_2_POS] = 0xFC;
    mlpacket[ML_PK_VERSION_POS] = _version;
    mlpacket[ML_PK_FLAGS_POS] = (_direction & 0x01) << 3 | (_packetType & 0x01) << 2 | (_receiverFlag & 0x01) << 1 | (_ackBit & 0x01);
    memcpy(&mlpacket[ML_PK_ID_POS], _id, ML_PK_ID_SIZE);
    pos = ML_PK_ID_POS+ML_PK_ID_SIZE;
    uint8_t payloadLen = getMLPayload(mlpayload);
    if (payloadLen <=0)
        return -1;
    uint8_t totalLen = payloadLen + ML_PK_HEADER_SIZE;
    mlpacket[3] = totalLen;
    mlpacket[pos] = getCrc(mlpacket, pos);
    memcpy(&mlpacket[pos+1], mlpayload, payloadLen);
    mlpacket[totalLen-1] = getCrc(mlpacket, totalLen-1);
    return totalLen;
}

