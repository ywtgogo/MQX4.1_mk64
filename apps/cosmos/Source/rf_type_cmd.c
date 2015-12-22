#include "global.h"
#include <fio.h>
#include <shell.h>

#include "rf_scan_task.h"
#include "rf_type_cmd.h"
#include "gw_serial.h"
#include "CRC16_CODE.h"
#include "internal_mem.h"
////extern SUBNET_PTR		SubNetPtr;
//
//uchar routetab[] = 
//{
//	0x00, 0xb0, 00, 00, 00, 00, 00, 00, 00, 0x01, 00, 0x01,
//	00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
//	00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
//	00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
//	00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
//	00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
//	00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,
//	00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00,		
//};
//
//uchar rfdtab0[] =
//{
//	0x00,0x11, 0x30,0xCF, 0x30,0xD8, 0x30,0x8C, 0x30,0x8A, 0x30,0x16, 0x30,0x12, 0x30,0x8E, 0x30,0xF3, 0x30,0xE2, 0x30,0xE5, 0x30,0x96, 0x30,0xE8, 0x30,0x7C 
//};
//uchar rfdtab1[] = 
//{
//	0x00,0x11,0x00,0x12, 0x00,0x13, 0x00,0x14, 0x00,0x21 ,0x00,0x22, 0x00,0x23 ,0x00,0x24,  0x00,0x31, 0x00,0x32, 0x00,0x33 ,0x00,0x34,  0x00,0x41 ,0x00,0x42 ,0x00,0x43 ,0x00,0x44
//};
//
////设置gw的reg参数目前还不太了解，先固化配置频段测试
//uchar testallcooreg[] = 
//{
//	0x00,0x01, 0x24,0x0f, 0x00,0x46, 0x15,0x8b, 0x00,0x41, 0x00,0x71, 0x00,0x04, 0xed,0xb8, 0x00,0x95, 0x00,0x16, 0x00,0x22, 0x1f,0x40, 0x1d,0x4c, 0x27,0x10, 0x23,0x28, 0x29,0x04, 0x25,0x1c, 0x00,0x64, 0x00,0x0a, 0x00,0x0a, 0x00,0x0a, 0x03,0xe8, 0x03,0x03, 0x03,0x03
//};
//uchar test0etab[] =
//{
//	0x80, 0x01, 0x10, 00, 0x01, 00, 0x01, 00, 0x09, 00, 0x01, 0x02, 00, 0x0E, 
//};
//uchar test0btab[] =
//{
//	0x80, 0x01, 0x10, 00, 0x01, 00, 0x01, 00, 0x09, 00, 0x01, 0x02, 00, 0x0b, 
//};

