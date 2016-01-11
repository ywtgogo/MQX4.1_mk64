#include "global.h"
#include "task.h"
#include <mfs.h>
#include "uplink_data_type.h"
#include "kwtj_GWII.h"
#include "CRC16_CODE.h"
#include "internal_mem.h"

//全局标志位，表明WIFI模块是否正常连接TCP1
bool WifiTcp1Stat = FALSE;
bool WifiTcp2Stat = FALSE;
bool WifiUpgradeNow = FALSE;

extern int32_t MFS_filesync(MQX_FILE_PTR);
//上电时，状态灯是否默认为高电平？需要再次确认
bool wifi_cfg_set(void)
{
	lwgpio_set_value(&wifi_cfgset, LWGPIO_VALUE_LOW);
	lwgpio_set_value(&wifi_reset, LWGPIO_VALUE_LOW);
	//lwgpio_set_value(&wifi_power, LWGPIO_VALUE_LOW);
	_time_delay(2000);
	//lwgpio_set_value(&wifi_power, LWGPIO_VALUE_HIGH);
	lwgpio_set_value(&wifi_reset, LWGPIO_VALUE_HIGH);
	printf("\nWIFI正在进入配置");
	for(int i=0; i<15; i++)
	{
		_time_delay(1000);
		//检测设置完成的管脚，低-进入设置，高继续等待
		if(lwgpio_get_value(&wifi_cfgget)) {
			//printf("\nWIFI正在进入配置");
			continue;
		}else {
			lwgpio_set_value(&wifi_cfgset, LWGPIO_VALUE_HIGH);
			//是否需要对WIFI重启动
			printf("\nWIFI进入配置模式成功，请使用手机APP对设备发送配置信息");
			return TRUE;
		}
	}
	lwgpio_set_value(&wifi_cfgset, LWGPIO_VALUE_HIGH);
	printf("\nWIFI进入配置失败，请重新对WIFI上电");
	return FALSE;
}

bool wifilk_check(void)
{
	static	bool	last = FALSE;
	if (LWGPIO_VALUE_LOW == lwgpio_get_value(&wifi_cfgget)) {
		printf("\nWIFI正在配置");
		_time_delay(1000);
		return FALSE;
	}
	if (LWGPIO_VALUE_LOW == lwgpio_get_value(&wifi_ap)) {
		//printf("\nWIFI未连接AP");
		lwgpio_set_value(&gw_plug_led, LWGPIO_VALUE_HIGH);
		lwgpio_set_value(&gw_ser_led, LWGPIO_VALUE_HIGH);
		return FALSE;
	}
	lwgpio_set_value(&gw_plug_led, LWGPIO_VALUE_LOW);
	if (LWGPIO_VALUE_LOW == lwgpio_get_value(&wifi_preftcp)) {
		last = FALSE;
		//printf("\nWIFI未连接TCP1");
		lwgpio_set_value(&gw_ser_led, LWGPIO_VALUE_HIGH);
		return FALSE;
	}else {
		if(last != TRUE) {
			//printf("\nWIFI重连接TCP1");
			lwgpio_set_value(&gw_ser_led, LWGPIO_VALUE_LOW);
			last = TRUE;
			printf("\n向TCP1发送注册包");
			uplk_send_connreg();
			/* check "last" to send offline datalog */
			_time_delay(500);
			_task_create(0,UPLK_OFFLINE_TASK,0);
		}
	}
	
	if (LWGPIO_VALUE_LOW == lwgpio_get_value(&wifi_secdtcp)) {
		//printf("\nWIFI未连接TCP2");//第二路不能被客户发现
		//return FALSE;
	}
	return TRUE;
}

uint32_t wifilk_wirte(uchar *wbuf, uint16_t len) 
{
    uint32_t 		count = 0;
	
	print_sys_time();
	printf("\n WIFISEND: ");
	for (int i=0 ;  i<len; i++ ) {
	   printf("%.2x ", wbuf[i]);
	}
	printf("\n ");
	count = fwrite(wbuf, 1, len, fd_ittya_wifi_ptr);
	if (len != count) {
		printf("\nUart of wifi send error");
	}
	return count;
}

