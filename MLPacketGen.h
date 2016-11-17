#ifndef MLPACKETGEN_H
#define MLPACKETGEN_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "MLPacketComm.h"

typedef struct MLLocation {
    uint32_t longtitude;
    uint32_t latitude;
} mllocation;

class MLPayloadGen {
    public:
        MLPayloadGen(uint16_t cmdId = 0, uint8_t optionFlags = 0, uint8_t *optionData = NULL, uint8_t version = 0x0B) {
            _version = version;
            _cmdId = cmdId;
            _optionFlags = optionFlags;
            if (_optionFlags & 0x01 && optionData != NULL) {
                _optionDataLen = 3;
                memcpy(_optionData, optionData, _optionDataLen);
            } else {
                _optionDataLen = 0;
            }
        }

        void setMLPayloadOption(uint8_t optionFlags, uint8_t* optionData) {
            _optionFlags = optionFlags;
            if (_optionFlags & 0x01 && optionData != NULL) {
                _optionDataLen = 3;
                memcpy(_optionData, optionData, _optionDataLen);
            } else {
                _optionDataLen = 0;
            }
        }
    
        virtual int getPayload(uint8_t *payload) = 0;

        uint8_t getVersion() { return _version; }
        uint16_t getCmdId() { return _cmdId; }
        uint8_t getOptionFlags() { return _optionFlags; }
        uint8_t* getOptionData(uint8_t& optionDataLen) {
            optionDataLen = _optionDataLen;
            return _optionData;
        }
        uint8_t getOptionDataLen() { return _optionDataLen; }
    protected:
        uint8_t _version;
        uint16_t _cmdId;
        uint8_t _optionFlags;
        uint8_t _optionData[ML_MAX_OPTION_DATA_SIZE];
        uint8_t _optionDataLen;
};

class MLReqSetLoraConfigGen : public MLPayloadGen {
    public:
        MLReqSetLoraConfigGen(uint8_t *frequency, uint8_t dataRate, uint8_t power, uint8_t wakeupInterval, uint8_t groupId);
        int getPayload(uint8_t *payload);
        uint8_t* getFrequency() { return _frequency; }
        uint8_t getDataRate() { return _dataRate; }
        uint8_t getPower() { return _power; }
        uint8_t getWakeupInterval() { return _wakeupInterval; }
        uint8_t getGroupId() { return _groupId; }
    private:
        uint8_t _frequency[ML_FREQUENCY_LEN];
        uint8_t _dataRate;
        uint8_t _power;
        uint8_t _wakeupInterval;
        uint8_t _groupId;
};

class MLReqDataPayloadGen : public MLPayloadGen {
    public:
        MLReqDataPayloadGen(uint16_t resInterval, uint8_t dataLen, uint8_t *data);
        int getPayload(uint8_t *payload);
        uint16_t getResInterval() { return _resInterval; }
        uint8_t getDataLen() { return _dataLen; }
        uint8_t* getData(uint8_t& dataLen) {
            dataLen = _dataLen;
            return _data;
        }
    private:
        uint16_t _resInterval;
        uint8_t _dataLen;
        uint8_t _data[ML_MAX_DATA_SIZE];
};

class MLNotifyLocationGen : public MLPayloadGen {
    public:
        MLNotifyLocationGen(uint32_t dateTime, mllocation location, uint8_t notifyType, uint8_t gpsStatus);
        int getPayload(uint8_t *payload);
        uint32_t getDataTime() { return _dateTime; }
        void getLocation(mllocation& location) {
            location.longtitude = _location.longtitude;
            location.latitude = _location.latitude;
        }
        uint8_t getNotifyTYpe() { return _notifyType; }
        uint8_t getGpsStatus() { return _gpsStatus; }
    private:
        uint32_t _dateTime;
        mllocation _location;
        uint8_t _notifyType;
        uint8_t _gpsStatus;
};

class MLReqLocationGen : public MLPayloadGen {
    public:
        MLReqLocationGen(int32_t resInterval);
        int getPayload(uint8_t *payload);
        int32_t getResInterval() { return _resInterval; }
    private:
        int32_t _resInterval;
};

class MLSetGeoFenceConfigGen : public MLPayloadGen {
    public:
        MLSetGeoFenceConfigGen(uint16_t geofRadius, uint16_t resInterval);
        int getPayload(uint8_t *payload);
        uint16_t getGeoRadius() { return _geofRadius; }
        uint16_t getResInterval() { return _resInterval; }
    private:
        uint16_t _geofRadius;
        uint16_t _resInterval;
};

class MLGetGeoFenceConfigGen : public MLPayloadGen {
    public:
        MLGetGeoFenceConfigGen() : MLPayloadGen(CMD_GET_CONFIG_GEOF) {};
        int getPayload(uint8_t *payload);
};

class MLResSetLoraConfigGen : public MLPayloadGen {
    public:
        MLResSetLoraConfigGen(uint8_t errorCode);
        int getPayload(uint8_t *payload);
        uint8_t getErrorCode() { return _errorCode; }
    private:
        uint8_t _errorCode;
};

class MLResDataPayloadGen : public MLPayloadGen {
    public:
        MLResDataPayloadGen(uint8_t errorCode, uint8_t dataLen, uint8_t *data);
        int getPayload(uint8_t *payload);
        uint8_t getErrorCode() { return _errorCode; }
        uint8_t getDataLen() { return _dataLen; }
        uint8_t* getData(uint8_t& dataLen) {
            dataLen = _dataLen;
            return _data;
        }
    private:
        uint8_t _errorCode;
        uint8_t _dataLen;
        uint8_t _data[ML_MAX_DATA_SIZE];
};

