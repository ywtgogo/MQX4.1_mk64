/*****************************************************************************
*
* Comments:
* 启动全局任务，用于启动执行过程中所必须全程run的任务
*   
*
*
*END************************************************************************/

#include "global.h"
#include "task.h"

#if ! MQX_HAS_TIME_SLICE
#error This application requires MQX_HAS_TIME_SLICE defined non-zero in user_config.h. Please recompile PSP and BSP with this option.
#endif
#include <timer.h>

#include "kwtj_GWII.h"
#include "spinorshell.h"
#include "internal_mem.h"

#define TIMER_USER_TASK_PRIORITY   (12)
#define TIMER_USER_STACK_SIZE      (1024+512)

void main_task(uint32_t   parameter)
{	

	printf("\r\n---> Cosmos Ver %06x ", VERNO);
	printf("\r\n---> Build %s ", VERTIME); 
	print_rtc_time();
	print_sys_time();
	
	gw_gpio_init();
	nftl_init();
	flashx_init();
	gw_serial_init();
	gw_sync_init();					//同步信号

	_timer_create_component(TIMER_USER_TASK_PRIORITY, TIMER_USER_STACK_SIZE);
	
	_task_create(0,SHELL_TASK,0);
	if (CosmosInfoPtr->FuncEn&FUNC_EN_ETH) {
		_task_create(0,UPLK_CHECK_TASK,0);
	}else {
		_task_create(0,WIFILK_READ_TASK,0);	
	}

//	_task_create(0,UPLK_READ_TASK,0);
//	_task_create(0,RFLK_READ_TASK,0);
//	_task_create(0,RS485LK_READ_TASK,0);
//	_task_create(0,WIFILK_READ_TASK,0);
//	_task_create(0,HEARTBEAT_TASK,0);

	_task_block();
}


void test_task(uint32_t   parameter)
{
//	uint32_t handle;
//	char buffer[20000];
//	uint32_t count;
//	
//	count = send(handle, buffer, 20000, 0);
//	if (count == RTCS_ERROR)
//		printf(“\nError, send() failed with error code %lx”, RTCS_geterror(handle));
	
	
	_task_block();
}

/* End of File */