void wifilk_read_task(uint32_t temp)
{
	uint32_t 	count;
	char	 	buff[1024];
	uint16_t	beat_times;
	uint32_t	timedata;

	/* check Uid */
	/* while(1) if not set Uid  */
	while(1) {
		if (CosmosInfoPtr->InitHead&INIT_HEAD_COSMOS_INFO){
			printf("\n\nWIFIlk task runing...");
			break;
		}else{
			printf("\nPlease config Cosmos UID 0x%08x", CosmosInfoPtr->InitHead);
			_time_delay(5000);
			continue;
		}
	}
	
	_task_create(0,HEARTBEAT_TASK,0);
	
	if (CosmosInfoPtr->FuncEn&FUNC_EN_RF) {
		_task_create(0,SCAN_TASK,0);
	}else {
		printf("\nFUNC_RF has been set disable.");
	}
#if 0
	if (CosmosInfoPtr->FuncEn&FUNC_EN_RS485) {
		_task_create(0,RS485LK_READ_TASK,0);
	}else {
		printf("\nFUNC_RS485 has been set disable.");
	}
#endif
	
    while(1)
    {
		/* 用于检查GPIO的置位情况，同步记录log数据和发送旧的离线数据 */
		WifiTcp1Stat = wifilk_check();
		if (FALSE == wifilk_check()) {
			/* 设置离线标志 */
			WifiTcp1Stat = FALSE;
		}else {
			/* 设置在线标志 */
			WifiTcp1Stat = TRUE;
		}
		_time_delay(220);
		if (!fstatus(fd_ittya_wifi_ptr))
		{
			continue;
		}

		_time_delay(30);		
		count = fread(buff, 1, 1024, fd_ittya_wifi_ptr);
		
		print_sys_time();
		printf("\n WIFIRECV: ");
		for (int i=0 ;  i<count; i++ ) {
		   printf("%.2x ", buff[i]);
		}		
		if (count == 0) {
			continue;
		}else if (count == 8 || count == 16) {
			/* 心跳包返回的Ser包 */
			printf("\nHeartbeat Packet\n");
			if (beat_times%60 == 0) { //一分钟一次心跳包，60，六十次心跳包接收处理记录一次时间
			/* 处理从透传wifi传入的时间包数据 */
				timedata = (buff[4]<<24)|(buff[5]<<16)|(buff[6]<<8)|buff[7];
				rtctime_set(timedata);
				print_rtc_time();
				print_sys_time();
			}
			beat_times++;
			continue;
		}else {
			uplink_recv_mix((char *)buff, count, 0);
		}
    }
}

#define	UPGRADE_CELL_DATA_SIZE	543
#pragma	pack(1)
typedef struct COSMOS_FW_SEND_TYPE
{
	uint32_t	Uid;
	uint16_t	Fun;
	uint16_t	RspFN;
	uint16_t	ReqFN;
}COSMOS_FW_SEND_TYPE_t,* COSMOS_FW_SEND_TYPE_PTR;