class MLRetConfigGeofGen : public MLPayloadGen {
    public:
        MLRetConfigGeofGen(uint16_t geofRadius, uint16_t resInterval, mllocation location);
        int getPayload(uint8_t *payload);
        uint16_t getGeoFRadius() { return _geofRadius; }
        uint16_t getResInterval() { return _resInterval; }
        void getLocation(mllocation& location) {
            location.longtitude = _location.longtitude;
            location.latitude = _location.latitude;
        }
    private:
        uint16_t _geofRadius;
        uint16_t _resInterval;
        mllocation _location;
};

class MLNotifyVindunoPayloadGen : public MLPayloadGen {
    public:
        MLNotifyVindunoPayloadGen(uint8_t *apiKey, float soil_1, float soil_2, float soil_3, float soli_4, 
                float sysVoltage, float humidity, float temperature, float reserved);
        int getPayload(uint8_t *payload);
        uint8_t* getApiKey() { return _apiKey; }
        float getSoil1() { return _soil_1; }
        float getSoil2() { return _soil_2; }
        float getSoil3() { return _soil_3; }
        float getSoil4() { return _soil_4; }
        float getSysVoltage() { return _sysVoltage; }
        float getHumidity() { return _humidity; }
        float getTemperature() { return _temperature; }
        float getReserved() { return _reserved; }
    private:
        uint8_t _apiKey[VINDUNO_API_KEY_LEN]; 
        float _soil_1;
        float _soil_2;
        float _soil_3;
        float _soil_4;
        float _sysVoltage;
        float _humidity;
        float _temperature;
        float _reserved;
};

//////////////////////////////////////////////////////////////////////////////////

class MLReqAuthChallengePayloadGen : public MLPayloadGen {
public:
    MLReqAuthChallengePayloadGen() : MLPayloadGen(CMD_REQ_AUTH_CHALLENGE) {}
    
    int getPayload(uint8_t *payload)
    {
        return 4 + 4;
    }
};

class MLReqAuthJoinPayloadGen : public MLPayloadGen {
public:
    MLReqAuthJoinPayloadGen() : MLPayloadGen(CMD_REQ_AUTH_JOIN) {}
    
    int getPayload(uint8_t *payload)
    {
        // prefix
        payload[0] = _version;
        payload[1] = _cmdId & 0xFF;
        payload[2] = _cmdId >> 8;
        
        // postfix
        payload[3] = _optionFlags;
        if (_optionDataLen > 0)
            memcpy(&payload[4], _optionData, _optionDataLen);
        
        return (4 + _optionDataLen);
    }
};

#define SIZE_RES_AUTH_RESPONSE_HMAC     16
class MLResAuthResponsePayloadGen : public MLPayloadGen {
public:
    MLResAuthResponsePayloadGen(uint8_t *dataHMAC) : MLPayloadGen(CMD_RES_AUTH_RESPONSE)
    {
        memcpy(_dataHMAC, dataHMAC, SIZE_RES_AUTH_RESPONSE_HMAC);
    }
    
    int getPayload(uint8_t *payload)
    {
        // prefix
        payload[0] = _version;
        payload[1] = _cmdId & 0xFF;
        payload[2] = _cmdId >> 8;
        
        memcpy(&payload[3], _dataHMAC, SIZE_RES_AUTH_RESPONSE_HMAC);
        // postfix
        int pos = 3 + SIZE_RES_AUTH_RESPONSE_HMAC;
        payload[pos++] = _optionFlags;
        if (_optionDataLen > 0) {
            memcpy(&payload[pos], _optionData, _optionDataLen);
            pos += _optionDataLen;
        }
        
        return pos;
    }

    uint8_t* getDataMAC() { return _dataHMAC; }
private:
    uint8_t _dataHMAC[SIZE_RES_AUTH_RESPONSE_HMAC];
};

//////////////////////////////////////////////////////////////////////////////////

class MLPacketGen {
    public:
        MLPacketGen(uint8_t version = 0x0B);
        MLPacketGen(uint8_t ackBit, uint8_t receiverFlag, uint8_t packetType, uint8_t direction, uint8_t *id, uint8_t version = 0x0B);
        void setMLPacket(uint8_t ackBit, uint8_t receiverFlag, uint8_t packetType, uint8_t direction, uint8_t *id, uint8_t version);
        void setMLPayloadGen(MLPayloadGen *mlpayloadGen);
        int getMLPacket(uint8_t *mlpacket);
        uint8_t getAckBit() { return _ackBit; }
        uint8_t getReceiverFlag() { return _receiverFlag; }
        uint8_t getPacketType() { return _packetType; }
        uint8_t getDirection() { return _direction; }
        uint8_t* getID() { return _id; }
        uint8_t getVersion() { return _version; }
        MLPayloadGen* getMLPayload() { return _mlPayloadGen; }
    private:
        int getMLPayload(uint8_t *payload);
    private:
        uint8_t _ackBit;
        uint8_t _receiverFlag;
        uint8_t _packetType;
        uint8_t _direction;
        uint8_t _id[ML_PK_ID_SIZE];
        uint8_t _version;
        MLPayloadGen *_mlPayloadGen;
};

#endif /* MLPACKETGEN_H */
