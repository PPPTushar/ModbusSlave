#include "timer1.h"




/**
 * This timer initializes such that PR1 match occurs at 1ms.
 * This means the T1IF flag is set every 1ms.
 */
void initTimer1(void)
{
    T1CONbits.TSIDL = 0; //continues through Idle Mode
    T1CONbits.TGATE = 0; //no TGATE Operations
    T1CONbits.TCS = 0; //internal CLK 
    T1CONbits.TECS = 0x00;

    //T1CONbits.TCKPS = 3; //1:256 pre-scale
    T1CONbits.TCKPS1 = 0; //1:1 pre-scale
    T1CONbits.TCKPS0 = 0; //1:1 pre-scale
    TMR1 = 0x00;
    PR1 = 16000U;         //value calculated for 4ms
}


/**
 * This delay function running in a for loop for ms times.
 * If ms value given is 100, then total 1sec delay generated.
 */
//void timer1Delayms(int ms)
//{
//    unsigned int i;
//    T1CONbits.TON = 1;
//    for(i=0;i<ms;i++)
//    {
//       while (!IFS0bits.T1IF);
//       IFS0bits.T1IF = 0;   
//    }
//    T1CONbits.TON = 0; 
//}

