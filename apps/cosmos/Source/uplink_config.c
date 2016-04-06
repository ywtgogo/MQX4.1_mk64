//#include <mqx.h>
#include <string.h>
#include "global.h"
#include "internal_mem.h"
#include "rf_type_cmd.h"

//配置的函数和动作，额外的增加一下一条自动组网
extern uint32_t auto_subnet_cfg(void);
extern int32_t remote_update(int32_t argc, char *argv[]);
extern uint32_t uplk_send(char *buffer, uint16_t buflen, ...);

uint32_t cfg_rsp_uplk_send(char *buffer, uint16_t buflen)
{
	//uplk_send(buffer, buflen);
	return 0;
}


uint32_t download_subnet(char *r_frame_ptr)
{
	int 		in=0;
	char 		*p[16];
	char 		*buf=r_frame_ptr;
	char 		*outer_ptr=NULL;
	char 		*inner_ptr=NULL;
	SUBNET_PTR	subnet_ptr;
	
	subnet_ptr = _mem_alloc_zero(sizeof(SUBNET_t));

	while((p[in] = strtok_r(buf, "@", &outer_ptr))!=NULL)
	{
		buf=p[in];
		while((p[in]=strtok_r(buf, ",", &inner_ptr))!=NULL)
		{
			in++;
			buf=NULL;
		}
		buf=NULL;
	}
	printf("\nHere we have %d strings\n",in);
	subnet_ptr->CellCount = in;
	for (int j=0; j<in; j++)
	{
		StrToHex(p[j], (uchar *)&subnet_ptr->SubNetCell[j]);
		//printf("\n%s\n",p[j]);
		printf("\n");
		for (int i=0; i<strlen(p[j])/2; i++)
		{
			printf("%02x ", *((uchar *)(&subnet_ptr->SubNetCell[j])+i));
		}
	}
	sub_net_write(subnet_ptr, sizeof(SUBNET_t));
	CosmosInfoPtr->InitHead = CosmosInfoPtr->InitHead|INIT_HEAD_SUBNET;
	cosmos_info_write(CosmosInfoPtr, sizeof(uint32_t));
	_mem_free(subnet_ptr);
	return 0;
}

uint32_t download_cmddelay(char *r_frame_ptr)
{
	CMD_DELAY_PTR	cmddelay_ptr;
	cmddelay_ptr = _mem_alloc_zero(sizeof(CMD_DELAY_t));

	r_frame_ptr++;
	StrToHex(r_frame_ptr, (uchar *)cmddelay_ptr);
	printf("\n");
	for (int i=0; i<strlen(r_frame_ptr)/2; i++)
	{
		printf("%02x ", *((uchar *)cmddelay_ptr+i));
	}
	cmd_delay_write(cmddelay_ptr, sizeof(CMD_DELAY_t));
	
	CosmosInfoPtr->InitHead = CosmosInfoPtr->InitHead|INIT_HEAD_COSMOS_CMDDELAY;
	cosmos_info_write(CosmosInfoPtr, sizeof(uint32_t));
	_mem_free(cmddelay_ptr);

	return 0;
}

