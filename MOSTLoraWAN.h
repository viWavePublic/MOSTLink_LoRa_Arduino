//
//  MOSTLoraWAN.h
//  LM-130 module AT Commands
//
//  Created by Macbear Chen on 2017/2/2.
//  Copyright © 2017 viWave. All rights reserved.
//

#ifndef MOSTLoraWAN_h
#define MOSTLoraWAN_h

#include "LoraBase.h"

class MOSTLoraWAN : public LoraBase
{
public:
    // initialize for begin baud-rate
    void begin(long speed = 57600) {
        LoraBase::begin(speed);
    }

    // response
    boolean isOK(const char *strResponse);
    void testAll();
    
    // Upgrade the LM-130 module firmware.
    // Response ok after entering the command.
    void updateFW();
    
    // Show up firmware version.
    char *getFwVersion();
    
    // Put LM-130 into sleep mode.
    // To leave sleep mode, just Input 0xFF by UART to wake up LM-130.
    // Response ok after entering the command.
    void setSLEEP();
        
    // send payload by Tx: port: 1~223, cnf/uncnf, Hex-payload(11 bytes)
    // AAT2 Tx=[parameter1], [parameter2], [parameter3]
    // [parameter1]: decimal number representing the port number, from 1 to 223.
    // [parameter2]: string representing the uplink payload type, either “cnf” or “uncnf”.
    //      (cnf = confirmed, uncnf = unconfirmed)
    // [parameter3]: hexadecimal number representing the payload value.
    // The length of [parameter3] bytes capable of being transmitted are dependent upon the set data
    // rate. (Please refer to the LoRaWANTM Specification for further details)
    // Response: This command may reply with two responses. The first response will be received
    //      immediately is valid (ok reply received), the second reply will be received after the
    //      end of the uplink transmission. (Please refer to the the LoRaWANTM Specification for
    //      further details.)
    // Response after entering the command:
    //      *ok - if parameters and configurations are valid.
    //      *Invalid_param – if parameters ([parameter1],[parameter2],[parameter3]) are not valid.
    //      *Tx_ok - if “cnf” radio Tx return with ACK
    //      *Tx_ok - if “uncnf” radio Tx return
    //      *Tx_noACK – if ”cnf” radio Tx return without ACK
    //      *Rx < parameter1> < parameter2> – if transmission
    //          was successful, [parameter1] port number, from 1 to 223; [parameter2] hexadecimal
    //          number that was received from the server.
    boolean setTx(int port, char *confirm, char *payload);

    // [parameter1]: 0: Disable (Active Report Mode: Off) 1: Enable (Active Report Mode: On)
    // Response:
    //      ok if value is valid
    //      invalid_param if parameter1 is not valid
    // This command sets the state of the active report mode for the module.
    boolean setTestMode(int mode);
    
    // Response:
    //      0: disable (Active Report Mode: Off) 1: enable (Active Report Mode: On)
    //      This command will return the state of the active report mode.
    int getTestMode();
    
    // [parameter1]: 4-byte hexadecimal number representing the device address, from 00000001 – FFFFFFFF.
    // Response:
    //      ok if address is valid
    //      invalid_param if parameter1 is not valid
    // This command configures the module with a 4-byte unique network device address [parameter1].
    // The [parameter1] must be unique to the current network. This must be directly set solely for
    // activation by personalization devices. This parameter must not be set before attempting to
    // join using over-the-air activation because it will be overwritten once the join process is over.
    boolean setDevAddr(const char *strAddr);
    
    // Response: 4-byte hexadecimal number representing the device address, from 00000001 to FFFFFFFF.
    // This command will return present end-device address of the module.
    char *getDevAddr();
    
    // [parameter1]: 8-byte hexadecimal number representing the device EUI.
    // Response:
    //      ok if address is valid
    //      invalid_param if parameter1 is not valid
    // This command sets the globally unique device identifier for the module. The identifier
    // must be set by the host MCU. The module contains a pre-programmed unique EUI and
    // can be retrieved using user provided EUI can be configured using the AAT2 DevEui command.
    boolean setDevEui(const char *strEui);
    
    // Response: 8-byte hexadecimal number representing the device EUI. This command returns
    // the globally unique end-device identifier, as set in the module.
    char *getDevEui();
    
    // [parameter1]: 8-byte hexadecimal number representing the application EUI.
    // Response:
    //      ok if address is valid
    //      invalid_param if parameter1 is not valid
    // This command sets the application identifier for the module.
    boolean setAppEui(const char *strAddr);
    
