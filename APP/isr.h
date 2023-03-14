/* 
 * File:   isr.h
 * Author: tushar.mali
 *
 * Created on 10 February, 2023, 5:08 PM
 */

#ifndef ISR_H
#define	ISR_H

#include "../BIOS/bios.h"

#ifdef	__cplusplus
extern "C" {
#endif

    extern uint8_t giRxIndex;
    extern uint8_t giTxIndex;
    extern uint8_t gcTxBuf[64];
    extern uint8_t gcRx_buf[32];

    extern bool gbFlagPacketReceived;
    void initInterrupts(void);


#ifdef	__cplusplus
}
#endif

#endif	/* ISR_H */


