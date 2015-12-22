#include "global.h"
#include <Charq.h>
#include <ipcfg.h>
#include "crc16_code.h"
#include "gw_serial.h"
#include "uplink_data_type.h"
#include "kwtj_GWII.h"
#include "rf_type_analysis.h"
#include "rf_type_cmd.h"

extern void cmd_log(uint8_t *data, uint32_t len, uchar);

volatile CHARQ_STRUCT_PTR             in_queue_dnlk_read;

void serial_dnlk_read_init()
{
    in_queue_dnlk_read = (void *)_mem_alloc_zero(sizeof(CHARQ_STRUCT) - (4 * sizeof(char)) + SERIAL_DNLk_QUEUE_SIZE);
    _CHARQ_INIT(in_queue_dnlk_read,SERIAL_DNLk_QUEUE_SIZE);
}

/* 若485接口存在主动上报数据，则需要为自动执行的任务 */
void rs485lk_read_task(uint32_t	parameter)
{
	uint32_t 	count;
	uchar	 	buff[1024];

    while(1)
    {
		_time_delay(500);
		if (!fstatus(fd_ittye_485_ptr))
		{
			continue;
		}
		_time_delay(30);
		//count = serial_ioctl(fd_ittyf_485_ptr, IO_IOCTL_SERIAL_GET_LEN, NULL);		
		count = fread(buff, 1, RS485_BUF_SIZE, fd_ittye_485_ptr);
		if(count > 0)
		{
			//count = read(fd_ittyf_485_ptr, buff, count);
			//count = fread(rxbuff, 1, RS485_BUF_SIZE, fd);
			//printf("\ncount = %d", count);
			//test 将485的数据传入上行接口
			//校验CRC并删除CRC长度

			uplink_recv_mix((char *)buff, count, 0);
		}
    }
}

uint32_t isr_readrf(uchar *rxbuff, uchar *cmd_apn, uint8_t min_delays, uint8_t max_delays)
{
	MQX_FILE_PTR	fd = NULL;
    uchar			tempbuff[RF_BUF_SIZE];	
	uint32_t		count;
	uint32_t		result;
	uint8_t  		times = max_delays-min_delays;
	fd = fopen("ittyf:", (const char *)(IO_SERIAL_RAW_IO|IO_SERIAL_NON_BLOCKING));
	if (fd == NULL)
	  	printf("\n ittye_rf init error");
	_time_delay(min_delays*1000);
	while (times){
		if (fstatus (fd)){
			count = fread(rxbuff, 1, RF_BUF_SIZE, fd);
			if (count>18&&count<827) {//轮询接收采样数据可能会发生一次读取不完整
				
				printf("\n Incomplete data reception, Continue reading... count=%d", count);
				_time_delay(3000);
				result = fread(tempbuff, 1, RF_BUF_SIZE, fd);
				_mem_copy(tempbuff, rxbuff+count, result);
				count = count+result;
				printf("\n Incomplete data reception, Continue reading... count=%d, tempbuff=%d", count, result);
			}
			//向rs485接口透传帧
			//rs485lk_wirte(rxbuff, count);
			//帧解析			
			result = rf_mix_analysis(rxbuff, count);
			switch (result)
			{
			case EC_OK:
				break;
			case EC_ERROR:
				break;
			defualt:
				break;
			}
			break;
		}else{
			_time_delay(1000);
			times--;
			if(0 == times){
				rf_outoftime(cmd_apn, max_delays);
			   	break;		
			}
			continue;
		}
	}
	if(fd != NULL)
		fclose(fd);
	fd = NULL;
	return count;
}


