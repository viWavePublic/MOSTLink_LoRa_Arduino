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

/////////////////////////////////////////
// Uplink: for tracker-node
/////////////////////////////////////////

class MLReportLocationPayloadGen : public MLAnsNDCallPayloadGen {
public:
    MLReportLocationPayloadGen(uint32_t dateTime = 0, uint8_t statusGPS = 0, uint32_t lng = 0, uint32_t lat = 0, uint8_t batteryLevel = 0)
    : MLAnsNDCallPayloadGen(dateTime, statusGPS, lng, lat, batteryLevel)
    {
        _cmdId = CMD_REP_LOCATION;
    }
    int getPayload(uint8_t *payload) { return 0; };
    void setPayload(const uint8_t *payload, int szPayload);
};

class MLReportBeaconPayloadGen : public MLPayloadGen {
public:
    MLReportBeaconPayloadGen()
    : MLPayloadGen(CMD_REP_LOCATION)
    {
        _cmdId = CMD_REP_LOCATION;
    }
    int getPayload(uint8_t *payload) { return 0; };
    void setPayload(const uint8_t *payload, int szPayload);
};


#endif /* __MLPACKETGEN3_H */
