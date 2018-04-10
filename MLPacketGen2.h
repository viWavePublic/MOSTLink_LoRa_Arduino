#ifndef __MLPACKETGEN2_H
#define __MLPACKETGEN2_H

#include "MLPacketGen.h"
/*
 ansType
    BIT0 : 0:Device response, 1:User response
    BIT1 : 0: response once, 1 : Need to response Regularly
    BIT2 : 1: Response need to include GPS value
    BIT3 : 1 : Response need to include Battery Level
    BIT4 : 1 : Stop reqularly response
    BIT5 : 0 : Response location directly, 1 : Response location after a delay time
 
 action
    BIT0: 0: No action, 1 : LIGHT
    BIT1: 0: No action, 1 : VIBRATION
*/

class MLReqNDCallPayloadGen : public MLPayloadGen {
public:
    MLReqNDCallPayloadGen(uint8_t ansType = 44, uint8_t action = 3, uint16_t ansTypeEx_ResInterval = 10, uint16_t ansTypeEx_DelayTime = 30);
    int getPayload(uint8_t *payload);
    uint8_t getAnsType() { return _ansType; }
    uint8_t getAction() { return _action; }
    uint16_t getResInterval() { return _resInterval; }
    uint16_t getDelayTime() { return _delayTime; }

private:
    uint8_t _ansType;
    uint8_t _action;
    uint16_t _resInterval;
    uint16_t _delayTime;
};

class MLAnsNDCallPayloadGen : public MLPayloadGen {
public:
    MLAnsNDCallPayloadGen(uint32_t dateTime = 0, uint8_t statusGPS = 0, uint32_t lng = 0, uint32_t lat = 0, uint8_t batteryLevel = 0);
    void setPayload(const uint8_t *payload, int szPayload);
    int getPayload(uint8_t *payload);
    uint32_t getDataTime() { return _dateTime; }
    uint8_t getStatusGPS() { return _statusGPS; }
    uint8_t getTypeReport() { return _typeReport; }
    uint8_t getTypeGPS() { return _typeGPS; }

    uint8_t getBatteryLevel() { return _batteryLevel; }
    double getLat() { return _loc.latitude / 1000000.0; }
    double getLng() { return _loc.longtitude / 1000000.0; }
    
protected:
    uint32_t _dateTime;
    union {
        uint8_t _statusGPS;
        struct {
            uint8_t _typeReport:5;
            uint8_t _typeGPS:3;
        };
    };
    mllocation _loc;
    uint8_t _batteryLevel;
};

// Tracker node: SOS related
class MLReqSOSPayloadGen : public MLAnsNDCallPayloadGen {
public:
    MLReqSOSPayloadGen(uint32_t dateTime = 0, uint8_t statusGPS = 0, uint32_t lng = 0, uint32_t lat = 0, uint8_t batteryLevel = 0)
    : MLAnsNDCallPayloadGen(dateTime, statusGPS, lng, lat, batteryLevel)
    {
        _cmdId = CMD_REQ_SOS;
    }
};

class MLAnsSOSPayloadGen : public MLPayloadGen {
public:
    MLAnsSOSPayloadGen(uint8_t *devID = NULL, uint8_t conditionFlag = 0);
    void setPayload(const uint8_t *payload, int szPayload);
    int getPayload(uint8_t *payload);
    uint8_t* getDevID() { return _responserDevID; }
    uint8_t getConditionFlag() { return _conditionFlag; }
    
private:
    
    uint8_t _responserDevID[8];
    uint8_t _conditionFlag;
};


#endif /* __MLPACKETGEN2_H */