//0x43 sync sys time to rf
uint32_t cmd_sync(uchar *destid, uchar *targnet, uchar *cmd_apn)
{
	uint8_t 		syncdata[sizeof(TIME_SYNC_t) + 2];
	uint8_t 		i;
	uchar 			rxbuff[RF_BUF_SIZE];
	TIME_SYNC_t 	syncdata_cmd;
	TIME_STRUCT  	ts;
    DATE_STRUCT  	tm;
	TIME_STRUCT		mqx_time;
	
	_time_get(&mqx_time);
    ts.SECONDS = mqx_time.SECONDS;
    ts.MILLISECONDS = mqx_time.MILLISECONDS;
	
	if (_time_to_date(&ts, &tm) == FALSE ) {
	   printf("\n Cannot convert mqx_time to date format");
	}
	syncdata_cmd.DestId[0] = *destid; 
	syncdata_cmd.DestId[1] = *(destid+1);
		
	syncdata_cmd.FuncCode = *cmd_apn;
	syncdata_cmd.TargId[0] = *targnet;
	syncdata_cmd.TargId[1] = *(targnet+1);

	syncdata_cmd.Third_Time.Day = tm.DAY;
	syncdata_cmd.Third_Time.Hour = tm.HOUR;
	syncdata_cmd.Third_Time.Minute = tm.MINUTE;
	syncdata_cmd.Third_Time.Second = tm.SECOND;
	syncdata_cmd.Third_Time.MiliSecond_h = tm.MILLISEC>>8;
	syncdata_cmd.Third_Time.MiliSecond_l = tm.MILLISEC;

	_mem_copy(&syncdata_cmd, syncdata, sizeof(TIME_SYNC_t));

	rflk_wirte(syncdata, sizeof(syncdata_cmd));
	isr_readrf(rxbuff, cmd_apn, CmdDelayPtr->SyncMin, CmdDelayPtr->SyncMax);
	return 0;
}
//0x46 
uint32_t cmd_realtime(uchar *destid, uchar *targnet, uchar *cmd_apn) 
{
   	uchar  			txbuff[10], i;
	uchar 			rxbuff[RF_BUF_SIZE];
	
	_mem_copy(destid, txbuff, 2);
	_mem_copy(cmd_apn, &txbuff[2], 1);
	_mem_copy(targnet, &txbuff[2+1], 2);
	
	rflk_wirte(txbuff, sizeof(NODE_REALT_t));
	isr_readrf(rxbuff, cmd_apn, CmdDelayPtr->StartSampleMin, CmdDelayPtr->StartSampleMax);
   	return 0;
}
//04
uint32_t cmd_sample(uchar *destid, uchar *targnet, uchar *cmd_apn) //采样范围跟随某个结构体？，或暂时在此处直接写入，或全局读取
{
	uchar  			txbuff[12] = {0}, i;
	uchar 			rxbuff[RF_BUF_SIZE];
	uint8_t			startaddr[] = {0x00, 0x00};
	uint16_t 		nodenum;

	nodenum = targnet[3]<<8&0xff00;
	_mem_copy(destid, txbuff, 2);
	_mem_copy(cmd_apn, &txbuff[2], 1);
	_mem_copy(targnet, &txbuff[2+1], 2);
	_mem_copy(startaddr, &txbuff[2+1+2], 2);
	_mem_copy(&nodenum, &txbuff[2+1+2+2], 2);
	
	rflk_wirte(txbuff, sizeof(NODE_SAMPLE_t));	
	isr_readrf(rxbuff, cmd_apn, CmdDelayPtr->CollectDataMin, CmdDelayPtr->CollectDataMax);
	return 0;
}
////0x04 采样读取 e.g.80 01 04 00 0a 00 00 00 01 ------bak
//uint32_t cmd_sample(uchar *destid, uchar *targid, uchar *cmd_apn) //采样范围跟随某个结构体？，或暂时在此处直接写入，或全局读取
//{
//   	uchar  			txbuff[12], i;
//	uchar 			rxbuff[RF_BUFFER_SIZE];
//	uint16_t 		crc;
//	uint8_t			startaddr[] = {0x00, 0x00};
//	uint8_t 		nodenum[]	= {0x00, 0x04};
//	
//	_mem_copy(destid, txbuff, 2);
//	_mem_copy(cmd_apn, &txbuff[2], 1);
//	_mem_copy(targid, &txbuff[2+1], 2);
//	_mem_copy(startaddr, &txbuff[2+1+2], 2);
//	_mem_copy(nodenum, &txbuff[2+1+2+2], 2);
//	
//	crc = CRC16Calc(txbuff, sizeof(NODE_SAMPLE_t));
//	txbuff[sizeof(NODE_SAMPLE_t)] = crc>>8;//(uint8_t)((crc>>8)&0x00ff);
//	txbuff[sizeof(NODE_SAMPLE_t)+1] = crc;//(uint8_t)(crc&0x00ff);	
//	
//	rflk_wirte(txbuff, sizeof(NODE_SAMPLE_t)+2);	
//	//task_isr_readrf(sizeof(NODE_SAMPLE_t), 20);
//	isr_readrf(rxbuff, cmd_apn, 3, 20);
//	return 0;
//}

