#include "global.h"
#include <timer.h>
#include "gw_serial.h"
#include "kwtj_GWII.h"
#include "rf_scan_task.h"
#include "uplink_data_type.h"
#include "internal_mem.h"

extern void wifi_upgrade_cosmos(char *, uint32_t);
_timer_id  	active_rfscan_timer_id;

static void LED_on
   (
      _timer_id id,
      void   *data_ptr,
      MQX_TICK_STRUCT_PTR tick_ptr
   )
{
   //rf_led_blink();
   user_task_create("port_scan_task");
   //_task_create(0, SCAN_TASK, 0);
   //_timer_cancel(active_rfscan_timer_id);
}

_timer_id active_rfscan_timer(void)//0xfe 
{
    MQX_TICK_STRUCT     active_rfscan_ticks;
	
	_timer_cancel(active_rfscan_timer_id);
	_time_get_ticks(&active_rfscan_ticks);
	//_time_add_sec_to_ticks(&active_rfscan_ticks, ); //1S*60=1min
	_time_add_min_to_ticks(&active_rfscan_ticks, 5);
	return _timer_start_oneshot_at_ticks(LED_on, 0, TIMER_KERNEL_TIME_MODE, &active_rfscan_ticks);
}

static void uplk_passthrough(char *buffer, uint32_t buflen)//需解析去掉帧头
{
//	uchar p[512]; 
//	
//	buflen = buflen-6+2;
//	p[0] = 0x80|buffer[7];
//	p[1] = buffer[8];
//	p[2] = buffer[5];
//	_mem_copy(buffer+9, p+3, buflen-6);
//	
//	rflk_wirte(p, buflen-6);
//	task_isr_readrf(&p[2], 15);
	
	uchar *p = _mem_alloc_zero(buflen-6+2);

	p[0] = 0x80|buffer[7];
	p[1] = buffer[8];
	p[2] = buffer[5];
	_mem_copy(buffer+9, p+3, buflen-6);
	
	rflk_wirte(p, buflen-6);
	task_isr_readrf(&p[2], 45);
	_mem_free(p);
}

static void uplkuart_passthrough(char *buffer, uint32_t buflen)
{
	rflk_wirte((uchar *)buffer, buflen);
	task_isr_readrf((uchar *)&buffer[2], 15);
}

/****** 该函数中可加入全局队列 ******
*	若不需要队列，该处直接开始解析数据
*
*/
/******
*	解析需要将实时命令和配置参数首先区别开来
*	接收实时命令前需要先发送终止节点轮询的命令，不发送后手动发送节点轮询命令，超时30分钟自动开始轮询
*	接收配置命令时处理时，注意对flash或其它存储介质互斥处理
*/
/*
* 使用队列处理数据放到下一步
*/
void uplink_recv_mix(char *buffer, uint32_t buflen, uint32_t socketID)
{
	/* Check zhe first byte of data frame */
	switch (buffer[0])
	{
		case	'@':
			while (_task_get_id_from_name("port_scan_task") != MQX_NULL_TASK_ID){
				user_task_abort("port_scan_task");
				_time_delay(5000);
			};
			if(_mutex_try_lock(&mutex_serial_rf) != MQX_EOK){
				printf("\n忙");
				return;
			}
			active_rfscan_timer_id = active_rfscan_timer();
			buffer[buflen]='\0';
			download_cfgmix(buffer);
			
			_mutex_unlock(&mutex_serial_rf);
			break;
		case	0x20:	
			/* 确定命令对象为本台Cosmos，Check UID */
			if((buffer[1] == (uint8_t)(CosmosInfoPtr->CosmosUid>>16))&&\
				(buffer[2] == (uint8_t)(CosmosInfoPtr->CosmosUid>>8))&&\
					(buffer[3] == (uint8_t)CosmosInfoPtr->CosmosUid)) 
			{
				while (_task_get_id_from_name("port_scan_task") != MQX_NULL_TASK_ID){
					user_task_abort("port_scan_task");
					_time_delay(5000);
				};
				if(_mutex_try_lock(&mutex_serial_rf) != MQX_EOK){
					printf("\n忙");
					return;
				}
				active_rfscan_timer_id = active_rfscan_timer();
			} else {
				printf("\nOld protocol Check UID error.");
				return;			
			}
			/* 兼容旧服务器下发指令透传 */
			switch (buffer[UPLK_CMD_COMMON_Apn_OFFSET])
			{
				case	UPLK_CFG_10_FUNC:
					printf("\ncfg 10");
					cfg_10_save(buffer+6);
					break;
				case	UPLK_CFG_51_FUNC:
					printf("\ncfg 51");
					cfg_51_save(buffer+6);
					break;
				case	UPLK_CFG_66_FUNC:
					printf("\ncfg 66");
					cfg_66_save(buffer+6);
					break;
				case	UPLK_CFG_45_FUNC:
					printf("\ncfg 45");
					break;
				case	UPLK_RLTIME_43_FUNC:
					printf("\nrltime 43");
					break;
				case	UPLK_RLTIME_46_FUNC:
					printf("\nrltime 46");
					break;
				case	UPLK_RLTIME_04_FUNC:
					printf("\nrltime 04");
					break;
				case	UPLK_RLTIME_03_FUNC:
					printf("\nrltime 03");					
					break;	
				case	UPLK_RLTIME_41_FUNC:
					printf("\nrltime 41");					
					break;	
				case	UPLK_RLTIME_44_FUNC:
					printf("\nrltime 44");					
					break;
				case	UPLK_UPGRADE_05_FUNC:
					printf("\nupgrade 05");
					wifi_upgrade_cosmos(buffer, buflen);
					_mutex_unlock(&mutex_serial_rf);
					return;
					//不进行下面的代码
					break;
				case	UPLK_UPGRADE_31_FUNC:
					printf("\nupgrade 31");
					//wifi_upgrade_rf(buffer+ );
					break;					
				default:
					printf("\nunknown cmd");
					break;
			}
			/* 兼容旧服务器下发指令透传 */
			uplk_passthrough(buffer, buflen);
			_mutex_unlock(&mutex_serial_rf);
			break;
		case	0x80:
			while (_task_get_id_from_name("port_scan_task") != MQX_NULL_TASK_ID){
				user_task_abort("port_scan_task");
				_time_delay(1500);
			};
			if(_mutex_try_lock(&mutex_serial_rf) != MQX_EOK){
				printf("\n忙");
				return;
			}
			
			buflen = buflen-2;/* 2: CRC_LEN */
			uplkuart_passthrough(buffer, buflen);
			
			_mutex_unlock(&mutex_serial_rf);
			break;
		default:
			/* 填充其它特殊指令类型 */
			printf("\nUnknow Cmd Head");
			break;
	}
	return;
}


