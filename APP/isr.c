#include "isr.h"


uint8_t giRxIndex = 0;
uint8_t gcRx_buf[32];   //receiver buffer which will save data coming to slave(U1RXREG)
//we are going to receive total 8bytes of data as modbus having 8 byte frame while 
//reading input registers

uint8_t giTxIndex = 0;
uint8_t gcTxBuf[64];

bool gbFlagPacketReceived = false;

/*********************************************************************************************/
/*
 * initInterrupt function will set interrupt enable registers U1RXIE and TIIE
 * U1RXIE - UART1 receive interrupt enable 
 * TIIE   - Timer1 interrupt enable 
 */
void initInterrupts(void)
{
    IEC0bits.U1RXIE = 1;
    //IEC0bits.U1TXIE = 1;
    IEC0bits.T1IE = 1;   
}
/*********************************************************************************************/

/*********************************************************************************************/
/*
 * U1RXInterrupt will occur when U1RXREG receives a data. Here we are getting 
 * total 8 bytes of data one by one. when first byte receives that data stored 
 * in Rx buffer and index of Rx Buffer gets incremented.
 * When total 8 bytes of data received in Rx Buffer then there will be no data 
 * coming on U1RXREG so UART interrupt will not be called.
 * Hence, timer interrupt called as TMR1 matches PR1 value.
 */
void __attribute__((interrupt(auto_psv))) _U1RXInterrupt(void)
{
    T1CONbits.TON = 1;  //timer1 ON   
    TMR1 = 0;   //resets the timer
    gcRx_buf[giRxIndex] = U1RXREG;    //Rx Buffer will get filled with U1RXREG 
    giRxIndex++;  //index increments

    IFS0bits.U1RXIF = 0;    //UART1 interrupt flag cleared
}
/***********************************************************************************************/

/***********************************************************************************************/
/*
 * When timer interrupt called, it will first turn off the timer. 
 * Will make index = 0 so when next modbus packet receive. 
 * It will save as new buffer.
 * One flag will set indicating that modbus packet fully received 
 * that can be driven in app.c 
 */
void __attribute__((interrupt(auto_psv))) _T1Interrupt(void)
{
    T1CONbits.TON = 0;  //timer1 OFF
    gbFlagPacketReceived = true;   
    IFS0bits.T1IF = 0;  //Timer1 interrupt flag cleared
}
/***********************************************************************************************/

