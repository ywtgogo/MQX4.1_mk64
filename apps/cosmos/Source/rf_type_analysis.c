#include "global.h"
#include "internal_mem.h"
#include "rf_type_cmd.h"
#include "CRC16_CODE.h"
#include "uplink_data_type.h"
#include "kwtj_GWII.h"

static const char *GATEWAY_ERROR[] = { "gateway no replay", "gateway crc error", "MB", "GB", "TB" };

extern int32_t MFS_filesync(MQX_FILE_PTR fd_ptr);
extern int32_t MFS_get_freesize(MQX_FILE_PTR fd_ptr);
extern void data_log(uint8_t *data, uint32_t len);
extern void cmd_log(uint8_t *data, uint32_t len, uchar);
extern uint32_t uplk_send(char *buffer, uint16_t buflen, ...);

uint32_t rf_rsp_uplk_send(char *buffer, uint16_t buflen)
{
	char *sendbuf = NULL;
	sendbuf 	= _mem_alloc_zero(CMD_RSP_ORG_LEN);//CMD_RSP_ORG_LEN
	sendbuf[CMD_RSP_Uid_OFFSET]		= CosmosInfoPtr->CosmosUid>>24;
	sendbuf[CMD_RSP_Uid_OFFSET+1]	= CosmosInfoPtr->CosmosUid>>16;
	sendbuf[CMD_RSP_Uid_OFFSET+2]	= CosmosInfoPtr->CosmosUid>>8;
	sendbuf[CMD_RSP_Uid_OFFSET+3]	= CosmosInfoPtr->CosmosUid;
	sendbuf[CMD_RSP_Len_OFFSET]		= 0x01;//buflen;//目前旧版本服务器只支持0、1、2
	sendbuf[CMD_RSP_Apn_OFFSET]		= buffer[2];
	if (buffer[2]&&0x80) sendbuf[CMD_RSP_Apn_OFFSET] = buffer[2]&~0x80;
	sendbuf[CMD_RSP_Stat_OFFSET]	= buffer[5];
	_mem_copy(buffer, sendbuf + CMD_RSP_ORG_OFFSET, buflen) ;
	uplk_send(sendbuf, CMD_RSP_ORG_LEN);
	_mem_free(sendbuf);
	sendbuf = NULL;
	return 0;
}

uint32_t rf_newdata_uplk_send(char *buffer, uint16_t buflen)
{
	char 					*sendbuf = NULL;
	uint16_t				vtemp;
	RF_SAMPLE_TYPE_PTR		cell_rfdata_ptr;
	cell_rfdata_ptr	= _mem_alloc_zero(sizeof(RF_SAMPLE_TYPE_t));
	sendbuf 		= _mem_alloc_zero(0x0462+NEW_DATA_STAT_LEN);
	sendbuf[NEW_DATA_Head_OFFSET]	= 0x12;
	sendbuf[NEW_DATA_Uiu_OFFSET]	= CosmosInfoPtr->CosmosUid>>24;
	sendbuf[NEW_DATA_Uiu_OFFSET+1]	= CosmosInfoPtr->CosmosUid>>16;
	sendbuf[NEW_DATA_Uiu_OFFSET+2]	= CosmosInfoPtr->CosmosUid>>8;
	sendbuf[NEW_DATA_Uiu_OFFSET+3]	= CosmosInfoPtr->CosmosUid;
	sendbuf[NEW_DATA_Obj_OFFSET]	= 0x03;//
	sendbuf[NEW_DATA_Size_OFFSET]	= 0x04;//buflen>>8;
	sendbuf[NEW_DATA_Size_OFFSET+1]	= 0x62;//buflen;
	
	for (int i=0; i<buffer[8]; i++)
	{
		sendbuf[NEW_DATA_Data_OFFSET + 22*i] 	= 0x10;
		sendbuf[NEW_DATA_Data_OFFSET + 1 + 22*i]= 0x16;
#if 1	/* 旧版服务器Vin Vout刚好相反 */
		//cell_rfdata_ptr = (RF_SAMPLE_TYPE_PTR)(buffer + 9 + 16*i);
		_mem_copy(buffer + 9 + 16*i, cell_rfdata_ptr, sizeof(RF_SAMPLE_TYPE_t));
		vtemp = cell_rfdata_ptr->Vout;
		cell_rfdata_ptr->Vout = cell_rfdata_ptr->Vin;
		cell_rfdata_ptr->Vin =vtemp;
		_mem_copy((uchar *)cell_rfdata_ptr, sendbuf + NEW_DATA_Data_OFFSET + 22*i + 2, 16);
#else
		_mem_copy(buffer + 9 + 16*i, sendbuf + NEW_DATA_Data_OFFSET + 22*i + 2, 16);
#endif 
	}

	uplk_send(sendbuf, 0x0462+NEW_DATA_STAT_LEN);
	_mem_free(cell_rfdata_ptr);
	_mem_free(sendbuf);
	sendbuf = NULL;
	return 0;
}

