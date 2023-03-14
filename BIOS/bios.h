/* 
 * File:   bios.h
 * Author: tushar.mali
 *
 * Created on 2 February, 2023, 4:32 PM
 */

#ifndef BIOS_H
#define	BIOS_H

#include "../projectdefines.h"
#include "timer1.h"
#include "uart1.h"
#include "pinmap.h"

#ifdef	__cplusplus
extern "C" {
#endif


    void initBIOS(void);

#ifdef	__cplusplus
}
#endif

#endif	/* BIOS_H */

