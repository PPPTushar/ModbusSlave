#include "bios.h"

void initBIOS(void)
{
    initPinmap();
    initTimer1();
    initUART1();
}
