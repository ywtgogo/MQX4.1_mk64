#ifndef _RF_SCAN_H_
#define _RF_SCAN_H_



uint32_t cmd_sync(uchar *destid, uchar *targid, uchar *cmd_apn);
uint32_t cmd_realtime(uchar *destid, uchar *targid, uchar *cmd_apn) ;
uint32_t cmd_sample(uchar *destid, uchar *targid, uchar *cmd_apn);
void user_task_abort(char *task_name);
void user_task_create(char *task_name);

#endif 