/* 
 * File:   modbusslave.h
 * Author: tushar.mali
 *
 * Created on 2 March, 2023, 1:05 PM
 */

#ifndef MODBUSSLAVE_H
#define	MODBUSSLAVE_H

#include "../BIOS/bios.h"
#include "isr.h"

#define MODBUS_SLAVE_ID 1

#define READ_INPUT_REGISTER 0x04
#define READ_HOLDING_REGISTER 0x03
#define WRITE_SINGLE_REGISTER 0x06
#define WRITE_MULTIPLE_REGISTER 0x10

#define RIR_QTY_START_INDEX 4

#define WSR_ADDRESS_START_INDEX 2           //address index in write single register
#define WSR_VALUE_INDEX 4                   //reg value index in write single register

#define WMR_BYTE_COUNT_INDEX 6              //Byte count index in write multiple register
#define WMR_REG_VALUE_START_INDEX 7         //reg value start index in write multiple register


#define MB_FC_INDEX 1



#ifdef	__cplusplus
extern "C" {
#endif

    extern uint8_t gcRx_buf[32];
    extern uint8_t gcTxBuf[64];
    void modbusParsePacket();
    bool crcCheck();
    void buildModbusPacket();
    void sendModbusPacket();
    void readHoldingRegister();
    void readInputRegister();
    void writeSingleRegister();
    void writeMultipleRegister();
    

#ifdef	__cplusplus
}
#endif

#endif	/* MODBUSSLAVE_H */