//uart4 isr read rf-gateway data 
//rf_mux_analysis analyze recvdate decode afn...
//write recvdata to uplink
uint32_t task_isr_readrf(uchar *cmd_apn, uint8_t delay_s)
{
	MQX_FILE_PTR fd = NULL;
    uchar rxbuf[RF_BUF_SIZE];
	uint32_t result;
    uint16_t count = 0;	
	uint8_t  times = delay_s;
	fd = fopen("ittyf:", (const char *)(IO_SERIAL_RAW_IO|IO_SERIAL_NON_BLOCKING));
	if (fd == NULL)
	  	printf("\n ittye_rf init error");
	while (times){
		if (fstatus (fd)){
			count = fread(rxbuf, 1, RF_BUF_SIZE, fd);
			/* 帧解析 */			
			result = rf_mix_analysis(rxbuf, count);
			/* 解析后的发送应放在此处，借用result判断状态 */
			switch (result)
			{
			case EC_OK:
				break;
			case EC_ERROR:
				break;
			defualt:
				break;
			}
			break;
		}else{
			_time_delay(1000);
			times--;
			if(0 == times){
				result = EC_ERROR;
				rf_outoftime(cmd_apn, delay_s);
			   	break;		
			}
			continue;
		}
	}
	if(fd != NULL)
		fclose(fd);
	fd = NULL;
	return result;
}
//uart5 write data to rf-gateway
uint32_t rflk_wirte(uchar *buff, uint16_t len) 
{
    MQX_FILE_PTR 	fd = NULL;
    uint32_t 		count = 0;
    uint16_t 		i;
	uint16_t 		crc;
	
    /* Prepare serial port */
    fd = fopen("ittyf:", (const char *)(IO_SERIAL_RAW_IO));  //"w"
    if (fd == NULL)
    {
        fprintf(stderr, "rflk_wirte Fatal Error: Unable to open device %s.\n", fd);
        _task_block();
    }
	//计算CRC
	crc = CRC16Calc(buff, len);
	buff[len] = crc>>8;//(uint8_t)((crc>>8)&0x00ff);
	buff[len+1] = crc;//(uint8_t)(crc&0x00ff);		
	
	print_sys_time();
	printf("\n RFSEND: ");
	for (i=0 ;  i<len+2; i++ )
	{
	   printf("%.2x ", buff[i]);
	}
	serial_ioctl(fd,IO_IOCTL_SERIAL_CLEAR_BUF,NULL);
	//写入到spi花费时间较长，需要变更存入的方式
	//printf("\n%s---%d",__FILE__, __LINE__);
	cmd_log(buff, len+2, 1);
	//printf("\n%s---%d",__FILE__, __LINE__);
	count = fwrite(buff, 1, len+2, fd);	
	//printf("\ncount = %d \n", count);
	if (fd != NULL)
		fclose(fd);
	fd = NULL;
	return count;
}

uint32_t rs485lk_wirte(uchar *buff, uint16_t len)
{
    MQX_FILE_PTR fd;
    uint32_t count = 0;
        
    /* Prepare serial port */
    fd = fopen("ittyf:", (const char *)(IO_SERIAL_RAW_IO));  //"w"
    if (fd == NULL)
    {
        fprintf(stderr, "RXtask Fatal Error: Unable to open device %s.\n", fd);
        _task_block();
    }
	/* Write buff from uart5*/
	count = fwrite(buff, 1, len, fd);	
//	printf("\ncount = %d \n", count);
	if (fd != NULL)
		fclose(fd);
    _time_delay(10);
	return 0;
}
/* Wifi 预留uart接口
void wifilk_read_task(uint32_t	parameter)
{
	uint32_t count;
	char	 buff[1024];
    while(1)
    {
		_time_delay(100);
		if (!fstatus(fd_ittya_wifi_ptr))
		{
			continue;
		}
		count = serial_ioctl(fd_ittya_wifi_ptr, IO_IOCTL_SERIAL_GET_LEN, NULL);
		if(count > 0)
		{
			count = read(fd_ittya_wifi_ptr, buff, count);
			count = send(clinet_primarysock, buff, count, 0);			
			if (count == RTCS_ERROR)
			{
				shutdown(clinet_primarysock, 0);
			}
		}
    }
}
*/