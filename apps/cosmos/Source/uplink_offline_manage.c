#include "global.h"
#include "rf_type_cmd.h"
#include "internal_mem.h"
#include <mfs.h>

/**
*	离线数据管理 TASK
*/
#pragma   pack(1)
//typedef struct
//{
//   uint16_t 	YEAR;	/* 1970 - 2099 */
//   uint16_t 	MONTH;	/* 1 - 12 */
//   uint16_t 	DAY;	/* 1 - 31 (depending on month) */
//   uint16_t 	HOUR;	/* 0 - 23 */
//   uint16_t 	MINUTE;	/* 0 - 59 */
//   uint16_t 	SECOND;	/* 0 - 59 */
//}SAVE_TIME;

typedef struct 
{
	uint8_t  	type;
	uint32_t    Uid;
	uint8_t	  	dummy;
	uint32_t 	save_time;
	uint16_t   	len;
	uint8_t    	data[51*22];
} SAVE_DATA_BUFFER;
#pragma   pack()

extern uint32_t uplk_send(char *buffer, uint16_t buflen, ...);
extern int32_t MFS_filesync(MQX_FILE_PTR fd_ptr);

void uplk_offline_task(uint32_t	parameter)
{
	TIME_STRUCT 	mqx_time;
	TIME_STRUCT 	save_time;
	DATE_STRUCT 	mqx_date_time;
	DATE_STRUCT 	save_date_time;
	
	uint32_t		len;
	uint32_t 		freesize;
	char 			logfilename[16];
	char 			path[32];
	MQX_FILE_PTR    fd = NULL;
	char 			rbuff[2048]={0};
	uint32_t		daynum=0;
	/* test offline data */
	uchar 					swbuf[900];
	uint16_t				vtemp;
	RF_SAMPLE_TYPE_PTR		cell_rfdata_ptr;
	cell_rfdata_ptr	= _mem_alloc_zero(sizeof(RF_SAMPLE_TYPE_t));
	
	SAVE_DATA_BUFFER		offline_package;
	offline_package.type 	= 0x14;
	offline_package.Uid		= htonl(CosmosInfoPtr->CosmosUid);
	offline_package.dummy	= 0x00;
	//offline_package.save_time
	offline_package.len		= htons(0x0462);
	_mem_zero(offline_package.data, 51*22);
	/* 方法二: 不退出任务，在发送完成后block */
	
	_time_get(&mqx_time);
	/* 检查所有离线日志文件是否发送完毕 */
	while(1){
		mqx_time.SECONDS = mqx_time.SECONDS - (86400U)*daynum++;
		/* 转换当日时间格式 */
		if (_time_to_date(&mqx_time, &mqx_date_time) == FALSE ) {
			printf("\n Cannot convert mqx_time to date format");
		}
		/* 生成文件路径 */
		snprintf(logfilename, sizeof(logfilename), "%04d%02d%02d", mqx_date_time.YEAR, mqx_date_time.MONTH,mqx_date_time.DAY);	
		snprintf(path, sizeof(path), "%s%s", "c:/datalog/", logfilename);
		printf("\nCheck date:%.8s offline file", logfilename);
		/* 打开文件，方式为"r" */
		fd=fopen(path,"r");
		if (fd == NULL){
			printf("\nHaven't date:%.8s offline data", logfilename);
			printf("\nOffline data of All date uploaded completed");
			break;
		}
		/* 只有接收到触发信号才启动下面的发送 */

		/* 读取第一行数据 */
		while(fgets(rbuff,2048,fd) != NULL)
		{
			len = strlen(rbuff);
			rbuff[len-1] = '\0';  /*去掉换行符*/
			printf("\n%s %d ",rbuff,len - 1);

			/* 解析并填充时间数据 */
			save_date_time.YEAR		= mqx_date_time.YEAR;
			save_date_time.MONTH	= mqx_date_time.MONTH;
			save_date_time.DAY		= mqx_date_time.DAY;
			save_date_time.HOUR		= (rbuff[1]-'0')<<4 |(rbuff[2]-'0');
			save_date_time.MINUTE	= (rbuff[4]-'0')<<4 |(rbuff[5]-'0');
			save_date_time.SECOND	= (rbuff[7]-'0')<<4 |(rbuff[8]-'0');
			_time_from_date(&save_date_time, &save_time);
			
			offline_package.save_time = htonl(save_time.SECONDS);
			/* 将数据转换为16进制 */
			StrToHex(rbuff+10, swbuf);
			
			for (int i=0; i<swbuf[8]; i++)
			{
				offline_package.data[22*i] 	= 0x10;
				offline_package.data[1 + 22*i]= 0x16;
#if 1			/* 旧版服务器Vin Vout刚好相反 */
				/* 拼接离线数据为旧版本兼容格式 */
				_mem_copy(swbuf + 9 + 16*i, cell_rfdata_ptr, sizeof(RF_SAMPLE_TYPE_t));
				vtemp = cell_rfdata_ptr->Vout;
				cell_rfdata_ptr->Vout = cell_rfdata_ptr->Vin;
				cell_rfdata_ptr->Vin =vtemp;
				_mem_copy((uchar *)cell_rfdata_ptr, offline_package.data + 22*i + 2, 16);
#else
				_mem_copy(buffer + 9 + 16*i, offline_package.data + 22*i + 2, 16);
#endif 
			}
			/* 发送本条离线数据 */	
			uplk_send((char *)&offline_package, sizeof(SAVE_DATA_BUFFER));
			/* 删除本行---暂时不行进行操作，若在发送过程中断网络则重发此文件 */
			
			/* 延时100ms */
			_time_delay(100);
		}
		/* 查找是否到达文件末尾 */
		if(feof(fd)){
			printf("\nWe have reached endof file");
		}
		fclose(fd);
		/* 文件发送完毕，则删除该文件 */
		ioctl(fd, IO_IOCTL_DELETE_FILE, (void *) path);
		//MFS_filesync(fd);
		_time_delay(1000*3);
	}
	_mem_free(cell_rfdata_ptr);
	/* 对系统发送结束离线上传的信号 */
}










