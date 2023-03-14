/* 
 * File:   uart1.h
 * Author: tushar.mali
 *
 * Created on 2 February, 2023, 4:34 PM
 */

#ifndef UART1_H
#define	UART1_H

#include "../projectdefines.h"

#ifdef	__cplusplus
extern "C" {
#endif

    void initUART1(void);
    void uart1Write(uint8_t data);
    void uart1PutString(char *tx_str, int size);


#ifdef	__cplusplus
}
#endif

#endif	/* UART1_H */

