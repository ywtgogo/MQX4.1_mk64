#include "global.h"
#include "task.h"

void user_task_abort(char *task_name)
{
	_task_id	task_id;
	task_id = _task_get_id_from_name(task_name);
	if (task_id != MQX_NULL_TASK_ID){
		_task_abort(task_id);
		printf("\nTermination of this task: %s", task_name);
	}else{
		printf("\nThe task is not running: %s", task_name);
	}
}

void user_task_create(char *task_name)
{
	_task_id	task_id;
	task_id = _task_get_id_from_name(task_name);
	if (task_id == MQX_NULL_TASK_ID){
		//此处可解析出内容，创建被输入参数对应的任务
		_task_create(0, SCAN_TASK, 0);
		printf("\nCreate a task successfully: %s", "port_scan_task"/*task_name*/);
	}else{
		printf("\nAlready in the task: %s", "port_scan_task"/*task_name*/);
	}
}