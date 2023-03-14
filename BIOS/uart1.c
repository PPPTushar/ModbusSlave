#include "uart1.h"


/**
 * Baud rate register loaded with the value calculated for 9600 baud rate.
 */
void initUART1(void)
{
    U1BRG = 25;                     //9600 baud rate set into baud rate register     
    U1MODE = 0x8000;                //UART enabled
    U1STA = 0;                      //UART1 status and control register
    
    U1STAbits.UTXEN = 1;            //Enable Transmission
    U1STAbits.URXEN = 1;            //Enable Receiver    
}



/**
 * write a data on U1TXREG register. When UTXBF is full, data gets transmitted. 
 */
void uart1Write(uint8_t data)
{
    while (U1STAbits.UTXBF);       // Wait until the transmit buffer is empty
    U1TXREG = data;                 // Write the data to the UART1 transmit register
}



void uart1PutString(char *tx_str, int size)
{
    unsigned int i;
    for(i=0; i<size; i++)
    {
        uart1Write(*tx_str);
        tx_str++;
    }
}

