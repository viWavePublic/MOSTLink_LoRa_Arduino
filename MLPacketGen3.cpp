////////////////////////////////////////////////////////////////////////////
// MOSTLink v1.5 for LT-601 tracker
//
//    payload version 0x0C, no more optional-flag at payload-end
//
////////////////////////////////////////////////////////////////////////////

#include "MLPacketGen3.h"

////////////////////////////////////////////////////////////////////////////

MLReqLocationPayloadGen::MLReqLocationPayloadGen(uint8_t reportType, uint8_t action, uint16_t ansTypeEx_ResInterval, uint16_t ansTypeEx_DelayTime)
: MLPayloadGen(CMD_REQ_LOCATION, 0, NULL, 0x0C)
{
    _reportType = reportType;
    _action = action;
    _resInterval = ansTypeEx_ResInterval;
    _delayTime = ansTypeEx_DelayTime;
}

int MLReqLocationPayloadGen::getPayload(uint8_t *payload)
{
    // prefix
    int pos = getPayloadPrefix(payload);
    
    payload[pos++] = _reportType;
    payload[pos++] = _action;
    
    // bit1, bit5, add optional extend data field
    if ((_reportType & 0x02) || (_reportType & 0x20))
    {
        uint16_t optResInterval = 0;
        uint16_t optDelayTime = 0;
        if (_reportType & 0x02)
            optResInterval = _resInterval;

        if (_reportType & 0x20)
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

MLAnsAlarmPayloadGen::MLAnsAlarmPayloadGen(uint8_t *idAnswerer, uint8_t conditionFlag)
: MLPayloadGen(CMD_ANS_ALARM, 0, NULL, 0x0C)
{
    if (idAnswerer != NULL) {
        memcpy(_idAnswerer, idAnswerer, ML_PK_ID_SIZE);
    }
    _conditionFlag = conditionFlag;
}

int MLAnsAlarmPayloadGen::getPayload(uint8_t *payload)
{
    // prefix
    int pos = getPayloadPrefix(payload);
    
    memcpy(payload + pos, _idAnswerer, ML_PK_ID_SIZE);
    pos += 8;
    
    payload[pos++] = _conditionFlag;
   
    // postfix
    pos = getPayloadPostfix(payload, pos);
    return pos;
}
////////////////////////////////////////////////////////////////////////////
// SetDevice Command: "L2(param)" + "\r\n" as (0D0A)
MLReqGtrCommandPayloadGen::MLReqGtrCommandPayloadGen(char *cmdParam, bool bPresetL2)
: MLPayloadGen(CMD_REQ_GTR_COMMAND, 0, NULL, 0x0C)
{
    char *strFmt;
    if (bPresetL2) {       // L2(XXX)\r\n
        strFmt = "L2(%s)\r\n";
    }
    else {              // cmd\r\n
        strFmt = "%s\r\n";
    }
    sprintf((char*)_data, strFmt, cmdParam);
    _dataLen = strlen((char*)_data);
}

int MLReqGtrCommandPayloadGen::getPayload(uint8_t *payload)
{
    // prefix
    int pos = getPayloadPrefix(payload);
    
    payload[pos] = _dataLen;
    pos++;

    memcpy(payload + pos, _data, _dataLen);
    pos += _dataLen;
        
    // postfix
    pos = getPayloadPostfix(payload, pos);
    return pos;
    
}

////////////////////////////////////////////////////////////////////////////
// set member variable by decode payload
void MLReportLocationPayloadGen::setPayload(const uint8_t *payload, int szPayload)
{
    int pos = 3;
  
    _loc.longtitude = MLPayloadGen::convertHexToDec(payload + pos, 4, true);
    pos += 4;
    
    _loc.latitude = MLPayloadGen::convertHexToDec(payload + pos, 4, true);
    pos += 4;

    _statusGPS = payload[pos];
    pos ++;

    _batteryLevel = payload[pos];
    pos ++;
    
    _dateTime = MLPayloadGen::convertHexToDec(payload + pos, 4, true);
    pos += 4;
}


////////////////////////////////////////////////////////////////////////////
// set member variable by decode payload
void MLReportBeaconPayloadGen::setPayload(const uint8_t *payload, int szPayload)
{
    int pos = 3;
    memcpy(_uuid, payload + pos, 20);
    pos += 20;
    
    _type = payload[pos];
    pos++;
    
    _rssi = payload[pos];
    pos++;

    _txPower = payload[pos];
    pos++;
    
    _batteryLevel = payload[pos];
    pos++;
}

