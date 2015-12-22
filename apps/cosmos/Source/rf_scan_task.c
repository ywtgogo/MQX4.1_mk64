#include "global.h"
#include <fio.h>
#include "rf_scan_task.h"
#include "internal_mem.h"

#define SINGLE_DEV_OFFSET	2
#define NODE_LEN			2




//typedef struct FFD_ID
//{
//	uint16_t ffdid;
//}FFD_ID_t, *FFD_ID_PTR;
//
//typedef struct RFD_ID
//{
//	uint16_t rfdid;
//}RFD_ID_t, *RFD_ID_PTR;
//
//////先实现轮询等功能，有空加入链表运行
////typedef struct FFD_RFD{
////	uint16_t *ffddevptr;
////	uint16_t *rfddevptr;
////	uint8_t  num;
////};
////static uint16_t 	ffd_index = 0;
//uint8_t ffdptr[] = {0x01, 0xa0, 0x00, 0xb0, 0x00, 0xc0, 0x00, 0xd0};//路由表中FFD的排序
//uchar routetab0[] =
//{
//	0x80, 0x01, 0x51, 00, 0x0a,
//	00, 0x1a, 00, 0x0b, 00, 00, 00, 00, 00, 0x01, 00, 0x01,
//	00, 0x0b, 00, 0x0c, 00, 00, 00, 00, 00, 0x1a, 00, 0x02,
//	00, 0x0c, 00, 0x0d, 00, 00, 00, 00, 00, 0x0b, 00, 0x03,
//	00, 0x0d, 00, 00, 00, 00, 00, 00, 00, 0x0c, 00, 0x04,
//	00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
//	00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
//	00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
//	00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
//};
//
//FFD_ID_PTR ffddevptr = (FFD_ID_PTR)(&ffdptr[0]);//指向接收缓存或flashx缓存
//FFD_ID_t ffddev[4];//用于转换到16bit过滤和比较,初始化时对空间大小赋值
//
////uint8_t rfdptr[]={0x00,0x11,0x00,0x12, 0x00,0x13, 0x00,0x14, 0x00,0x21 ,0x00,0x22, 0x00,0x23 ,0x00,0x24,  0x00,0x31, 0x00,0x32, 0x00,0x33 ,0x00,0x34,  0x00,0x41 ,0x00,0x42 ,0x00,0x43 ,0x00,0x44};
//uint8_t rfdcellptr[]={0x00,0x11,0x00,0x12, 0x00,0x13, 0x00,0x14};//需要从完整存储rfd的内存段中读取每FFD下的RFD，数量由51路由表提供
////RFD_ID_PTR rfddevptr = (RFD_ID_PTR)rfdcellptr;
//RFD_ID_t rfddev[50];//初始化时赋值，大小来源于配置信息

static uint32_t rf_scan(uchar *ffdnet_ptr)
{
	uchar 		did[2] ;//= {0x00,0x01};
	uint32_t	result = 0;
	uchar 		cmd_apn;
	
	did[0] = 0x80|PhubInfoPtr->RfCfg.CooId;
	did[1] = PhubInfoPtr->RfCfg.CooId>>8;
	
	if(_mutex_try_lock(&mutex_serial_rf) != MQX_EOK){
		printf("\nrf_port 忙");
		return -1;
	}
	cmd_apn = 0x43;
	cmd_sync(did, ffdnet_ptr, &cmd_apn);
	cmd_apn = 0x46;
	cmd_realtime(did, ffdnet_ptr, &cmd_apn);
	cmd_apn = 0x04;
	cmd_sample(did, ffdnet_ptr, &cmd_apn);
	
	_mutex_unlock(&mutex_serial_rf);
	
	return result;
}

///*
//Function:If the rf_cmd task is in the waiting the first serial byte routine ,
//then this function will set the event bit EVENT_BIT_QUIT_WAITING to 1 to make the routine quit
//*/
//void quit_if_waiting_uart_rx()
//{
//	//EVENT_BIT_QUIT_WAITING
//	_mqx_uint task_wait_count;
//	task_wait_count = _event_get_wait_count(event_ptr_gateway_rx);
//	if(task_wait_count>0){//Mean that the task is waiting for the event
//		_event_set(event_ptr_gateway_rx,EVENT_BIT_QUIT_WAITING);
//	}
//}

///*
//Function:在delay_scan_task中，设定此函数为_timer_start_oneshot_after设定的时间到了之后，
//*/
//void timer_notif_func(_timer_id timer, void *param, uint32_t vl1, uint32_t vl2){
//	_lwsem_post(&scan_task_run);
//	is_scan_task_waiting = false;
//}
//
///*
//Call this function to delay the port_scan_task
//原理：
//如果当前Scan_task是处于等待状态中的即is_scan_task_waiting==true，则在进入此函数的时候，将原来的等待进程取消，而再重置一个新的等待进程。
//如果不是等待状态，则仅仅是进行一个新的等待进程。
//这里说的等待，就是在收到了Tele的指令之后，将Scan暂停一段时间后重新启动。
//*/
//void delay_scan_task(uint32_t delay_time){
//	//退出gateway的waiting过程
//	quit_if_waiting_uart_rx();
//	if(is_scan_task_waiting == false){
//		if(_lwsem_wait(&scan_task_run) == MQX_OK){
//			delay_timer_to_restart = _timer_start_oneshot_after(timer_notif_func,0,TIMER_KERNEL_TIME_MODE,delay_time);
//			is_scan_task_waiting = true;
//		}
//	}else{
//		_timer_cancel(delay_timer_to_restart);
//		delay_timer_to_restart = _timer_start_oneshot_after(timer_notif_func,0,TIMER_KERNEL_TIME_MODE,delay_time);
//	}
//}
//
//void
//{
//	/* Create a timer component */
//	if (_timer_create_component(TIMER_DEFAULT_TASK_PRIORITY,1024)!= MQX_OK){
//		printf("\nCount not create a timer [in Scan task]\n");
//		_task_block();
//	}
//}


void port_scan_task(uint32_t parameter)
{
	uint8_t 			result;
	uint8_t 			step;
	uint8_t 			ffd_count=0, rfd_count;
	uint8_t				*ffdptr;
	static uint8_t		ffd_index=0;
	
	_time_delay(1000);
	if (CosmosInfoPtr->InitHead&INIT_HEAD_COSMOS_CMDDELAY\
			&&CosmosInfoPtr->InitHead&INIT_HEAD_PHUB_INFO\
				&&CosmosInfoPtr->InitHead&INIT_HEAD_SUBNET\
					&&CosmosInfoPtr->InitHead&INIT_HEAD_COSMOS_INFO){
		printf("\nPolling...");
	}else{
		printf("\nSome information deficiency, Please reset 0x%08x", CosmosInfoPtr->InitHead);
		_task_block();
	}
	
	while(1)
	{
		//ffdptr = (uint8_t*)SubNetPtr+sizeof(SUBNET_CELL_t);
		//result = rf_scan(ffdptr, SINGLE_DEV_OFFSET*ffd_index);
		result = rf_scan((uchar *)SubNetPtr->SubNetCell + sizeof(SUBNET_CELL_t)*ffd_index);
		if(result == 0)
		{
			ffd_index++;
			if (ffd_index >= SubNetPtr->CellCount/*sizeof(SUBNET_t)/sizeof(SUBNET_CELL_t)*/)
				ffd_index = 0;
		}
		//FFD 之间的延时，参数需外部读取
		_time_delay(CmdDelayPtr->FfdNext*1000);
	}
}