uint32_t download_phubinfo(char *r_frame_ptr)
{
	int 		in=0;
	char 		*p[16];
	char 		*buf=r_frame_ptr;
	char 		*outer_ptr=NULL;
	char 		*inner_ptr=NULL;
	PHUB_INFO_PTR	phubinfo_ptr;
	phubinfo_ptr = _mem_alloc_zero(sizeof(PHUB_INFO_t));
	//buf++;
	while((p[in] = strtok_r(buf, "@", &outer_ptr))!=NULL)
	{
		buf=p[in];
		while((p[in]=strtok_r(buf, ",", &inner_ptr))!=NULL)
		{
			in++;
			buf=NULL;
		}
		buf=NULL;
	}
//对结构体成员要怎样循环赋值？？？？？？	
//	for (int j=0; j<in; j++)
//	{
		StrToHex(p[0], (uchar *)&phubinfo_ptr->EthCfg);
		StrToHex(p[1], (uchar *)&phubinfo_ptr->RfCfg);
		StrToHex(p[2], (uchar *)&phubinfo_ptr->Rs485Cfg);
//		printf("\n%s\n",p[j]);
//		for (int i=0; i<strlen(p[j])/2; i++)
//		{
//			printf("%02x ", *((uchar *)(&subnet_ptr->SubNetCell[j])+i));
//		}
//	}
		printf("\n");
		for (int i=0; i<strlen(p[0])/2; i++)
		{
			printf("%02x ", *((uchar *)(&phubinfo_ptr->EthCfg)+i));
		}
		printf("\n");
		for (int i=0; i<strlen(p[1])/2; i++)
		{
			printf("%02x ", *((uchar *)(&phubinfo_ptr->RfCfg)+i));
		}
		printf("\n");
		for (int i=0; i<strlen(p[2])/2; i++)
		{
			printf("%02x ", *((uchar *)(&phubinfo_ptr->Rs485Cfg)+i));
		}		
		
	phub_info_write(phubinfo_ptr, sizeof(PHUB_INFO_t));
	CosmosInfoPtr->InitHead = CosmosInfoPtr->InitHead|INIT_HEAD_PHUB_INFO;
	cosmos_info_write(CosmosInfoPtr, sizeof(uint32_t));
	_mem_free(phubinfo_ptr);
	return 0;
}

uint32_t download_cosmosinfo(char *r_frame_ptr)
{
	int 			in=0;
	char 			*p[5];
	char 			*buf=r_frame_ptr;
	char 			*outer_ptr=NULL;
	char 			*inner_ptr=NULL;
	
	uint32_t		temp;
	COSMOS_INFO_PTR	cosmosinfo_ptr;
	
	cosmosinfo_ptr = _mem_alloc_zero(sizeof(COSMOS_INFO_t));

	while((p[in] = strtok_r(buf, "@", &outer_ptr))!=NULL)
	{
		buf=p[in];
		while((p[in]=strtok_r(buf, ",", &inner_ptr))!=NULL)
		{
			in++;
			buf=NULL;
		}
		buf=NULL;
	}
	printf("\nHere we have %d strings",in);
	printf("\nFunctionEn %s", p[0]);
	printf("\nProtocolEn %s", p[1]);
	printf("\nCosmos Uid %s", p[2]);
	
	StrToHex(p[0], (uchar*)&temp);
	cosmosinfo_ptr->FuncEn = htonl(temp);
	StrToHex(p[1], (uchar*)&temp);
	cosmosinfo_ptr->ProtocolEn = htonl(temp);
	StrToHex(p[2], (uchar*)&temp);
	cosmosinfo_ptr->CosmosUid = htonl(temp);
	
	cosmosinfo_ptr->InitHead = CosmosInfoPtr->InitHead|INIT_HEAD_COSMOS_INFO;
	cosmos_info_write(cosmosinfo_ptr, sizeof(COSMOS_INFO_t));
	_mem_free(cosmosinfo_ptr);
	return 0;
}

uint32_t download_cosmostcp(char *r_frame_ptr)
{
	r_frame_ptr++;	
	int 			in=0;
	char 			*p[16];
	char 			*buf=r_frame_ptr;
	char 			*outer_ptr=NULL;
	char 			*inner_ptr=NULL;
	uint16_t		temp;
	COSMOS_TCP_PTR	cosmostcp_ptr;
	
	cosmostcp_ptr = _mem_alloc_zero(sizeof(COSMOS_TCP_t));	
	
	while((p[in] = strtok_r(buf, ",", &outer_ptr))!=NULL)
	{
		buf=p[in];
		while((p[in]=strtok_r(buf, " ", &inner_ptr))!=NULL)
		{
			in++;
			buf=NULL;
		}
		buf=NULL;
	}
	printf("\nHere we have %d strings",in/2);	
	cosmostcp_ptr->CellCount = in/2;
	for (int j=0; j<in/2; j++)
	{
		printf("\nTCP %s %s",p[j*2], p[j*2+1]);
		_mem_copy(p[j*2], cosmostcp_ptr->TcpCell[j].TcpDns, sizeof(TCP_CELL_t));
		cosmostcp_ptr->TcpCell[j].TcpPort = _io_atoi(p[j*2+1]);
	}
	
	cosmos_tcp_write(cosmostcp_ptr, sizeof(COSMOS_TCP_t));
	CosmosInfoPtr->InitHead = CosmosInfoPtr->InitHead|INIT_HEAD_COSMOS_TCP;
	cosmos_info_write(CosmosInfoPtr, sizeof(uint32_t));	
	_mem_free(cosmostcp_ptr);	
	
	return 0;
}

