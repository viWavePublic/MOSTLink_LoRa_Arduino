#ifndef __MLPACKETGEN3_H
#define __MLPACKETGEN3_H

#include "MLPacketGen2.h"

/////////////////////////////////////////
// Downlink: for gateway
/////////////////////////////////////////
/*
 reportType
 BIT0 - BIT1 : ( One time report : 0x01, Periodic report : 0x02 : Motion Mode : 0x03 )
 BIT3 - BIT4: Motion mode sensitivity ( Low : 0x01, Medium : 0x02 : High : 0x03 )
 BIT5 : Thist bit available when One time report. 0 : Response location directly, 1 : Response location after a delay time
 
 *BEACON priority or both"
 
 action
    BIT0: 0: Reserve
    BIT1: 0: No action, 1 : VIBRATION
*/

class MLReqLocationPayloadGen : public MLPayloadGen {
public:
    MLReqLocationPayloadGen(uint8_t reportType = 0x01, uint8_t action = 3, uint16_t ansTypeEx_ResInterval = 10, uint16_t ansTypeEx_DelayTime = 30);
    int getPayload(uint8_t *payload);
    uint8_t getReportType() { return _reportType; }
    uint8_t getAction() { return _action; }
    uint16_t getResInterval() { return _resInterval; }
    uint16_t getDelayTime() { return _delayTime; }

private:
    uint8_t _reportType;
    uint8_t _action;
    uint16_t _resInterval;
    uint16_t _delayTime;
};

/*
 CONDITION_FLAG
    BIT0 : FROCE_HELP_STOP 1 : Tracker should force stop REQ_ALARM_GPS Help
    BIT1 : FORCE_MANDOWN_STOP 1 : Tracker should force stop REQ_ALARM_GPS mandown
 */

class MLAnsAlarmPayloadGen : public MLPayloadGen {
public:
    MLAnsAlarmPayloadGen(uint8_t *idAnswerer, uint8_t conditionFlag = 0x01);
    int getPayload(uint8_t *payload);
    uint8_t getConditionFlag() { return _conditionFlag; }
    
private:
    uint8_t _idAnswerer[ML_PK_ID_SIZE];
    uint8_t _conditionFlag;

};

// CMD_REQ_GTR_COMMAND
class MLReqGtrCommandPayloadGen : public MLPayloadGen {
public:
    MLReqGtrCommandPayloadGen(char *cmdParam, bool bPresetL2);

    int getPayload(uint8_t *payload);

private:
    uint8_t _dataLen;
    uint8_t _data[ML_MAX_DATA_SIZE];
};

/////////////////////////////////////////
// Uplink: for tracker-node
/////////////////////////////////////////
// GPS Location
class MLReportLocationPayloadGen : public MLAnsNDCallPayloadGen {
public:
    MLReportLocationPayloadGen(uint32_t dateTime = 0, uint8_t statusGPS = 0, uint32_t lng = 0, uint32_t lat = 0, uint8_t batteryLevel = 0)
    : MLAnsNDCallPayloadGen(dateTime, statusGPS, lng, lat, batteryLevel)
    {
        _cmdId = CMD_REP_LOCATION;
        _version = 0x0C;
    }
    int getPayload(uint8_t *payload) { return 0; };
    void setPayload(const uint8_t *payload, int szPayload);
};

class MLAlarmGPSPayloadGen : public MLReportLocationPayloadGen {
public:
    MLAlarmGPSPayloadGen(uint32_t dateTime = 0, uint8_t statusGPS = 0, uint32_t lng = 0, uint32_t lat = 0, uint8_t batteryLevel = 0)
    : MLReportLocationPayloadGen(dateTime, statusGPS, lng, lat, batteryLevel)
    {
        _cmdId = CMD_REQ_ALARM_GPS;
    }
};

/////////////////////////////////////////
// iBeacon, EddyStone
class MLReportBeaconPayloadGen : public MLPayloadGen {
public:
    MLReportBeaconPayloadGen()
    : MLPayloadGen(CMD_REP_BEACON, 0, NULL, 0x0C)
    {
        _cmdId = CMD_REP_BEACON;
    }
    int getPayload(uint8_t *payload) { return 0; };
    void setPayload(const uint8_t *payload, int szPayload);
    
    uint8_t *getUuid() { return _uuid; }
    uint8_t getTypeReport() { return _typeReport; }
    uint8_t getTypeBeacon() { return _typeBeacon; }
    int8_t getRssi() { return _rssi; }
    int8_t getTxpower() { return _txPower; }
    uint8_t getBatteryLevel() { return _batteryLevel; }
private:
    uint8_t _uuid[20];
    union {
        uint8_t _type;
        struct {
            uint8_t _typeReport:5;
            uint8_t _typeBeacon:3;
        };
    };

    int8_t _rssi;
    int8_t _txPower;
    uint8_t _batteryLevel;
};


class MLReqAlarmBeaconPayloadGen : public MLReportBeaconPayloadGen {
public:
    MLReqAlarmBeaconPayloadGen()
    {
        _cmdId = CMD_REQ_ALARM_BEACON;
    }
};

#endif /* __MLPACKETGEN3_H */