///* 本题的一个完整的c程序如下，程序在win-tc和Dev-c++下已调试通过。 */ 
//#include<stdio.h> 
//#include<stdlib.h> 
//#include<string.h> 
//#define N 255 /* 设文章不超过255行，可更改 */ 
//
//int main() 
//{ 
//int i=0,max; 
//char s[N][81],*p="A.txt";/*设文件A.txt每行文章不超过80字符，可更改*/ 
//FILE *fp; 
//if ((fp=fopen("A.txt","r"))==NULL) 
//{ printf("Open file %s error! Strike any key to exit.",p); 
//system("pause"); 
//exit(0); 
//} 
//while (i<N&&fgets(s[i],81,fp)!=NULL)/*按行读原文章*/ 
//{ 
//p=strchr(s[i],'\n');/*消除每行最后的回车符*/ 
//if(p) 
//*p='\0'; 
//i++; 
//} 
//max=i;/*max为文章实际的最大行数*/ 
//fclose(fp); 
//fp=fopen("A.txt","w"); 
//for(i=0;i<max-2;i++) /*写入文件并输出到屏幕显示除最后一行后的所有行*/ 
//{ fprintf(fp,"%s\n",s[i]); 
//printf("%s\n",s[i]); 
//} 
//fprintf(fp,"%s",s[i]); 
//printf("%s\n",s[i]); 
//fclose(fp); 
//system("pause"); 
//return 0; 
//} 
//我发的类似的问题很多的，你可以参阅: 
//http://zhidao.baidu.com/question/79123769.html 
//http://zhidao.baidu.com/question/80045856.html
//http://zhidao.baidu.com/question/80034032.html 

