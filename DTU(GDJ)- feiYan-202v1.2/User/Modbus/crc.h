#ifndef _crc_h_
#define _crc_h_
#include "stdint.h"
extern uint16_t calCRC16(uint8_t *pSource, uint32_t uiLen);
extern uint16_t calCRC16_modbus(uint8_t *updata,uint16_t len);
extern uint16_t M433_CalCRC16(uint8_t *pucBuf, uint16_t uwLength);
extern uint8_t  calCRC7(uint8_t *pSource, uint16_t len);
#endif

