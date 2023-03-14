#include "app.h"

static void initApp(void);

void runApp()
{
    initApp();
    while (1)
    {   
        if(gbFlagPacketReceived == true)
        {
            gbFlagPacketReceived = false;
            modbusParsePacket();
            giRxIndex = 0;
        }
    }
}

static void initApp(void)
{
    initInterrupts();
}