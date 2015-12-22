/***********************************************************
*
*log.c 中系统自带LOG放弃修改 需要做一个数组转换字符串接口 20150702
*
***********************************************************/


#include "global.h"
#include <mfs.h>

extern int32_t MFS_get_freesize(MQX_FILE_PTR);
extern int32_t MFS_filesync(MQX_FILE_PTR);
//retrun: nftl status
//call: systime,fsctrlfun
//brief: write data sample to nftl 
uint32_t data_log(uchar *logdata, uint16_t len)
{
	TIME_STRUCT 	mqx_time;
	DATE_STRUCT 	mqx_date_time;
	
	uint32_t 		freesize;
	uint32_t 		rlen;
	int32_t  		error = 0;
	char 			logfilename[16];
	char 			path[20];
	MQX_FILE_PTR    fd = NULL;
	char 			wbuf[2048];
	
	/* 数据帧前写入时间 */
	_time_get(&mqx_time);
    if (_time_to_date(&mqx_time, &mqx_date_time) == FALSE ) {
        printf("\n Cannot convert mqx_time to date format");
    }
	/* 生成写入路径 */
	snprintf(logfilename, sizeof(logfilename), "%04d%02d%02d", mqx_date_time.YEAR, mqx_date_time.MONTH,mqx_date_time.DAY);	
	snprintf(path, sizeof(path), "%s%s", "c:/datalog/", logfilename);

	/* 拼接写入的字符串 */
	snprintf(wbuf, 5+6, "@%02d:%02d:%02d,", mqx_date_time.HOUR,mqx_date_time.MINUTE,mqx_date_time.SECOND);	
	ToString(logdata, wbuf+strlen(wbuf), len);
	strcat(wbuf, "\n");
	//printf("\n%s", wbuff);
	/* 打开文件，方式为"a" */
	fd=fopen(path,"a");
	if (fd == NULL){
		printf("\nopen %s error\n", path);
		return 1;
	}
	if (fseek(fd, 0, IO_SEEK_CUR ) != IO_ERROR) {
		/* 在时间后写入数据帧内容 */
		if (write(fd, wbuf, strlen(wbuf)) != strlen(wbuf)) {
			/* incomplete write */
			printf("Error writing file %s.\n", path );
			if (ferror(fd) == MFS_DISK_FULL)
				printf("Disk full.\n" );
		}
	} else {
		printf("Error, unable to seek file %s.\n", path );
	}
	/* 操作结束，关闭文件 */
	fclose(fd);
	MFS_filesync(fd);
	fd = NULL;
	/* 读出本次操作写入的内容 */
//	fd = fopen(path,"r");
//	fseek(fd, -1L*(len+6), IO_SEEK_END);
//	rlen = fread(logdata,1,len+6,fd);
//	if (len+6 == rlen)
//	{
//		for(i=0;i<len+6;i++)
//		{
//			printf("%02x ", logdata[i]);
//		}
//		printf("\n");
//	}	
//	fclose(fd);
	//获取log行,在二进制情况下不能得到
	return 0;
}

