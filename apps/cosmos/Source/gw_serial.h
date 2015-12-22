#ifndef _GW_SERIAL_H
#define _GW_SERIAL_H

//#include <serial.h>

#define SERIAL_DNLK_RECV_BUFFER_LEN     100
#define SERIAL_DNLk_READ_BUFFER_LEN		100
#define SERIAL_DNLk_QUEUE_SIZE			128//1024//512

#define RF_BUF_SIZE						1024
#define RS485_BUF_SIZE					1024
#define DBLK_BUF_SIZE					1024


void serial_dnlk_read_init();
uint32_t isr_readrf(uchar*, uchar*, uint8_t, uint8_t);
uint32_t task_isr_readrf(uchar*, uint8_t);
uint32_t rs485lk_wirte(uchar *buff, uint16_t len);
uint32_t rflk_wirte(uchar *buff, uint16_t len);
#endif 