#include "MLPacketGen.h"

int main() {
    uint8_t mlpacket[99];
    MLPacketGen mlPacketGen(0,0,0,1,0xFFFFFFFFFFFFFF01,0x9B667C0E0B000480);
    mlPacketGen.setMLPayloadGen(0x0004);
    
    uint8_t cmdData[8] = {1,2,3,4,5,6,7,8};
    uint16_t param[1] = {100};
    MLPayloadGen *mlPayloadGen = mlPacketGen.getMLPayloadGen();
    mlPayloadGen->setPayload(cmdData, 8, param);
    uint8_t packetLen = mlPacketGen.getMLPacket(mlpacket);

    for (int i=0; i<packetLen; i++){
        printf("0x%02X ", mlpacket[i]);
        if((i+1)%16 == 0)
            printf("\n");
    }

    return 0;
}
