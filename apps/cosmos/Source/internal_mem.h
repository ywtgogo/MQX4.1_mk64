#ifndef _INTERNAL_MEM_H_
#define _INTERNAL_MEM_H


#define COSMOS_INFO_LOCATION		0x00000000
#define COSMOS_TCP_LOCATION			0x00000050
#define COSMOS_CMDDELAY_LOCATION	0x00000100	//最多占用10B

#define PHUB_INFO_LOCATION			0x00000200

#define SUBNET_LOCATION				0x00000400

/* big-edian cfg info*/
#define INIT_HEAD_COSMOS_INFO		0x00000001
#define INIT_HEAD_PHUB_INFO			0x00000002
#define INIT_HEAD_COSMOS_CMDDELAY	0x00000004
#define INIT_HEAD_COSMOS_TCP		0x00000008
#define INIT_HEAD_SUBNET			0x00000010

#define FUNC_EN_DEBUG				0x00000001
#define FUNC_EN_RF					0x00000002
#define FUNC_EN_SYNC				0x00000004
#define FUNC_EN_RS485				0x00000008

#define FUNC_EN_ETH					0x00000010
#define FUNC_EN_DHCP				0x00000020
#define FUNC_EN_TCPX				0x00000040
//#define FUNC_EN_DHCP				0x00000080

#define FUNC_EN_CMDLOG				0x00000100
#define FUNC_EN_DATALOG				0x00000200

#define PROTOCOL_EN_20H_PT			0x00000001
#define PROTOCOL_EN_80H_PT			0x00000002
#define PROTOCOL_EN_NEW				0x00000004
#define PROTOCOL_EN_OLD				0x00000008
//#define PROTOCOL_EN_OLD_
/* end big-edian */

#pragma   pack(1)
//PHUB_INFO_t外设配置信息
typedef struct RF_CFG{
	uint16_t		CooId;
}RF_CFG_t, *RF_CFG_PTR;
typedef struct RS485_CFG{
	uint8_t			En;
}RS485_CFG_t, *RS485_CFG_PTR;
typedef struct ETH_CFG{
	uint32_t		IpStatic;
	uint32_t		MaskStatic;
	uint32_t		GateStatic;
	uint32_t		DnsStatic;
	uchar			Dhcp;
//	uchar 			SntpDns[35];
}ETH_CFG_t, *ETH_CFG_PTR;
typedef struct PHUB_INFO{
	ETH_CFG_t		EthCfg;
	RF_CFG_t		RfCfg;
	RS485_CFG_t		Rs485Cfg;
}PHUB_INFO_t, *PHUB_INFO_PTR;
//end PHUB_INFO_t

typedef struct CMD_DELAY{
	uint8_t			SyncMin;
	uint8_t			SyncMax;
	uint8_t			StartSampleMin;
	uint8_t			StartSampleMax;
	uint8_t			CollectDataMin;
	uint8_t			CollectDataMax;
	uint8_t			FfdNext;
	uint8_t			Other;
}CMD_DELAY_t, *CMD_DELAY_PTR;

//子网配置信息，最大FFD个数为8个，每个FFD最大RFD为256个
//FFD的顺序需要根据该协议格式正确写入
typedef struct SUBNET_CELL{
	uint16_t		FfdId;
	uint8_t			Order;
	uint8_t			RfdCount;
	uint16_t		Rfd[50];
}SUBNET_CELL_t, *SUBNET_CELL_PTR;
typedef struct SUBNET{
	uint8_t			CellCount;
	SUBNET_CELL_t	SubNetCell[8];
}SUBNET_t, *SUBNET_PTR;

typedef struct TCP_CELL{
	char 			TcpDns[36];
	uint16_t 		TcpPort;
}TCP_CELL_t, *TCP_CELL_PTR;
typedef struct COSMOS_TCP{
	uint8_t			CellCount;
	TCP_CELL_t		TcpCell[3];
}COSMOS_TCP_t, *COSMOS_TCP_PTR;

typedef struct COSMOS_INFO{
	uint32_t		InitHead;	//32位检查初始化
//	uint32_t		VerNo;
//	uint32_t		VerTime;
	uint32_t		FuncEn;		//32位开关量
	uint32_t		ProtocolEn;	//32位协议开关量
	uint32_t		CosmosUid;
}COSMOS_INFO_t, *COSMOS_INFO_PTR;
#pragma   pack()

extern SUBNET_PTR		SubNetPtr;
extern CMD_DELAY_PTR	CmdDelayPtr;
extern PHUB_INFO_PTR	PhubInfoPtr;
extern COSMOS_INFO_PTR	CosmosInfoPtr;
extern COSMOS_TCP_PTR	CosmosTcpPtr;

void flashx_init(void);
//uint32_t flashx_write(MQX_FILE_PTR, uint32_t, char, uint32_t);

uint32_t sub_net_write(SUBNET_PTR, uint32_t);
uint32_t sub_net_read(void);
uint32_t cmd_delay_read(void);
uint32_t cmd_delay_write(CMD_DELAY_PTR, uint32_t);
uint32_t phub_info_write(PHUB_INFO_PTR, uint32_t);
uint32_t phub_info_read(void);
uint32_t cosmos_info_read(void);
uint32_t cosmos_info_write(COSMOS_INFO_PTR, uint32_t);
uint32_t cosmos_tcp_read(void);
uint32_t cosmos_tcp_write(COSMOS_TCP_PTR, uint32_t);
#endif