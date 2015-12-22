#include "global.h"
#include "task.h"
#include "gw_serial.h"

extern uint32_t download_cfgmix(char *r_frame_ptr);

void dblk_at_task(uint32_t parameter)
{
	MQX_FILE_PTR         COMMAND_FP;
	char 				buf[128];
	bool				EXIT=FALSE;
	COMMAND_FP = stdin;
//	TD_STRUCT_PTR 		td_ptr;
	
	printf("\nSwitch to debug mode succeed.\n");
	while(!EXIT)
	{
		do {
			if (!fgets(buf, sizeof(buf), COMMAND_FP)) {
			   if (COMMAND_FP != stdin)  {
				  fclose(COMMAND_FP);
				  COMMAND_FP = stdin;
				  printf("\n");
			   }else  {
                  EXIT=TRUE;
				  /* 退出且重新启动DBLK */
				  break;
			   }
			}
		} while ((buf[0] == '\0') && (COMMAND_FP == stdin)) ; 

		if (!strncmp(buf, "dbquit", 6)) {
			_task_create(0, SHELL_TASK, 0);
			EXIT=TRUE;
		}else{
			printf("stdin:%s\n", buf);
			/* 进入接收cfg处理函数 */
			download_cfgmix(buf);
		}
	}
	
//	td_ptr = _task_get_td(_task_get_id_from_name("Shell"));
//	_int_disable();
//	if ((td_ptr != NULL) && (td_ptr->STATE == WAIT_BLOCKED)){
//	_task_ready(td_ptr);
//	}
//	_int_enable();
}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
//	MQX_FILE_PTR	fd_dblk;
//	uint16_t 		count;
//	uchar	 		buff[DBLK_BUF_SIZE];
//	fd_dblk = fopen("ittyd:",(void *)(IO_SERIAL_RAW_IO)); 
//	if(NULL == fd_dblk)
//	{
//		printf("\nittyd: init error");
//	}
//	
//	while(1)
//	{
//		_time_delay(500);
//		if (!fstatus(fd_dblk))
//		{
//			continue;
//		}
//		_time_delay(30);	
//		count = fread(buff, 1, DBLK_BUF_SIZE, fd_dblk);
//		if(count > 0)
//		{
//			printf("\ncount = %d", count);
//
//			//uplink_recv_mix((char *)buff, count, 0);
//		}
//	}