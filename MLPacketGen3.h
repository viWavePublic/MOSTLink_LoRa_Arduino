#ifndef __MLPACKETGEN3_H
#define __MLPACKETGEN3_H

#include "MLPacketGen.h"
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

#endif /* __MLPACKETGEN3_H */