typedef struct COSMOS_FW_PROTOCOL_TYPE
{
	uint32_t	Uid;
	uint16_t	Fun;
	uint16_t	RspFN;
	uint16_t	DataLen;
	uint8_t		DataFile[UPGRADE_CELL_DATA_SIZE];
	uint16_t	Crc;
}COSMOS_FW_PROTOCOL_TYPE_t,* COSMOS_FW_PROTOCOL_TYPE_PTR;
#pragma	pack()
/*
S->C	2000011c 02 05
接收到服务器指令，Cosmos从下一步骤开始文件请求
C->S	2000011c 02 05 0000(开始序号)  0001(请求序号) 
S->C	2000011c 02 05 0001(数据序号)  0200(数据长度) (512B的文件内容) CRC(对512B内容做校验)
C->S	2000011c 02 05 0001(完成序号)  0002(请求序号) 
S->C	2000011c 02 05 0002(数据序号)  0200(数据长度) (512B的文件内容) CRC(对512B内容做校验)
C->S	2000011c 02 05 0002(完成序号)  0003(请求序号)
S->C	2000011c 02 05 0003(数据序号)  0200(数据长度) (512B的文件内容) CRC(对512B内容做校验)
C->S	2000011c 02 05 0003(完成序号)  0004(请求序号)
S->C	2000011c 02 05 0004(数据序号)  0200(数据长度) (512B的文件内容) CRC(对512B内容做校验)
... ...
C->S	2000011c 02 05 0153(完成序号)  0154(请求序号)
S->C	2000011c 02 05 0154(数据序号)  0013(数据长度) ( <512B文件内容) CRC(对<512B内容做校验)
C->S	2000011c 02 05 0154(完成序号)  ffff(结束序号)
Cosmos当检测到某个文件块小于512B时，结束文件写入，并向服务器发送结束的信号，用(0xffff)填充请求序号位
*/
extern int32_t  Shell_update(int32_t argc, char *argv[] );
void wifi_upgrade_cosmos(char *r_frame_ptr, uint32_t len)
{
	static	char 			update_path[] = "c:/x.mbt";
	static	MQX_FILE_PTR	update_fd = NULL;
	char	*param[3] 	  = {"update", "new", "c:/x.mbt"};
	uint16_t		crc_offset;
	uint16_t		crc;
	
	COSMOS_FW_PROTOCOL_TYPE_t 	cosmos_fw_protocol_frame;
	COSMOS_FW_SEND_TYPE_t		cosmos_fw_send_frame;
	_mem_copy(r_frame_ptr, &cosmos_fw_protocol_frame, sizeof(COSMOS_FW_PROTOCOL_TYPE_t));
	crc_offset = sizeof(COSMOS_FW_SEND_TYPE_t)+htons(cosmos_fw_protocol_frame.DataLen);
	cosmos_fw_protocol_frame.Crc=r_frame_ptr[crc_offset+1]|r_frame_ptr[crc_offset]<<8;
	
	cosmos_fw_send_frame.Uid 	= cosmos_fw_protocol_frame.Uid;
	cosmos_fw_send_frame.Fun 	= cosmos_fw_protocol_frame.Fun;
	cosmos_fw_send_frame.RspFN 	= cosmos_fw_protocol_frame.RspFN;
	cosmos_fw_send_frame.ReqFN 	= htons(htons(cosmos_fw_protocol_frame.RspFN)+1);
	
	if (len == 6) {
		WifiUpgradeNow = TRUE;
		cosmos_fw_send_frame.RspFN 	= 0x0000;
		cosmos_fw_send_frame.ReqFN 	= htons(0x0001);
		/* 发送初始化请求 */
		uplk_send((char *)&cosmos_fw_send_frame, sizeof(COSMOS_FW_SEND_TYPE_t));
		return;
	}else if (len < (UPGRADE_CELL_DATA_SIZE+12)) {
		crc = CRC16Calc(cosmos_fw_protocol_frame.DataFile, htons(cosmos_fw_protocol_frame.DataLen) );
		if (cosmos_fw_protocol_frame.Crc == crc/* CRC检查正确*/) {
			WifiUpgradeNow = FALSE;
			cosmos_fw_send_frame.ReqFN 	= 0xFFFF;
			/* 发送结束信号 */
			uplk_send((char *)&cosmos_fw_send_frame, sizeof(COSMOS_FW_SEND_TYPE_t));
			goto file_rw;
		}else {
			cosmos_fw_send_frame.RspFN = htons(htons(cosmos_fw_protocol_frame.RspFN)-1);
			cosmos_fw_send_frame.ReqFN = cosmos_fw_protocol_frame.RspFN;
			/* 发送重发信号*/
			uplk_send((char *)&cosmos_fw_send_frame, sizeof(COSMOS_FW_SEND_TYPE_t));
			
			return;
		}
	}
	/* 发送正常交互结果 */
	uplk_send((char *)&cosmos_fw_send_frame, sizeof(COSMOS_FW_SEND_TYPE_t));

file_rw:
	update_fd = fopen(update_path, "a+");
	if (update_fd == NULL){
		printf("\nopen %s error\n", update_path);
		/* 需加入删除错误文件的代码 */
		return;
	}
	
	if (fseek(update_fd, UPGRADE_CELL_DATA_SIZE*(htons(cosmos_fw_protocol_frame.RspFN)-1), IO_SEEK_SET ) != IO_ERROR) {
		if (fwrite(cosmos_fw_protocol_frame.DataFile, 1, htons(cosmos_fw_protocol_frame.DataLen), update_fd)!=htons(cosmos_fw_protocol_frame.DataLen)) {
			/* incomplete write */
			printf("Error writing file %s.\n", update_path );
			if (ferror(update_fd) == MFS_DISK_FULL) printf("Disk full.\n" );
		}
	} else {
		printf("\nError, unable to seek file %s.", update_path );
	}
	fclose(update_fd);
	MFS_filesync(update_fd);
	update_fd = NULL;
	if (cosmos_fw_send_frame.ReqFN 	!= 0xFFFF) return;

	/* 开始固件升级 */
	Shell_update(3, param);
	/* update内含重启Cosmos */
	
}

//   /* Install uart Rx callback function */
//   ioctl(wifi_uart_dev, IO_IOCTL_SERIAL_INSTALL_RX_CALLBACK,(void*)wifi_uart_rx_callback) ;
//
//    /* setup event */
//    if (_event_create("wifilk_rx") != MQX_OK) {
//        printf("\nCreate event failed[wifilk rx evnet]");
//        _mqx_exit(0);
//    }
//    
//    if (_event_open("wifilk_rx", &event_ptr_wifilk_rx) != MQX_OK) {
//      printf("\nOpen event failed");
//      _mqx_exit(0);
//    }
//
//	void wifilk_uart_rx_callback()
//	{
//	   _event_set(event_ptr_wifilk_rx,EVENT_BIT_GATEWAY_RX); 
//	}




