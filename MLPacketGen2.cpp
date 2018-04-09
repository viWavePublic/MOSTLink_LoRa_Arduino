#include "MLPacketGen2.h"
#include "MLutility.h"

////////////////////////////////////////////////////////////////////////////

MLReqNDCallPayloadGen::MLReqNDCallPayloadGen(uint8_t ansType, uint8_t action, uint16_t ansTypeEx_ResInterval, uint16_t ansTypeEx_DelayTime) : MLPayloadGen(CMD_REQ_NDCALL)
{
    _ansType = ansType;
    _action = action;
    _resInterval = ansTypeEx_ResInterval;
    _delayTime = ansTypeEx_DelayTime;
}

int MLReqNDCallPayloadGen::getPayload(uint8_t *payload)
{
    // prefix
    int pos = getPayloadPrefix(payload);
    
    payload[pos++] = _ansType;
    payload[pos++] = _action;
    
    // bit1, bit5, add optional extend data field
    if ((_ansType & 0x02) || (_ansType & 0x20))
    {
        uint16_t optResInterval = 0;
        uint16_t optDelayTime = 0;
        if (_ansType & 0x02)
            optResInterval = _resInterval;

        if (_ansType & 0x20)
            optDelayTime = _delayTime;
        
        payload[pos++] = optResInterval & 0xFF;
        payload[pos++] = optResInterval >> 8;

        payload[pos++] = optDelayTime & 0xFF;
        payload[pos++] = optDelayTime >> 8;
    }
    
    // postfix
    pos = getPayloadPostfix(payload, pos);
    return pos;
}

////////////////////////////////////////////////////////////////////////////

MLAnsNDCallPayloadGen::MLAnsNDCallPayloadGen(uint32_t dateTime, uint8_t statusGPS, uint32_t lng, uint32_t lat, uint8_t batteryLevel)
: MLPayloadGen(CMD_ANS_NDCALL)
{
    _dateTime = dateTime;
    _statusGPS = statusGPS;
    _loc.latitude = lat;
    _loc.longtitude = lng;
    _batteryLevel = batteryLevel;
}

// set member variable by decode payload
void MLAnsNDCallPayloadGen::setPayload(const uint8_t *payload, int szPayload)
{
    int pos = 3;
    _dateTime = MLutility::convertHexToDec(payload + pos, 4, true);
    pos += 4;
    
    _statusGPS = payload[pos];
    pos ++;

    _loc.longtitude = MLutility::convertHexToDec(payload + pos, 4, true);
    pos += 4;
    
    _loc.latitude = MLutility::convertHexToDec(payload + pos, 4, true);
    pos += 4;
    
    _batteryLevel = payload[pos];
}

int MLAnsNDCallPayloadGen::getPayload(uint8_t *payload)
{
    // do nothing
    return 0;
}

////////////////////////////////////////////////////////////////////////////
// Response Device ID (1st LoRa module Mac address of gateway)
// Condition Flag: BIT0 : FROCE_STOP 1 : Tracker should force stop REQ_SOS
MLAnsSOSPayloadGen::MLAnsSOSPayloadGen(uint8_t *devID, uint8_t conditionFlag)
: MLPayloadGen(CMD_ANS_SOS)
{
    if (NULL != devID) {
        memcpy(_responserDevID, devID, 8);
    }
    _conditionFlag = conditionFlag;
}

void MLAnsSOSPayloadGen::setPayload(const uint8_t *payload, int szPayload)
{
    int pos = 3;
    memcpy(_responserDevID, payload + pos, 8);
    pos += 8;
    _conditionFlag = payload[pos];
}

int MLAnsSOSPayloadGen::getPayload(uint8_t *payload)
{
    // prefix
    int pos = getPayloadPrefix(payload);
    
    memcpy(payload + pos, _responserDevID, 8);
    pos += 8;
    
    payload[pos] = _conditionFlag;
    pos++;
    
    // postfix
    pos = getPayloadPostfix(payload, pos);
    return pos;
}