static void exc_code_print(uchar *rxbuff)
{
	switch (rxbuff[5])
	{
	case 2:
		printf("\n Status: %s", "EXC_ADDR_RANGE");
		break;
	case 3:
		printf("\n Status: %s", "EXC_REGS_QUANT");
		break;
	case 4:
		printf("\n Status: %s", "EXC_FUNC_CODE");
		break;
	case 9:
		printf("\n Status: %s", "EXC_RFD_TIMEOUT");
		break;
	case 10:
		printf("\n Status: %s", "EXC_CRC");
		break;
	case 11:
		printf("\n Status: %s", "EXC_TIMEOUT");
		break;
	case 12:
		printf("\n Status: %s", "EXC_IDLE");
		break;
	case 13:
		printf("\n Status: %s", "EXC_HopNodeId");
		break;
	case 14:
		printf("\n Status: %s", "EXC_DestId");
		break;
	case 15:
		printf("\n Status: %s", "EXC_Mode");
		break;	
	case 16:
		printf("\n Status: %s", "EXC_RFD_TIMEOUT");
		break;
	case 17:
		printf("\n Status: %s", "EXC_RADIO_FRM_CRC");
		break;		
	case 18:
		printf("\n Status: %s", "EXC_OPT_CTRL_FAIL");
		break;
	case 19:
		printf("\n Status: %s", "EXC_GTW_RSP_CRC");
		break;
	default:
		printf("\n Status: %s", "EXC_OK");	
		break;
	}
//	printf("\n Status: %.2x", rxbuff[5]);
}

uint32_t rf_mix_analysis(uchar *rxbuff, uint16_t len)
{
	uint16_t	crcread;	
	uint16_t	crc;
	uint16_t	i;
	uint32_t	result=0;
	bool		save_data = FALSE;
	
	uint32_t freesize;
	MQX_FILE_PTR      fd = NULL;
	
	crc = CRC16Calc(rxbuff, len-2);
//  if (crc == )
//	crcread = *(rxbuff+(len-1))<<8 | *(rxbuff+(len-2));
//	if (crc != htons(crcread)){
//		return 1;
//	}else{
		if (rxbuff[2]&&0x80) 
			result = rxbuff[5];
		print_sys_time();
		printf("\n RFRECV: ");
		for (i=0 ;  i<len; i++ )
		{
		   printf("%.2x ", rxbuff[i]);//*(&syncdata_cmd + i)//如何直接从地址上获得详细数据
		}
		exc_code_print(rxbuff);
		printf("\n");
//	}		
	/* 校验后的实时数据上传服务器 */
		if (len >= 0xFF){
			rf_newdata_uplk_send((char *)rxbuff, len);
		}else{
			rf_rsp_uplk_send((char *)rxbuff, len);
		}
		
	if (rxbuff[2]==0x04) {
		rf_led_on();
	}else if (rxbuff[2]==0x84) {
		rf_led_off();
	}
	
	cmd_log(rxbuff, len, 0);
	
	/* 判断服务器离线存入log */
	if (rxbuff[2]==0x04||rxbuff[2]==0x84) {
		if (CosmosInfoPtr->FuncEn&FUNC_EN_ETH) {
			for(int i=0; i<CosmosTcpPtr->CellCount; i++) {
				if (socket_info[i].Status!=SOCKET_OK) {
					break;
				}else {
					return result;	//只要有一路连接正常将跳出，不记录
				}
			}
		}else {
			if (WifiTcp1Stat == TRUE) {
				return result;
			}
		}
		data_log(rxbuff, len);
	}
	return result;
}

uint32_t rf_outoftime(uchar *cmd_apn, uint8_t max_delays)
{
	char no_replay[6] = {0};
	print_sys_time();
	printf("\n%s", GATEWAY_ERROR[0]);
	no_replay[2] = *cmd_apn;
	no_replay[5] = 0x01;
	rf_rsp_uplk_send(no_replay, 0);
	cmd_log((uchar *)GATEWAY_ERROR[0], strlen(GATEWAY_ERROR[0]), 0);
	return 0;
}




