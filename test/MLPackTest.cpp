#include "../MLPacketGen.h"

int main() {
    uint8_t mlpacket[99];
    MLPacketGen mlPacketGen(0,0,0,1,0xFFFFFFFFFFFFFF01,0x9B667C0E0B000480);

    uint8_t cmdData[8] = {1,2,3,4,5,6,7,8};
    uint8_t optionData[3] = {0x12, 0x34, 0x56};
    mlPacketGen.setMLPayloadGen(MLPayloadGen::createReqDataPayloadGen(100, 8, cmdData, 1, optionData));
    uint8_t packetLen = mlPacketGen.getMLPacket(mlpacket);

    for (int i=0; i<packetLen; i++){
        printf("0x%02X ", mlpacket[i]);
        if((i+1)%16 == 0)
            printf("\n");
    }

    return 0;
}