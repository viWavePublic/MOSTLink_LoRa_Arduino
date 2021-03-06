#include "../MLPacketGen.h"
#include "../MLPacketParser.h"

const uint8_t REQ_DATA_PK[] = {
0xFB,0xFC,0x0B,0x1A,0x01,0x9B,0x66,0x7C,0x0F,0x17,0x00,0x03,0x00,0x8D,0x0A,0x04,
0x00,0x0A,0x00,0x04,0x74,0x65,0x73,0x74,0x00,0x16 };

const uint8_t RES_DATA_PK[] = {
0xFB,0xFC,0x0B,0x1D,0x08,0x9B,0x66,0x7C,0x0F,0x17,0x00,0x03,0x00,0x83,0x0A,0x02,
0x02,0x00,0x08,0x00,0x00,0x2E,0x42,0x33,0x33,0xC3,0x41,0x00,0xEC };

int main() {
    uint8_t reqPacket[99];
    uint8_t resPacket[99];
    
    uint8_t ID[8] = {0x9B, 0x66, 0x7C, 0x0F, 0x17, 0x00, 0x03, 0x00};    
    MLPacketGen reqPacketGen(1, 0, 0, 0, ID);

    uint8_t cmdData[4] = {0x74,0x65,0x73,0x74};
    uint8_t optionData[1] = {0x00};
    reqPacketGen.setMLPayloadGen(new MLReqDataPayloadGen(10, 4, cmdData));
    uint8_t packetLen = reqPacketGen.getMLPacket(reqPacket);
    //for (int i=0; i<packetLen; i++){
    //    printf("0x%02X ", mlpacket[i]);
    //    if((i+1)%16 == 0)
    //        printf("\n");
    //}
    
    MLPacketGen resPacketGen(0, 0, 0, 1, ID);
    uint8_t cmdD[8] = {0x00, 0x00, 0x2E, 0x42, 0x33, 0x33, 0xC3, 0x41};
    resPacketGen.setMLPayloadGen(new MLResDataPayloadGen(0, 8, cmdD));
    resPacketGen.getMLPacket(resPacket);

    MLPacketParser pkParser;
    MLPacketGen mlPacket;

    pkParser.mostloraPacketParse(&mlPacket, reqPacket);
    printf("Version: %d\n", mlPacket.getVersion());
    uint8_t id[8] = {0};
    uint8_t* idP = mlPacket.getID();
    memcpy(id, idP, 8);
    for (int i =0; i<8; i++) {
        printf("0x%02x ",id[i]);
    }
    printf("\n");
    MLReqDataPayloadGen* reqPayload = dynamic_cast<MLReqDataPayloadGen*>(mlPacket.getMLPayload());
    printf("ResInterval:%d", reqPayload->getResInterval());
    printf("\n");
    uint8_t dSize=0;
    uint8_t* dataPtr;
    dataPtr = reqPayload->getData(dSize);
    for (int i = 0; i < dSize; i++) {
        printf("0x%02x ", *(dataPtr+i));
    }
    printf("\n");

    pkParser.mostloraPacketParse(&mlPacket, resPacket);
    pkParser.mostloraPacketParse(&mlPacket, REQ_DATA_PK);
    pkParser.mostloraPacketParse(&mlPacket, RES_DATA_PK);
 
    return 0;
}