/*
*	设置的参数将和gateway中的参数保持一致性，使用偏移地址对参数进行存取
*	参数输入0000，0000将取得Cosmos默认参数对节点进行配置reg
*	有参数范围需要注意
*/
uint32_t cmd_setreg(uchar *targffdid, uchar *targrfdid, uchar *paramaddr, uchar *paramdest)
{
	uchar		txbuf[128];
	uchar 		cooid[2];
	uint8_t 	i;
	uint8_t		numofregs[] = {0x00,0x01};
	uint8_t		numofbytes= 0x02;
	
	cooid[0] = 0x80|PhubInfoPtr->RfCfg.CooId;
	cooid[1] = PhubInfoPtr->RfCfg.CooId>>8;
	
	_mem_copy(cooid, txbuf, NODE_ID_LEN);
	txbuf[NODE_CMD_AFN_OFFSET]	= P01_AFN_NODE_REG_SET;	
	_mem_copy(targffdid, txbuf+3, NODE_ID_LEN);
	_mem_copy(targrfdid, txbuf+5, NODE_ID_LEN);
	_mem_copy(paramaddr, txbuf+7, NODE_REGADDR_LEN);
	_mem_copy(numofregs, txbuf+9, NODE_REGNUM_LEN);
	txbuf[11] = numofbytes;
	_mem_copy(paramdest, txbuf+12, NODE_REGPARAM_LEN);
	
	rflk_wirte(txbuf, sizeof(NODE_SREG_SET_t));	
	task_isr_readrf(&txbuf[NODE_CMD_AFN_OFFSET], CmdDelayPtr->Other);
	return 0;
}

uint32_t cmd_getreg(uchar *targffdid, uchar *targrfdid, uchar *regoffset)
{
	uchar 		txbuf[128];
	uchar	 	cooid[2];
	uchar 		cmd_apn = 0x03;
	uint16_t	numofregs = 0x0001;

	cooid[0] = 0x80|PhubInfoPtr->RfCfg.CooId;
	cooid[1] = PhubInfoPtr->RfCfg.CooId>>8;
	
	_mem_copy(cooid, txbuf, NODE_ID_LEN);
	txbuf[NODE_CMD_AFN_OFFSET]	= P01_AFN_NODE_REG_GET;
	_mem_copy(targffdid, txbuf+3, NODE_ID_LEN);
	_mem_copy(targrfdid, txbuf+5, NODE_ID_LEN);
	_mem_copy(regoffset, txbuf+7, NODE_ID_LEN);	
	txbuf[9]  = numofregs>>8;
	txbuf[10] = numofregs;	
	
	rflk_wirte(txbuf, sizeof(NODE_REG_GET_CMD_t));	
	task_isr_readrf(&txbuf[NODE_CMD_AFN_OFFSET], CmdDelayPtr->Other);
	
	return 0;
}

/*
*	是否是要将uplink数据和内部组网数据放到该一个函数中解析？
*   传入参数为FFDiD  路由表信息   
*	传入FFD 路由表 
*	指令发送的COO Apn由函数自己添加
*/
//uint32_t cmd_setffdroute(uchar *targid_ptr, uchar *routetab_ptr)
//{
//	uchar txbuff[128];
//	uchar coo_id[] = {0x80, 0x01};
//	
//	_mem_copy(coo_id, txbuff, NODE_ID_LEN);
//
//	txbuff[NODE_CMD_AFN_OFFSET]	= P01_AFN_RF_ROUTE_TABLE;
//	_mem_copy(targid_ptr, &txbuff[NODE_CMD_TargId_OFFSET], NODE_ID_LEN);
//	_mem_copy(routetab_ptr, &txbuff[NODE_CMD_HopTable_OFFSET], sizeof( HOP_TABLE_t)*8);	//routetab len 96 = 12*8
//
//	rflk_wirte(txbuff, sizeof(RF_ROUTE_TABLE_t));	
//	task_isr_readrf(sizeof(RF_ROUTE_TABLE_t), CmdDelayPtr->Other);
//	return 0;
//}

