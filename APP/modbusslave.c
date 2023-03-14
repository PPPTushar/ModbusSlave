#include "modbusslave.h"
#include "isr.h"


static uint16_t iInputRegisters[10] = {0};

static uint16_t iHoldingRegisters[10] = {0};

static uint16_t getCRC16(uint8_t *packet, uint16_t iSize);

static uint8_t auchCRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40
};

static uint8_t auchCRCLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
    0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
    0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
    0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
    0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
    0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
    0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
    0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
    0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
    0x40
};


/********************************************************************************/
/*
 * HERE WE ARE CHECKING CRC AND IF IT IS TRUE THEN BUILD AND SEND MODBUS PACKET. 
 */
void modbusParsePacket()
{
    if(crcCheck() == true)
    {
        if(gcRx_buf[0] == MODBUS_SLAVE_ID)
        {
            buildModbusPacket();
            sendModbusPacket();
        }
    }
}
/********************************************************************************/

/********************************************************************************/
/*
 * HERE WE ARE CHECKING CRC CALCULATED AND CRC RECEIVED AT SLAVE. 
 * IF CRC IS MATCHED THEN RETURN TRUE ELSE RETURN FALSE.
 */
bool crcCheck()
{
    uint16_t CRC16 = getCRC16(gcRx_buf, (giRxIndex - 2));
    uint16_t CRCReceived = (gcRx_buf[giRxIndex - 1] << 8) | gcRx_buf[giRxIndex - 2];
    if (CRCReceived == CRC16)
    {
        return true;
    }
    return false;
}
/********************************************************************************/

/********************************************************************************/
/*
 * HERE WE BUILD MODBUS PACKET FUNCTION USING IF ELSE.
 * WE ARE USING TXBUF TO STORE RESPONSE PACKET CALCULATED BY SLAVE.

void buildModbusPacket()
{
    // MODBUS RESPONSE PACKET FRAMING
    //| SLAVE_ID | FUNCTION_CODE | BYTE COUNT | DATA      | CRC     |
    //| 1 BYTE   |  1 BYTE       |  1 BYTE    | N*2 BYTES | 2 BYTES |

    uint16_t iByteCount = (gcRx_buf[RIR_QTY_START_INDEX] << 8 | gcRx_buf[RIR_QTY_START_INDEX + 1]) * 2;
    uint16_t CRC16;
    uint8_t iFunctCode = gcRx_buf[MB_FC_INDEX]; 
    
    giTxIndex = 0;
    gcTxBuf[giTxIndex++] = MODBUS_SLAVE_ID;
    gcTxBuf[giTxIndex++] = iFunctCode;
    gcTxBuf[giTxIndex++] = iByteCount & 0xFF;

    if (iFunctCode == READ_HOLDING_REGISTER)
    {
        for (int i = 0; i < (iByteCount / 2); i++)
        {
            gcTxBuf[giTxIndex++] = iHoldingRegisters[i] << 8;
            gcTxBuf[giTxIndex++] = iHoldingRegisters[i] & 0xFF;
        }
    }
    else if(iFunctCode == READ_INPUT_REGISTER)
    {
        for (int i = 0; i < (iByteCount / 2); i++)
        {
            gcTxBuf[giTxIndex++] = iInputRegisters[i] << 8;
            gcTxBuf[giTxIndex++] = iInputRegisters[i] & 0xFF;
        }
    }
    CRC16 = getCRC16(gcTxBuf, giTxIndex);
    gcTxBuf[giTxIndex++] = CRC16 & 0xFF;
    gcTxBuf[giTxIndex++] = CRC16 >> 8;
    
}
/********************************************************************************/

/********************************************************************************/
/*
 * HERE WE BUILD MODBUS PACKET FUNCTION USING SWITCH CASE.
 * WE ARE USING TXBUF TO STORE RESPONSE PACKET CALCULATED BY SLAVE.
*/
void buildModbusPacket()
{
    // MODBUS RESPONSE PACKET FRAMING
    //| SLAVE_ID | FUNCTION_CODE | BYTE COUNT | DATA      | CRC     |
    //| 1 BYTE   |  1 BYTE       |  1 BYTE    | N*2 BYTES | 2 BYTES |
  
    giTxIndex = 0;
    gcTxBuf[giTxIndex++] = MODBUS_SLAVE_ID;
    gcTxBuf[giTxIndex++] = gcRx_buf[MB_FC_INDEX]; 
    switch(gcRx_buf[MB_FC_INDEX]) 
    {
    case READ_HOLDING_REGISTER:
        readHoldingRegister();
        break;
    case READ_INPUT_REGISTER:
        readInputRegister();
        break;
    case WRITE_SINGLE_REGISTER:
        writeSingleRegister();
        break;
    case WRITE_MULTIPLE_REGISTER:
        writeMultipleRegister();
        break;
    default:
        break;
    }
}
    
void readHoldingRegister()
{
    uint16_t iByteCount = (gcRx_buf[RIR_QTY_START_INDEX] << 8 | gcRx_buf[RIR_QTY_START_INDEX + 1]) * 2;
    uint16_t CRC16;
    gcTxBuf[giTxIndex++] = iByteCount & 0xFF;

    for (int i = 0; i < (iByteCount / 2); i++)
    {
        gcTxBuf[giTxIndex++] = iHoldingRegisters[i] << 8;
        gcTxBuf[giTxIndex++] = iHoldingRegisters[i] & 0xFF;
    }

    CRC16 = getCRC16(gcTxBuf, giTxIndex);
    gcTxBuf[giTxIndex++] = CRC16 & 0xFF;
    gcTxBuf[giTxIndex++] = CRC16 >> 8;
}

