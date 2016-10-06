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

MLPayloadGen* MLPayloadGen::createNotifyLocationGen(uint32_t dateTime, mllocation location, uint8_t notifyType, uint8_t gpsStatus, uint8_t optionFlags, uint8_t *optionData, uint8_t version) {
    MLNotifyLocationGen *notifyLocation = new MLNotifyLocationGen(dateTime, location, notifyType, gpsStatus, optionFlags, optionData, version);
    return notifyLocation;
}

MLPayloadGen* MLPayloadGen::createReqLocation(int32_t resInterval, uint8_t optionFlags, uint8_t *optionData, uint8_t version) {
    MLReqLocationGen *reqLocation = new MLReqLocationGen(resInterval, optionFlags, optionData, version);
    return reqLocation;
}

MLPayloadGen* MLPayloadGen::createSetGeoFenceConfig(uint16_t geofRadius, uint16_t resInterval, uint8_t optionFlags, uint8_t *optionData, uint8_t version) {
    MLSetGeoFenceConfigGen *setGeofConf = new MLSetGeoFenceConfigGen(geofRadius, resInterval, optionFlags, optionData, version);
    return setGeofConf;
}

MLPayloadGen* MLPayloadGen::createGetGeoFenceConfig(uint8_t optionFlags, uint8_t *optionData, uint8_t version) {
    MLGetGeoFenceConfigGen *getGeofConf = new MLGetGeoFenceConfigGen(optionFlags, optionData, version);
    return getGeofConf;
}

MLPayloadGen* MLPayloadGen::createResSetLoraConfigGen(uint8_t errorCode, uint8_t optionFlags, uint8_t *optionData, uint8_t version) {
    MLResSetLoraConfigGen *res = new MLResSetLoraConfigGen(errorCode, optionFlags, optionData, version);
    return res;
}

MLPayloadGen* MLPayloadGen::createRetConfigGeof(uint16_t geofRadius, uint16_t resInterval, mllocation location, uint8_t optionFlags, uint8_t *optionData, uint8_t version) {
    MLRetConfigGeofGen *ret = new MLRetConfigGeofGen(geofRadius, resInterval, location, optionFlags, optionData, version);
    return ret;
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

MLNotifyLocationGen::MLNotifyLocationGen(uint32_t dateTime, mllocation location, uint8_t notifyType, uint8_t gpsStatus, uint8_t optionFlags, uint8_t *optionData, uint8_t version) {
    _version = version;
    _cmdId = CMD_NOTIFY_LOCATION;
    _dateTime = dateTime;
    _location.longtitude = location.longtitude;
    _location.latitude = location.latitude;
    _gpsStatus = gpsStatus;
    _optionFlags = optionFlags;
    if (_optionFlags & 0x01) {
        _optionDataLen = 3;
        memcpy(_optionData, optionData, _optionDataLen);
    } else {
        _optionDataLen = 0;
    }
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

MLReqLocationGen::MLReqLocationGen(int32_t resInterval, uint8_t optionFlags, uint8_t *optionData, uint8_t version) {
    _version = version;
    _cmdId = CMD_REQ_LOCATION;
    _resInterval = resInterval;
    _optionFlags = optionFlags;
    if (_optionFlags & 0x01) {
        _optionDataLen = 3;
        memcpy(_optionData, optionData, _optionDataLen);
    } else {
        _optionDataLen = 0;
    }
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

MLSetGeoFenceConfigGen::MLSetGeoFenceConfigGen(uint16_t geofRadius, uint16_t resInterval, uint8_t optionFlags, uint8_t *optionData, uint8_t version) {
    _version = version;
    _cmdId = CMD_SET_CONFIG_GEOF;
    _geofRadius = geofRadius;
    _resInterval = resInterval;
    _optionFlags = optionFlags;
    if (_optionFlags & 0x01) {
        _optionDataLen = 3;
        memcpy(_optionData, optionData, _optionDataLen);
    } else {
        _optionDataLen = 0;
    }
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

MLGetGeoFenceConfigGen::MLGetGeoFenceConfigGen(uint8_t optionFlags, uint8_t *optionData, uint8_t version) {
    _version = version;
    _cmdId = CMD_GET_CONFIG_GEOF;
    _optionFlags = optionFlags;
    if (_optionFlags & 0x01) {
        _optionDataLen = 3;
        memcpy(_optionData, optionData, _optionDataLen);
    } else {
        _optionDataLen = 0;
    }
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
    payload[pos++] = _cmdId & 0xFF;
    payload[pos++] = _cmdId >> 8;
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

MLRetConfigGeofGen::MLRetConfigGeofGen(uint16_t geofRadius, uint16_t resInterval, mllocation location, uint8_t optionFlags, uint8_t *optionData, uint8_t version) {
    _version = version;
    _cmdId = CMD_RET_CONFIG_GEOF;
    _geofRadius = geofRadius;
    _resInterval = resInterval;
    _location.longtitude = location.longtitude;
    _location.latitude = location.latitude;
    _optionFlags = optionFlags;
    if (_optionFlags & 0x01) {
        _optionDataLen = 3;
        memcpy(_optionData, optionData, _optionDataLen);
    } else {
        _optionDataLen = 0;
    }
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

MLPacketGen::MLPacketGen(uint8_t ackBit, uint8_t receiverFlag, uint8_t packetType, uint8_t direction, uint8_t *id, uint8_t version) {
    _ackBit = ackBit;
    _receiverFlag = receiverFlag;
    _packetType = packetType;
    _direction = direction;
    memcpy(_id, id, ML_PK_ID_SIZE);
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