uint32_t update_k64(char *r_frame_ptr)
{
	int 			in=0;
	char 			*p[5];
	char 			*buf=r_frame_ptr;
	char 			*outer_ptr=NULL;
	char 			*inner_ptr=NULL;
	
	uint32_t		temp;
	COSMOS_INFO_PTR	cosmosinfo_ptr;
	
	cosmosinfo_ptr = _mem_alloc_zero(sizeof(COSMOS_INFO_t));

	while((p[in] = strtok_r(buf, "@", &outer_ptr))!=NULL)
	{
		buf=p[in];
		while((p[in]=strtok_r(buf, ",", &inner_ptr))!=NULL)
		{
			in++;
			buf=NULL;
		}
		buf=NULL;
	}
	printf("\nHere we have %d strings",in);
	
	for (int j=0; j<in; j++)
	{
		printf("\n%s",p[j]);
	}
	remote_update(in, p);
	return 0;
}
#if 1 /* 支持旧版服务器协议的配置 */
#pragma   pack(1)
typedef struct RF_ID_MODIFY_UPLK//10
{
	uint32_t	UID;
	uint8_t		ObjDummy;
	uint8_t		FuncCode;
	uint8_t		RoleDummy;
	uint16_t	DestId;
	uint16_t	TargFfdId;
	uint16_t	TargRfdId;
	uint16_t	StartAddr;
	uint16_t	NumOfRegs;
	uint8_t		NumOfBytes;
	uint16_t	TargModifyId;
} RF_ID_MODIFY_UPLK_t, * RF_ID_MODIFY_UPLK_PTR;
#pragma   pack()
/* 10 */
void cfg_10_save(char *r_frame_ptr)
{
	RF_ID_MODIFY_UPLK_PTR	rf_id_modify_ptr = NULL;
	PHUB_INFO_t				phubinfo;
	
	_mem_copy(PhubInfoPtr, &phubinfo, sizeof(PHUB_INFO_t));
	rf_id_modify_ptr = (RF_ID_MODIFY_UPLK_PTR)r_frame_ptr;

	if (rf_id_modify_ptr->DestId == rf_id_modify_ptr->TargFfdId 
		&& rf_id_modify_ptr->TargFfdId == rf_id_modify_ptr->TargRfdId
			&& rf_id_modify_ptr->TargRfdId == rf_id_modify_ptr->DestId)
	{
		if (rf_id_modify_ptr->DestId != rf_id_modify_ptr->TargModifyId) {
			phubinfo.RfCfg.CooId = rf_id_modify_ptr->TargModifyId;
			phub_info_write(&phubinfo, sizeof(PHUB_INFO_t));
			printf("go on...");
		}else {
			;
		}
	}
	return;
}

/* 51 */
uint32_t cfg_51_save(char *r_frame_ptr)
{
	RF_ROUTE_TABLE_PTR	rf_route_table_ptr	= _mem_alloc_zero(sizeof(RF_ROUTE_TABLE_t));
	SUBNET_PTR			subnet_ptr 			= _mem_alloc_zero(sizeof(SUBNET_t));

	_mem_copy(r_frame_ptr, rf_route_table_ptr, sizeof(RF_ROUTE_TABLE_t));
	_mem_copy(SubNetPtr, subnet_ptr, sizeof(SUBNET_t));
	
	for (int i=0; i<8; i++){
		if (rf_route_table_ptr->Table[i].LineNum == 0) break;
		subnet_ptr->SubNetCell[i].FfdId = rf_route_table_ptr->Table[i].DestFfd;
		subnet_ptr->SubNetCell[i].Order = i+1;
		subnet_ptr->CellCount = i+1;
	}

	sub_net_write(subnet_ptr, sizeof(SUBNET_t));
	CosmosInfoPtr->InitHead = CosmosInfoPtr->InitHead|INIT_HEAD_SUBNET;
	cosmos_info_write(CosmosInfoPtr, sizeof(uint32_t));

	_mem_free(rf_route_table_ptr);
	_mem_free(subnet_ptr);
	rf_route_table_ptr = NULL;
	subnet_ptr = NULL;
	return 0;
}