uint32_t cmd_log(uchar *logdata, uint16_t len, uchar send_recv)
{
	TIME_STRUCT 	mqx_time;
	DATE_STRUCT 	mqx_date_time;
	
	uint32_t 		freesize;
	char 			logfilename[16];
	char 			path[20];
	MQX_FILE_PTR    fd = NULL;
	char 			wbuf[2048]={0};
	
	/* 转换时间格式 */
	_time_get(&mqx_time);
    if (_time_to_date(&mqx_time, &mqx_date_time) == FALSE ) {
        printf("\n Cannot convert mqx_time to date format");
    }
	/* 生成写入路径 */
	snprintf(logfilename, sizeof(logfilename), "%04d%02d%02d", mqx_date_time.YEAR, mqx_date_time.MONTH,mqx_date_time.DAY);	
	snprintf(path, sizeof(path), "%s%s", "c:/cmdlog/", logfilename);

	/*拼接写入的字符串*/
	snprintf(wbuf, 5+6, "@%02d:%02d:%02d,", mqx_date_time.HOUR,mqx_date_time.MINUTE,mqx_date_time.SECOND);	//前导+数据内容
	if (send_recv == 1){
		strcat(wbuf, "send,");
	}else if (send_recv == 0){
		strcat(wbuf, "recv,");
	}
	if (!strcmp((char const*)logdata, "gateway no replay")){
		strcat(wbuf,"gateway no replay");
	}else{
		ToString(logdata, wbuf+strlen(wbuf), len);
	}
	strcat(wbuf, "\n");
	//printf("\n%s", wbuff);
	/* 打开文件，方式为"a" */
	fd=fopen(path,"a");
	if (fd == NULL){
		printf("\nopen %s error\n", path);
		return 1;
	}
	if (fseek(fd, 0, IO_SEEK_CUR ) != IO_ERROR) {
		/* 在时间后写入数据帧内容 */
		if (write(fd, wbuf, strlen(wbuf)) != strlen(wbuf)) {
			/* incomplete write */
			printf("Error writing file %s.\n", path );
			if (ferror(fd) == MFS_DISK_FULL)
				printf("Disk full.\n" );
		}
	} else {
		printf("Error, unable to seek file %s.\n", path );
	}
	/* 操作结束，关闭文件 */
	fclose(fd);
	MFS_filesync(fd);
	fd = NULL;

	return 0;
}











//#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
//#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
//#endif
//
//
//#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
//#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
//#endif

//#define MY_LOG     1
//
//typedef struct entry_struct
//{
//   LOG_ENTRY_STRUCT   HEADER;
//   _mqx_uint          C[8];
//   _mqx_uint          I;
//} ENTRY_STRUCT, * ENTRY_STRUCT_PTR;
//
//
///*TASK*----------------------------------------------------------
//*
//* Task Name : main_task
//* Comments  :
//*   This task logs 10 keystroke entries then prints out the log.
//*END*-----------------------------------------------------------*/
//
//void data_log
//   (
//      uint8_t *data, uint32_t len
//   )
//{
//   ENTRY_STRUCT entry;
//   _mqx_uint    result;
//   _mqx_uint    i;
//   unsigned char        c;
//
//   /* test code */
//   char logbuff[800];
//	//char logdata[8] = "1234567";
//   char logdata[10] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};
//	logdata[9] = 0x00;
//   
//   /* Create the log component */
//   result = _log_create_component();
//   if (result != MQX_OK) {
//      printf("Main task: _log_create_component failed");
//      _task_block();  
//   }
//
//   /* Create a log */
//   result = _log_create(MY_LOG,
//      10 * (sizeof(ENTRY_STRUCT)/sizeof(_mqx_uint)), 0);
//   if (result != MQX_OK) {
//      printf("Main task: _log_create failed");   
//      _task_block();  
//   }
////   /* Switch to string */
////	   
////   
////   snprintf(logbuff, sizeof(logbuff), "%s", logdata);
////   printf("%s\n", logbuff);
//   
//   /* Write the data to the log */   
//   printf("Please type in 10 characters:\n");
//   for (i = 0; i < 10; i++) {
//      c = 'a';//getchar();
//      result = _log_write(MY_LOG, 2, data, i);
//      if (result != MQX_OK) {
//         printf("Main task: _log_write failed");   
//      }
//   }
//	_time_delay(2000);
//   /* Read data from the log */
//   printf("\nLog contains:\n");
//   while (_log_read(MY_LOG, LOG_READ_OLDEST_AND_DELETE, 2,
//      (LOG_ENTRY_STRUCT_PTR)&entry) == MQX_OK)
//   {
//      printf("Time: %lu.%03u.%03u, c=%lx, i=%u\n",
//         entry.HEADER.SECONDS,
//         (_mqx_uint)entry.HEADER.MILLISECONDS,
//         (_mqx_uint)entry.HEADER.MICROSECONDS,
//         (_mqx_uint)entry.C & 0xffffffff,
//         entry.I);
//   }
//
//   /* Destroy the log */
//   _log_destroy(MY_LOG);
//
//   //_task_block();
//
//}