void readInputRegister()
{
    uint16_t iByteCount = (gcRx_buf[RIR_QTY_START_INDEX] << 8 | gcRx_buf[RIR_QTY_START_INDEX + 1]) * 2;
    uint16_t CRC16;
    gcTxBuf[giTxIndex++] = iByteCount & 0xFF;

    for (int i = 0; i < (iByteCount / 2); i++)
    {
        gcTxBuf[giTxIndex++] = iInputRegisters[i] << 8;
        gcTxBuf[giTxIndex++] = iInputRegisters[i] & 0xFF;
    }

    CRC16 = getCRC16(gcTxBuf, giTxIndex);
    gcTxBuf[giTxIndex++] = CRC16 & 0xFF;
    gcTxBuf[giTxIndex++] = CRC16 >> 8;
}

void writeSingleRegister()
{
    // MODBUS RESPONSE PACKET FRAMING FOR WRITE SINGLE REGISTER
    //| SLAVE_ID | FUNCTION_CODE | REGISTER ADDRESS(Hi) |REGISTER ADDRESS(Lo)| REGISTER VALUE(Hi) |REGISTER VALUE(Lo)  | CRC     |
    //| 1 BYTE   |  1 BYTE       |  1 BYTE              | 1 BYTE             | 1 BYTE               | 1 BYTE               |2 BYTE   |
    
    uint16_t CRC16;
    uint16_t iAddress = (gcRx_buf[WSR_ADDRESS_START_INDEX] << 8 | gcRx_buf[WSR_ADDRESS_START_INDEX + 1]); 
    iHoldingRegisters[iAddress] = (gcRx_buf[WSR_VALUE_INDEX] << 8 | gcRx_buf[WSR_VALUE_INDEX + 1]);
    
    gcTxBuf[giTxIndex++] = iAddress << 8;
    gcTxBuf[giTxIndex++] = iAddress & 0xFF;
    
    gcTxBuf[giTxIndex++] = iHoldingRegisters[iAddress] << 8;
    gcTxBuf[giTxIndex++] = iHoldingRegisters[iAddress] & 0xFF;
    
    CRC16 = getCRC16(gcTxBuf, giTxIndex);
    gcTxBuf[giTxIndex++] = CRC16 & 0xFF;
    gcTxBuf[giTxIndex++] = CRC16 >> 8;
}

void writeMultipleRegister()
{
    // MODBUS RESPONSE PACKET FRAMING FOR WRITE MULTIPLE REGISTER
    //| SLAVE_ID | FUNCTION_CODE | STARTING ADDRESS(Hi) |STARTING ADDRESS(Lo)| QTY OF REG(Hi) | QTY OF REG(Lo)  | CRC     |
    //| 1 BYTE   |  1 BYTE       |  1 BYTE              | 1 BYTE             | 1 BYTE         | 1 BYTE          |2 BYTE   |
    
    uint16_t CRC16;
    uint16_t iStartAddress = (gcRx_buf[WSR_ADDRESS_START_INDEX] << 8 | gcRx_buf[WSR_ADDRESS_START_INDEX + 1]); 
    uint16_t iNumRegs = (gcRx_buf[WMR_BYTE_COUNT_INDEX])/2;
    uint16_t iCurrentIndex = WMR_REG_VALUE_START_INDEX;

    for (int i= iStartAddress; i < (iStartAddress + iNumRegs); i++)
    {
        iHoldingRegisters[i] = gcRx_buf[iCurrentIndex] << 8 | gcRx_buf[iCurrentIndex + 1];
        iCurrentIndex += 2;
    }
    
    gcTxBuf[giTxIndex++] = iStartAddress << 8;
    gcTxBuf[giTxIndex++] = iStartAddress & 0xFF;
    
    gcTxBuf[giTxIndex++] = iNumRegs << 8;
    gcTxBuf[giTxIndex++] = iNumRegs & 0xFF;
    
    CRC16 = getCRC16(gcTxBuf, giTxIndex);
    gcTxBuf[giTxIndex++] = CRC16 & 0xFF;
    gcTxBuf[giTxIndex++] = CRC16 >> 8;
}
/********************************************************************************/

/********************************************************************************/
/*
 * SEND MODBUS PACKET FUNCTION IS SENDING ONE BY ONE BYTE OF TXBUF TO UART. 
 * IT IS SENDING RESPONSE PACKET GENERATED BY MODBUS SLAVE. 
 */
void sendModbusPacket()
{
    unsigned int i;
    for (i = 0; i < giTxIndex; i++)
    {
        uart1Write(gcTxBuf[i]);
    }
}
/********************************************************************************/

/********************************************************************************/
/*
 * GETCRC16 IS A VARIABLE USED TO STORE VALUE OF CRC CALCULATED BU USING GCRXBUF
 * CRC CALCULATION IS DONE USING RXBUF PACKET RECEIVED AT MODBUS SLAVE.
 * HERE, *PACKET IS GCRXBUF. 
 */
static uint16_t getCRC16(uint8_t *packet, uint16_t iSize)
{
    uint16_t CRC16;
    
    unsigned char uchCRCHi = 0xFF; /* high byte of CRC initialized */
    unsigned char uchCRCLo = 0xFF; /* low byte of CRC initialized */
    uint16_t uIndex; /* will index into CRC lookup table */
    while (iSize--) /* pass through message buffer */
    {
        uIndex = uchCRCLo ^ *packet++; /* calculate the CRC */
        uchCRCLo = uchCRCHi ^ auchCRCHi[uIndex];
        uchCRCHi = auchCRCLo[uIndex];
    }
    CRC16 = (uchCRCHi << 8 | uchCRCLo);
    return CRC16;
}
/********************************************************************************/