/* 66 */
uint32_t cfg_66_save(char *r_frame_ptr)
{
	RFD_TABLE_t		rfd_table;	
	SUBNET_t		subnet;
	
	_mem_copy(r_frame_ptr, &rfd_table, sizeof(RFD_TABLE_t));
	_mem_copy(SubNetPtr, &subnet, sizeof(SUBNET_t));

	for (int i=0; i<8; i++){
		if (rfd_table.TargFfd == subnet.SubNetCell[i].FfdId) {
			subnet.SubNetCell->RfdCount = rfd_table.NumOfBytes/2;
			for (int j=0; j<50; j++) {
				subnet.SubNetCell->Rfd[j] = rfd_table.RfdId[j];
			}
			sub_net_write((SUBNET_PTR)&subnet, sizeof(SUBNET_t));
			CosmosInfoPtr->InitHead = CosmosInfoPtr->InitHead|INIT_HEAD_SUBNET;
			cosmos_info_write(CosmosInfoPtr, sizeof(uint32_t));	
			return 0;
		}		
	}
	printf("\ntarg ffd nonexistent 0x%.4x", htons(rfd_table.TargFfd));
	return 1;	
}
#endif /* 结束旧配置协议解析 */

uint32_t download_cfgmix(char *r_frame_ptr)
{
	char 			*cfgdata_ptr=NULL;
	char			*cfgrsp_ptr=NULL;
	uint32_t		offset;
	uint32_t		copy_len;
	uint32_t		return_code;
	
	cfgrsp_ptr = r_frame_ptr;
	r_frame_ptr++;
	cfgdata_ptr = strchr(r_frame_ptr,'@');
	if(cfgdata_ptr == NULL) {
        printf("\nError cfg package!"); 
		return 1;
	}
	offset = cfgdata_ptr - r_frame_ptr;
	/* 查找cfgdata中是否还有‘@’，有则报错数据包错误 */

	if (!strncasecmp(r_frame_ptr, "cmddelay", offset)){
		download_cmddelay(cfgdata_ptr);
	}else if (!strncasecmp(r_frame_ptr, "subnet", offset)){
		download_subnet(cfgdata_ptr);
		/* 传输信号通知自动组网函数 */
		auto_subnet_cfg(); 
	}else if (!strncasecmp(r_frame_ptr, "phubinfo", offset)){
		download_phubinfo(cfgdata_ptr);
	}else if (!strncasecmp(r_frame_ptr, "cosmosinfo", offset)){
		download_cosmosinfo(cfgdata_ptr);
	}else if (!strncasecmp(r_frame_ptr, "cosmostcp", offset)){
		download_cosmostcp(cfgdata_ptr);
	}else if (!strncasecmp(r_frame_ptr, "updatek64", offset)){
		update_k64(cfgdata_ptr);
	}else{
		printf("\nError cfg package!");
		copy_len = 3;
		_mem_copy("err", cfgrsp_ptr+offset+2, copy_len);
		cfg_rsp_uplk_send(cfgrsp_ptr, offset+2+copy_len);
		return 1;
	}
	
	/* 最终返回int数据，自动寻找int对应的字符串copy到rsp_ptr */
	copy_len = 2;
	_mem_copy("ok", cfgrsp_ptr+offset+2, copy_len);
	cfg_rsp_uplk_send(cfgrsp_ptr, offset+2+copy_len);
	
	return 0;
}