uint32_t auto_setffdroute(void)
{
	uint32_t result, error_count;
	RF_ROUTE_TABLE_PTR	rfroutetable_ptr;
	rfroutetable_ptr = _mem_alloc_zero(sizeof(RF_ROUTE_TABLE_t));
	
	rfroutetable_ptr->DestId = PhubInfoPtr->RfCfg.CooId|0x0080;
	rfroutetable_ptr->FuncCode = 0x51;
	for (int i=0; i<SubNetPtr->CellCount; i++) {
		rfroutetable_ptr->Table[i].DestFfd = SubNetPtr->SubNetCell[i].FfdId ;
		rfroutetable_ptr->Table[i].NextFfd = SubNetPtr->SubNetCell[i+1].FfdId;
		if (i>=SubNetPtr->CellCount) rfroutetable_ptr->Table[i].NextFfd = 0x0000;
		rfroutetable_ptr->Table[i].Dummy = 0x00000000;
		rfroutetable_ptr->Table[i].PreFfd = SubNetPtr->SubNetCell[i-1].FfdId;
		if (i==0) rfroutetable_ptr->Table[i].PreFfd = PhubInfoPtr->RfCfg.CooId;
		rfroutetable_ptr->Table[i].LineNum = SubNetPtr->SubNetCell[i].Order<<8&0xFF00;
	}
	for (int i=0; i<SubNetPtr->CellCount; i++) {
		error_count = 3;
		rfroutetable_ptr->TargFfd = SubNetPtr->SubNetCell[i].FfdId;
		while (error_count--){
			rflk_wirte((uchar*)rfroutetable_ptr, sizeof(RF_ROUTE_TABLE_t));	
			result = task_isr_readrf( &rfroutetable_ptr->FuncCode , CmdDelayPtr->Other);
			if (result == EC_OK) break;
		}
	}
	_mem_free(rfroutetable_ptr);
	return 0;
}
/*
*	发送rfd的注册信息   
*	传入FFD发送对象 rfdtab 
*	指令发送的COO Apn由函数内部定义
*/
#define NODE_CMD_Reg_StartAddrH_OFFSET	5
#define NODE_CMD_Reg_StartAddrL_OFFSET	6
#define NODE_CMD_Reg_NumOfNodesH_OFFSET	7
#define NODE_CMD_Reg_NumOfNodesL_OFFSET	8
#define NODE_CMD_Reg_NumOfBytes_OFFSET	9
uint32_t auto_setrfdtab(void)//uchar *targid, uchar *rfdtab
{
	uint32_t result, error_count;
	RFD_TABLE_PTR	rfdtable_ptr;	
	rfdtable_ptr = _mem_alloc_zero(sizeof(RFD_TABLE_t));
	
	rfdtable_ptr->DestId = PhubInfoPtr->RfCfg.CooId|0x0080;
	rfdtable_ptr->FuncCode = 0x66;
	for (int i=0; i<SubNetPtr->CellCount; i++){	
		error_count = 3;
		rfdtable_ptr->TargFfd		= SubNetPtr->SubNetCell[i].FfdId;
		rfdtable_ptr->StartAddr		= 0x0000;
		rfdtable_ptr->NumOfNodes	= SubNetPtr->SubNetCell[i].RfdCount<<8&0xff00;
		rfdtable_ptr->NumOfBytes	= SubNetPtr->SubNetCell[i].RfdCount*2;
//		结构体成员不对齐的情况下copy数据报警告如何处理？？？？？？
//		//rfdtable_ptr->RfdId		= SubNetPtr->SubNetCell[i].Rfd;
//		//mem_buf = (uint16_t *)&SubNetPtr->SubNetCell[i].Rfd[0];
//		_mem_copy((uchar *)SubNetPtr+4*i, mem_buf, sizeof(SubNetPtr->SubNetCell[i].Rfd));
		for (int j=0; j<SubNetPtr->SubNetCell[i].RfdCount; j++){
			rfdtable_ptr->RfdId[j]	= SubNetPtr->SubNetCell[i].Rfd[j];
		}
		while (error_count--){
			rflk_wirte((uchar*)rfdtable_ptr, sizeof(RFD_TABLE_t)-2*(50-SubNetPtr->SubNetCell[i].RfdCount));	
			result = task_isr_readrf(&rfdtable_ptr->FuncCode, CmdDelayPtr->Other);
			if (result == EC_OK) break;
		}
	}
	_mem_free(rfdtable_ptr);
	return 0;
}
/*
*	发送rfd绑定到FFD的命令   
*	传入FFD发送对象 rfdtab 
*	指令发送的COO Apn由函数内部定义
*/
uint32_t auto_setrfdbind(void)
{
	RFD_BIND_PTR	rfdbind_ptr;	
	rfdbind_ptr = _mem_alloc_zero(sizeof(RFD_BIND_t));
	rfdbind_ptr->DestId = PhubInfoPtr->RfCfg.CooId|0x0080;
	rfdbind_ptr->FuncCode = 0x45;
	for (int i=0; i<SubNetPtr->CellCount; i++)
	{
		rfdbind_ptr->TargFfd = SubNetPtr->SubNetCell[i].FfdId;
		rflk_wirte((uchar *)rfdbind_ptr, sizeof(RFD_BIND_t));
		task_isr_readrf(&rfdbind_ptr->FuncCode, 120);
	}
	_mem_free(rfdbind_ptr);
	return 0;
}
/*
*	根据被更新的子网信息重新对无线子网组网
*   通过信号接收，处理新的网络结构
*/
uint32_t auto_subnet_cfg(void)
{
	auto_setffdroute();
	auto_setrfdtab();
	auto_setrfdbind();
	return 0;
}

