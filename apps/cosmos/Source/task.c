/*HEADER**********************************************************************
*
* Comments:
*
*   
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

#include "task.h"


#define MAIN_STACK 	4096

extern void main_task(uint32_t);
extern void shell_task(uint32_t);
extern void uplk_read_task(uint32_t);
extern void rflk_read_task(uint32_t);
extern void rs485lk_read_task(uint32_t);
extern void wifilk_read_task(uint32_t);
extern void port_scan_task(uint32_t);
extern void heart_beat_task(uint32_t);
extern void uplk_check_task(uint32_t);
extern void dblk_at_task(uint32_t);
extern void uplk_offline_task(uint32_t);
extern void test_task(uint32_t);
extern void sdcard_task(uint32_t);
/*   Task Templates  */ 

const TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{   
	// Task Index,    		Function,       	Stack,      Priority, 	Name,             	Attributes,         	Param,  Time Slice 
	{ MAIN_TASK,     		main_task,      	MAIN_STACK,	11,      	"main_task",      	MQX_AUTO_START_TASK,    0,      0 },
	{ SHELL_TASK,    		shell_task,     	1024*6,  	12,         "Shell",    		0,					    0,      0 },//MQX_AUTO_START_TASK
	{ DBLK_AT_TASK,			dblk_at_task,		1024*6,		12,			"dblk_at_task",		0,						0,		0 },
	{ UPLK_READ_TASK,		uplk_read_task, 	1024*12,	14, 	  	"uplk_read_task", 	0,			 		   	0,      0 },
	{ UPLK_OFFLINE_TASK,	uplk_offline_task,	1024*8,		14,			"uplk_offline_task",0,						0,		0 },
	{ SCAN_TASK, 			port_scan_task, 	1024*12,	13,			"port_scan_task", 	0,						0,		0 },
	{ RS485LK_READ_TASK,	rs485lk_read_task,	1024*8,		15, 	  	"rs485lk_read_task",0,						0,      0 },
	{ UPLK_CHECK_TASK,		uplk_check_task, 	1024*4,		16,			"uplk_check_task",	0,						0,		0 },
	{ WIFILK_READ_TASK,		wifilk_read_task,	1024*8,	 	14, 	  	"wifilk_read_task", 0,						0,      0 },
	{ HEARTBEAT_TASK,		heart_beat_task, 	1024*5,		16,			"heart_beat_task",	0,						0,		0 },
	{ SDCARD_TASK,			sdcard_task,  		1024*2,     16,     	"sdcard", 			MQX_AUTO_START_TASK,	0,		0 },
	{ TEST,					test_task,			1024*4,		17,			"test_task",		0,						0,		0 },
	{ 0 }
};


/* End of File */