    // Response: 8-byte hexadecimal number representing the application EUI. This command will return
    // the application identifier for the module. The application identifier is a value given to the
    // device by the network.
    char *getAppEui();
    
    // [parameter1]: 16-byte hexadecimal number representing the network session key.
    // Response:
    //      ok if address is valid
    //      invalid_param if parameter1 is not valid
    // This command sets the network session key for the module. This key is 16 bytes in length,
    // and should be modified with each session between the module and network. The key should
    // remain the same until the communication session between devices is terminated.
    boolean setNwkSKey(const char *strKey);
    
    // Response: [parameter1]: 16-byte hexadecimal number representing the network session key.
    // This command sets the network session key for the module.
    char *getNwkSKey();

    // [parameter1]: 16-byte hexadecimal number representing the application session key.
    // Response:
    //      ok if address is valid
    //      invalid_param if parameter1 is not valid
    // This command sets the application session key for the module. This key is unique, created for
    // each occurrence of communication, when the network requests an action taken by the application.
    boolean setAppSKey(const char *strKey);
    
    // Response: [parameter1]: 16-byte hexadecimal number representing the application session key.
    // This command sets the application session key for the module.
    char *getAppSKey();

    // [parameter1]: 16-byte hexadecimal number representing the application key.
    // Response:
    //      ok if address is valid
    //      invalid_param if parameter1 is not valid
    // This command sets the application key for the module. The application key is used to identify
    // a grouping over module units which perform the same or similar task.
    boolean setAppKey(const char *strKey);
    
    // Response: [parameter1]: 16-byte hexadecimal number representing the application key.
    // This command sets the application key for the module.
    char *getAppKey();
    
    // [parameter1]: 0: disable, 1: enable
    // Response:
    //      ok if address is valid
    //      invalid_param if parameter1 is not valid
    // This command sets if the adaptive data rate (ADR) is to be enabled, or disabled. The server
    // is informed about the status of the module’s ADR in every uplink frame it receives from the
    // ADR field in uplink data packet. If ADR is enabled, the server will optimize the data rate
    // and the transmission power of the module based on the information collected from the network.
    boolean setADR(int state);
    
    // Response: 0: disable 1: enable
    // This command will return the state of the adaptive data rate mechanism.
    int getADR();

    // [parameter1]: decimal number representing the report interval in seconds, from 1 to 254.
    // This command will only take effect when “TestMode”=1.
    // Response:
    //      ok if parameter1 is valid
    //      invalid_param if parameter1 is not valid
    // This command sets the report interval for the module.
    boolean setEVK_TxCycle(int interval);
    
    // Response: decimal number representing the interval, in seconds, for EVK_TxCycle, from 1 to 254.
    // This command will return the interval, in seconds, for EVK_TxCycle.
    int getEVK_TxCycle();

    // [parameter1]: 0: ABP mode 1: OTAA mode
    // Response:
    //      ok if address is valid
    //      invalid_param if parameter1 is not valid
    // This command informs the module activation type.
    boolean setJoinMode(int mode);
    
    // Response: 0: ABP mode 1: OTAA mode
    // This command will return the activation type of module.
    int getJoinMode();

    // [parameter1]: decimal number representing the number of retransmissions for an uplink
    //      confirmed packet, from 0 to 8.
    // Response:
    //      ok if address is valid
    //      invalid_param if parameter1 is not valid
    // This command sets the number of retransmissions to be used for an uplink confirmed packet,
    // if no downlink acknowledgment is received from the server.
    boolean setreTx(int retry);
    
    // Response: decimal number representing the number of retransmissions, from 0 to 8.
    // This command will return the currently configured number of retransmissions which are
    // attempted for a confirmed uplink communication when no downlink response has been received.
    int getreTx();

    // [parameter1]: decimal number representing the delay between the transmission and the first
    //      reception window in microseconds, from 100000 to 10000000.
    // Response:
    //      ok if address is valid
    //      invalid_param if parameter1 is not valid
    // This command will set the delay between the transmission and the first reception window to
    // the [parameter1] in microseconds. The delay between the transmission and the second Reception
    // window is calculated in software as the delay between the transmission and the first Reception
    // window + 1000000 (μs).
    boolean setRxDelay1(long delayMicroSec);
    
