#ifndef  _CRC16_CODE_H
#define  _CRC16_CODE_H

//#include "psptypes.h"
//#include "global.h"

uint16_t CRC16Calc(uint8_t *puchMsg, uint16_t usDataLen);
uint8_t crc16_calc(uint8_t_ptr r_return_value_ptr,uint16_t recv_len);

uint8_t crc16_calc_frame_gw_v1(uint8_t_ptr r_frame_ptr,uint16_t len);

uint16_t chksum(uchar *buf, uint16_t len);
uint8_t crc_sum(uint8_t_ptr data,uint16_t len);
uint8_t dlt_crc_check(uint8_t_ptr data,uint16_t len,uint8_t crc_checked);
#endif   //_CRC16_CODE_H