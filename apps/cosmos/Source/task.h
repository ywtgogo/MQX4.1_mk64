#ifndef	_TASK_H_
#define	_TASK_H_	1


enum Task_list
{
	MAIN_TASK = 1,
	UPLK_CHECK_TASK,
	SDCARD_TASK,//sdcard_task,
	UPLK_READ_TASK,
	UPLK_OFFLINE_TASK,
	RFLK_READ_TASK,
	RS485LK_READ_TASK,
	WIFILK_READ_TASK,
	SCAN_TASK,
	SHELL_TASK,
	DBLK_AT_TASK,
	HEARTBEAT_TASK,
	TEST,
};

void user_task_abort(char *task_name);
void user_task_create(char *task_name);


#endif