    // Response: decimal number representing the interval, in microseconds, for RxDelay1.
    // This command will return the interval, in microseconds, for RxDelay1.
    long getRxDelay1();
    
    // [parameter1]: 0: disable, 1: enable
    // Response:
    //      ok if address is valid
    //      invalid_param if parameter1 is not valid
    // This command sets the state of the duty cycle. You could set the ratio of duty cycle by AAT2
    // Tx_Band=[parameter1],[parameter2],[parameter3]. For the details, please refer to the command above.
    boolean setDutyCycle(int state);
    
    // Response: 0: disable 1: enable
    // This command will return the state of the duty cycle.
    int getDutyCycle();
    
    // [parameter1]: 0: disable, 1: enable
    // Response:
    //      ok if address is valid
    //      invalid_param if parameter1 is not valid
    // This command sets the payload length’s verification.
    boolean setPLCheck(int state);
    
    // Response: 0: disable 1: enable
    // This command will return the state of payload length’s verification.
    int getPLCheck();
    
    // [parameter1]: decimal number representing Rx2 frequency, from 000000001 to 999999999 in Hz.
    // [parameter2]: decimal number representing Rx2 Data Rate, from 0 to 15.
    // Response:
    //      ok if address is valid
    //      invalid_param if parameter1 is not valid
    // This command sets the frequency and Data Rate of Rx2.
    boolean setRx2_Freq_DR(long freq, int dataRate);
    
    // Response: decimal number representing the frequency and Data Rate of RX2.
    // This command will return the frequency and Data Rate of RX2. When RX2 frequency is 915MHz
    // and Data Rate is 3, the response message is as “Freq.915000000, DR3”.
    void getRx2_Freq_DR(long &freq, int &dataRate);

    // [parameter1]: 0: Class A, 2: Class C
    // This command sets the operation mode of the module.
    boolean setClassMode(int mode);
    
    // Response: 0: Class A 2: Class C
    // This command will return the operation mode of module.
    int getClassMode();
    
    // [parameter1]: decimal number representing the index of Rx1 offset, from 0 to 3.
    // This command sets the index of Rx1 offset. The Rx1DrOffset sets the offset between the
    // uplink data rate and the downlink data rate used to communicate with the end-device on
    // the first reception slot (Rx1). As a default this offset is 0. The offset is used to take
    // into account maximum power density constraints for base stations in some regions and to
    // balance the uplink and downlink radio link margins.
    boolean setRx1DrOffset(int offset);
    
    // Response: decimal number representing the index of Rx1 offset.
    // This command will return the index of Rx1 offset.
    int getRx1DrOffset();
    
    // Tx_Channel[x]:
    //      channel: US 0~71, EU 0~15
    //      frequency 000000001~999999999 in Hz
    //      data-rate 0~15
    //      channel state: 0 - close, 1 - open
    //      band grouping: US 0, EU 0~3
    // [parameter1]: decimal number representing the channel number. The range for US is from 0 to 71.
    //      The range for EU is from 0 to 15.
    // [parameter2]: decimal number representing the frequency of TX channel,
    //      from 000000001 to 999999999 in Hz.
    // [parameter3]: 2 digit decimal numbers representing the operating range of Data Rate.
    //      (The left one is DR’s Max, the right one is DR’s Min.) The range of DR is from 0 to 15.
    //      Note: According to LoRaWAN_Regional_Parameter.pdf, Data Rate in some regions will be limited
    //      in a particular range. For example, upstream 64 channels numbered 0 to 63 utilizing
    //      LoRa 125 kHz BW varying from DR0 to DR3 for US.
    // [parameter4]: 0/1 representing the channel is close/open. [parameter5]: decimal number representing
    //      the number of band grouping. The range for US is 0. The range for EU is from 0 to 3.
    // Please refer to AAT2 Tx_Band=[parameter1], [parameter2],[parameter3] for further understanding.
    // Response:
    //      ok if address is valid
    //      invalid_param if one of parameters is not valid.
    // This command sets the frequency, Data Rate, status and the number of band grouping for assigned
    // Tx channel.
    // For example:
    //      AAT2 Tx_Channel=3,973300000,40,1,0
    // Description:
    //      Modify Channel 3 to have the settings below. 973300000 = Frequency
    //      40 = DR Range; (4=DR Max, 0=DR Min)
    //      1=Status ;(1=open)
    //      0 = the number of band grouping
    boolean setTx_Channel(int channel, long freq, int dataRate, int channelOpen, int bandGroup);
    
