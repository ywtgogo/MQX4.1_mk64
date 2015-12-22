/*  Defines and data structures for application       
*	定义全局互斥量，信号量
*	全局使用的函数名:
*	
*/
#ifndef	_GLOBAL_H_
#define	_GLOBAL_H_	1

#include <mqx.h>
#include <bsp.h>
#include <mutex.h>

extern MUTEX_STRUCT                 mutex_serial_rf;							//将scan和bypass配合使用

extern MQX_FILE_PTR 				fd_ittye_485_ptr;
extern MQX_FILE_PTR 				fd_ittya_wifi_ptr;

typedef enum Error_Code
{
    EC_OK,      //0
    EC_ERROR,   //1
	EC_ING,
    EC_LIST_NULL_FUN,
    EC_NODE_NOT_FOUND,
    EC_LIST_NULL,
    EC_LIST_NODE_SIZE,
    EC_NODE_ALARM,
    EC_NODE_ERROR,
    EC_NODE_ADD_ERROR,
    EC_SERIAL_RECV_NULL,        //串口没有接收到数据
    EC_SERIAL_RECV_ERROR,       //10
    EC_SERIAL_RECV_TIMEOUT,
    EC_UPLK_CRC_ERROR,
    EC_VER_ERROR,
    EC_PROTOCOL_ERROR,
    EC_GW_ADDR_ERROR,
    EC_NULL_AFN,
    EC_NULL_NODE_ADDR,
    EC_CFG_ERROR,           //20
    EC_SERIAL_BUSY,
    EC_RESET,
    EC_REVAL,                //return node return value
    EC_COMM_FORMAT_ERROR,
    EC_REVAL_ERROR,
    EC_NODE_INFO_READ_ERROR,
    EC_NODE_INFO_WRITE_ERROR,
    EC_GW_INFO_READ_ERROR,
    EC_GW_INFO_WRITE_ERROR,
    EC_STIME_DEFAULT_WRITE_ERROR,   //30
    EC_STIME_DEFAULT_READ_ERROR,
    EC_ACK,
    EC_FALL_LINE,
    EC_DIMMING_VALUE_ERROR,
    EC_NOT_LEN_ENOUGH,
    EC_FRAME_DIR_ERROR,
}ERROR_CODE,_PTR_ ERROR_CODE_PTR;

/*************
* 全局调用接口函数
*************/
void print_sys_time(void);
void print_rtc_time(void);
void StrToHex(char *src, uchar *dst);
void ToString(uchar *src, char *dst, uint32_t);
#endif 
/* EOF */