/*
*	使能PV的输出命令   
*	传入FFD发送对象 PV板的对象RFD 
*	指令发送的COO Apn由函数内部定义
*/
uint32_t cmd_setpvpoweron(uchar *targid, uchar *rfdid)
{
	uchar 		txbuf[9];
	uint16_t 	cooid[2];
	uchar		cmd_apn = 0x41;
	
	cooid[0] = 0x80|PhubInfoPtr->RfCfg.CooId;
	cooid[1] = PhubInfoPtr->RfCfg.CooId>>8;
	
	_mem_copy(cooid, txbuf, NODE_ID_LEN);
	txbuf[NODE_CMD_AFN_OFFSET] = cmd_apn;
	_mem_copy(targid, &txbuf[NODE_CMD_TargId_OFFSET], NODE_ID_LEN);
	
	rs485lk_wirte(txbuf, 7);
	task_isr_readrf(&txbuf[NODE_CMD_AFN_OFFSET], 126);
	return 0;
}
/*
*	关断PV的输出命令   
*	传入FFD发送对象 PV板的对象RFD 
*	指令发送的COO Apn由函数内部定义
*/
uint32_t cmd_setpvpoweroff(uchar *targid, uchar *rfdid)
{
	uchar 		txbuf[9];
	uint16_t 	cooid[2];
	uchar		cmd_apn = 0x44;
	
	cooid[0] = 0x80|PhubInfoPtr->RfCfg.CooId;
	cooid[1] = PhubInfoPtr->RfCfg.CooId>>8;;
	
	_mem_copy(cooid, txbuf, NODE_ID_LEN);
	txbuf[NODE_CMD_AFN_OFFSET] = cmd_apn;
	_mem_copy(targid, &txbuf[NODE_CMD_TargId_OFFSET], NODE_ID_LEN);
	
	rs485lk_wirte(txbuf, 7);
	task_isr_readrf(&txbuf[NODE_CMD_AFN_OFFSET], 126);
	return 0;
}