    // Response: the assigned Tx channel will be shown. Variable x needs to be assigned as channel number.
    // This command will show the assigned Tx channel. For example:
    //      AAT2 Tx_Channel15=?
    // Channel15 message will be shown as below:
    //      channel_15,Freq.905300000,DrRange.0-3,Status0, Band0
    void getTx_Channel(int channel, long &freq, int &dataRate, int &channelOpen, int &bandGroup);

    // Tx_Band[x]: band grouping: US 0, EU 0~3, dutyCycle 1~9999, ID-power 0~15
    // [parameter1]: decimal number representing the number of band grouping. The range for US is 0.
    //      The range for EU is from 0 to 3.
    // [parameter2]: decimal number representing the value of duty cycle, from 1 to 9999.
    //      The real duty cycle could be calculated as (100% / duty cycle value).
    // [parameter3]: decimal number representing the index of Tx power, from 0 to 15.
    // Response:
    //      ok if address is valid
    //      invalid_param if one of parameters is not valid.
    // This command sets duty cycle and Tx power for the assigned number of band grouping.
    // For example:
    //      AAT2 Tx_Band=0,50,5 (for US)
    // Description:
    //      The real duty cycle = 100% / 50 = 2%
    //      Modify band grouping 0 to set duty cycle as 2% and Tx power index as 5 (20dBm).
    boolean setTx_Band(int bandGroup, int dutyCycle, int idPower);
    
    // Response: the assigned Tx band will be shown. Variable x needs to be assigned as band grouping number.
    // This command will show the assigned Tx band. For example:
    //      AAT2 Tx_Band0=?
    //      The message will be shown as below: Band_0, DutyCycle.1, TxPower.5
    void getTx_Band(int bandGroup, int &dutyCycle, int &idPower);

    // Response: decimal number representing the number of uplink frame counter.
    // This command will return the number of uplink
    int getUplink_Count();

    // Response: decimal number representing the number of downlink frame counter.
    // This command will return the number of downlink frame counter.
    int getDownlink_Count();

    // Tx_Power[x]: ID-power 0~15, dbm: US 0~30, EU 0~20
    // [parameter1]: decimal number representing the index of Tx power from 0 to 15.
    // [parameter2]: decimal number representing the real TxPower. The range for US is 0 to 30.
    //      The range for EU is from 0 to 20.
    // Response:
    //      ok if address is valid
    //      invalid_param if one of parameters is not valid
    // This command sets the index of Tx power and the real Tx power for Tx power table.
    boolean setTx_Power(int idPower, int dbmPower);
    
    // Response: the real power of assigned Tx power index will be shown. Variable x needs to
    // be assigned as Tx power index.
    // This command will show the real power of the assigned Tx power index.
    // For example:
    //      AAT2 Tx_Power2=?
    //      The message will be shown as below: TxPower_2, 26 dBm.
    int getTx_Power(int idPower);
    
    // Pl_Max_Length[x]: dataRate 0~15, payload length: 0~255
    // [parameter1]: decimal number representing Data Rate from 0 to 15.
    // [parameter2]: decimal number representing maximum application payload length (N) from 0 to 255.
    // Response:
    //      ok if address is valid
    //      invalid_param if one of parameters is not valid
    // This command sets maximum application payload length (N) (without repeater) according to the
    // assigned Data Rate.
    boolean setPl_Max_Length(int dataRate, int lenPayload);
    
    // Response: the list of assigned Pl_Max_Length setting will be shown. Variable x needs to
    // be assigned as the level of Data Rate.
    // This command will show the assigned Pl_Max_Length setting.
    int getPl_Max_Length(int dataRate);
    
    // Plre_Max_Length[x]: dataRate 0~15, payload length: 0~255
    // [parameter1]: decimal number representing DataRate from 0 to 15.
    // [parameter2]: decimal number representing maximum application payload length (N) from 0 to 255.
    // Response:
    //      ok if address is valid
    //      invalid_param if one of parameters is not valid
    // This command sets maximum application payload length (N) (with repeater) according to the
    // assigned Data Rate.
    boolean setPlre_Max_Length(int dataRate, int lenPayload);
    
    // Response: the list of assigned Plre_Max_Length setting will be shown. Variable x needs to
    // be assigned as the level of Data Rate.
    // This command will show the assigned Plre_Max_Length setting.
    int getPlre_Max_Length(int dataRate);
};


#endif /* MOSTLoraWAN_h */
