#ifndef MLPACKETPARSER_H
#define MLPACKETPARSER_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "MLPacketComm.h"
#include "MLPacketGen.h"

class MLPacketParser {
    public:
        int mostloraPacketParse(MLPacketGen *mlpacket, const uint8_t *packet);
    private:
        int mostloraPayloadParse(MLPacketGen *mlpacket, const uint8_t *payload, const int szPayload);
};

#endif /* MLPACKETPARSER_H */
