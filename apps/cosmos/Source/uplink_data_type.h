#ifndef _UPLINK_DATA_H
#define _UPLINK_DATA_H


#define UPRX_BUFFER_SIZE 1024 
#define UPTX_BUFFER_SIZE 1024

typedef enum{
	SOCKET_OK,		//0
	SOCKET_INIT,	//1
	SOCKET_TIMEOUT,	//2
	SOCKET_DNS_ERR,	//3
	SOCKET_RECV_ERR,//4
	SOCKET_SEND_ERR,//5
}SocketSta;

typedef enum{
	UPLINK_OK,
	RTCS_ERR,
	PLUG_ERR,
	INIT_ERR,
	STATIC_ERR,
	DHCP_ERR,
	DNS_ERR,
	CONNECT_ERR,
	UPLINK_ERR,		//可嵌套socket错误代码
}UplinkSta;

#pragma   pack(1)
typedef struct SOCKET_CONNECT{
	uint32_t	ClientSock;
	char 		Domian[36];
	uint32_t	IpAddr;
	uint16_t	IpPort;
	uint8_t		Status;
}SOCKET_CONNECT_t;

typedef struct CONNREG_TYPE{
	uint8_t		Head;
	uint32_t	Uid;
	uint32_t	VerNo;
	uint16_t	VerTime;
	uint8_t		Dummy;
}CONNREG_TYPE_t;

#pragma   pack()

typedef struct HEARTBEAT_TYPE{
	uint32_t	Uid;
	uint32_t	UtcSec;
}HEARTBEAT_TYPE_t;

extern SOCKET_CONNECT_t socket_info[3];
extern bool WifiTcp1Stat;
extern bool WifiTcp2Stat;

#define UPLK_HEAD_CFG_FUNC				0x17
#define UPLK_HEAD_RLTIME_FUNC			0x15
#define UPLK_HEAD_BYPASS_FUNC			0x13
#define UPLK_HEAD_UPGRADE_FUNC			0x30

#define UPLK_CFG_10_FUNC				0x10
#define UPLK_CFG_51_FUNC				0x51
#define UPLK_CFG_66_FUNC				0x66
#define UPLK_CFG_45_FUNC				0x45

//#define UPLK_CFG_0B_FUNC				0x0B
//#define UPLK_CFG_UpgradeCosmos_FUNC		0x02
//#define UPLK_CFG_UpgradeNode_FUNC		0x30

#define UPLK_RLTIME_43_FUNC				0x43
#define UPLK_RLTIME_46_FUNC				0x46
#define UPLK_RLTIME_04_FUNC				0x04
#define UPLK_RLTIME_03_FUNC				0x03
#define UPLK_RLTIME_41_FUNC				0x41
#define UPLK_RLTIME_44_FUNC				0x44

#define UPLK_UPGRADE_05_FUNC			0x05
#define UPLK_UPGRADE_31_FUNC			0x31


#define UPLK_CMD_UID_LEN				4

//#define UPLK_CMD_Type_OFFSET			0		//定义透传[13]、实时命令[15]、配置命令[17]
#define UPLK_CMD_Uid_OFFSET				0		//Cosmos UID 四个字节[2000011C]
#define UPLK_CMD_COMMON_Apn_OFFSET		5		//命令通用功能号位置

//#define UPLK_CMD_CFG_Apn_OFFSET			6		//与REALTIME命令前导保持一致	[51]
//#define UPLK_CMD_CFG_Len_OFFSET			7		//定义为uint16_t				[0067]
//#define UPLK_CMD_CFG_Content_OFFSET		9		//配置内容起始偏移，定义结构体，尽量和节点发送的内容共用结构体
//
//#define UPLK_CMD_REALTIME_Apn_OFFSET	6		//与Cosmos I兼容				[43]
//#define UPLK_CMD_REALTIME_Len_OFFSET	7		//使用Cosmos I中的该标志位，用来发送uint8_t len，最大长度256	[8]

typedef enum Cfg_Err_Code
{
    CFG_OK,
    CFG_ERR,
	CFG_ERR_PAKG,
    CFG_ERR_CMDDELAY,
    CFG_ERR_SUBNET,
    CFG_ERR_PHUBINFO,
	CFG_ERR_COSMOSINFO,
	CFG_ERR_COSMOSTCP,
	CFG_ERR_UPDATEK64,
}CFG_ERR_CODE;

uint32_t download_cfgmix(char *r_fram_ptr);
uint32_t cfg_51_save(char *r_frame_ptr);
uint32_t cfg_66_save(char *r_frame_ptr);
void cfg_10_save(char *r_frame_ptr);
/*** uplink_mix data read ***/
void uplink_recv_mix(char *buffer, uint32_t buflen, uint32_t socketID);
uint32_t uplk_send(char *buffer, uint16_t buflen, ...);
bool check_network(void);
uint32_t systime_get(void);
bool uplk_send_connreg(void);
void rtctime_set(uint32_t);

#endif