#ifndef	_RF_TYPE_H
#define _RF_TYPE_H

#define CMD_RSP_UID_LEN		4
#define CMD_RSP_STAT_LEN	3
#define CMD_RSP_ORG_LEN		32

#define CMD_RSP_Uid_OFFSET	0
#define CMD_RSP_Len_OFFSET	4
#define CMD_RSP_Apn_OFFSET	5
#define	CMD_RSP_Stat_OFFSET	6
#define CMD_RSP_ORG_OFFSET	7

#define NEW_DATA_HEAD_LEN	1
#define NEW_DATA_UID_LEN	4
#define NEW_DATA_DUMMY_LEN	1
#define NEW_DATA_SIZE_LEN	2
#define NEW_DATA_STAT_LEN	8

#define NEW_DATA_Head_OFFSET	0
#define NEW_DATA_Uiu_OFFSET		1
#define NEW_DATA_Obj_OFFSET		5
#define NEW_DATA_Size_OFFSET	6
#define	NEW_DATA_Data_OFFSET	8

//#include <mqx.h>
// Addr:0x0000  Preamble
// Addr:0x1000  Configurable Reg
// Addr:0x2000  HopTable
// Addr:0x3000  RfdTable
// Addr:0x4000  Status
#define NODE_PREAMBLE_REG_OFFSET	0X0000
#define NODE_CFG_REG_OFFSET 		0X1000
#define NODE_HOPTABLEL_REG_OFFSET 	0X2000
#define NODE_RFDTABLE_REG_OFFSET 	0X3000
#define NODE_STATUS_REG_OFFSET 		0X4000

#define NODE_ID_LEN					2
#define NODE_REGADDR_LEN			2
#define NODE_REGNUM_LEN				2
#define NODE_REGPARAM_LEN			2
#define NODE_APN_LEN				1

//发送值偏移-前导
#define NODE_CMD_DestId_OFFSET		0
#define NODE_CMD_AFN_OFFSET			2
#define NODE_CMD_TargId_OFFSET		3	//43、46、04、41、44、45通用
#define NODE_CMD_ThirdTime_OFFSET	5	//解析43命令
#define NODE_CMD_TargRfdId_OFFSET	5	//解析03-Rfd命令
#define NODE_CMD_StartAddr_OFFSET	5	//解析04、66、10命令
#define NODE_CMD_NumOfNodes_OFFSET	7	//04、66
#define NODE_CMD_NumOfRegs_OFFSET	7	//10
#define NODE_CMD_NumOfBytes_OFFSET	9	//10、66
//发送值偏移-设置
#define	NODE_CMD_HopTable_OFFSET	5	//51
#define NODE_CMD_RfdIdList_OFFSET	10	//66
#define NODE_CMD_Crc_OFFSET(x)		(x)-2
//返回值特例
#define NODE_CMD_ExcCode_OFFSET		5	//错误代码
#define NODE_CMD_NumBytes_OFFSET	7   //03返回
#define NODE_CMD_RegRead_OFFSET		9	//03返回

//head的偏移字节数
#define	RFDTAB_HEAD_OFFSET			10

#define COO_ID {0x00,0x01}

typedef enum{
   NODE_ID,
   K_IIN,
   OSET_IIN,
   K_VIN,
   OSET_VIN,
   K_TEMP,
   OSET_TEMP_H,
   OSET_TEMP_L,
   TX_PWR,
   COMM_CHAN,
   PAN_ID,

   THD_VIN_H,
   THD_VIN_L,
   THD_IIN_H,
   THD_IIN_L,
   THD_TEMP_H,
   THD_TEMP_L,
   SAMP_PERIOD,
   OV_CYC,
   OC_CYC,
   OT_CYC,
   SW_DELAY,
   NWK_OTHER_RSND_TIME,
   SMP_ISP_RSND_TIME,
}NodeReg_t;

enum{
	EXC_OK,
	EXC_ADDR_RANGE    	= 2, 
	EXC_REGS_QUANT    	= 3, 
	EXC_FUNC_CODE     	= 4,
	EXC_CRC 	      	= 10,
	EXC_RFD_TIMEOUT   	= 9,
	EXC_TIMEOUT       	= 11,
	EXC_IDLE 	      	= 12,
	EXC_HopNodeId     	= 13,
	EXC_DestId        	= 14,
	EXC_Mode          	= 15,
	EXC_OPT_TIMEOUT		= 16,
	EXC_RADIO_FRM_CRC	= 17,
	EXC_OPT_CTRL_FAIL  	= 18,
	EXC_GTW_RSP_CRC    	= 19,
};
//gateway set
enum{
	P01_AFN_RF_ROUTE_TABLE	= 0X51,
	P01_AFN_FFD_RFDID		= 0X66,
	P01_AFN_NODE_SW 		= 0X45,
	P01_AFN_NODE_REG_SET	= 0X10,
	P01_AFN_NODE_REG_GET	= 0X03,
};
//poll
enum{
	P01_AFN_RF_SYNC		= 0X43,
	P01_AFN_RF_READY	= 0X46,
	P01_AFN_RF_SAMPLE	= 0X04,
	P01_AFN_RF_SYSSTART = 0X41,
	P01_AFN_RF_SYSSTOP	= 0X44,
};

enum{
	P01_AFN_NODE_BDENTER	= 0X31,
	P01_AFN_NODE_BDPREAMBLE	= 0X35,
	P01_AFN_NODE_BDFLASHER	= 0X32,
	P01_AFN_NODE_BDFLASHWR	= 0X33,
	P01_AFN_NODE_BDEND		= 0X34,
};

#pragma   pack(1)

typedef struct RF_RETURN_TYPE_NORMAL
{
	uint16_t	DestId;
	uint16_t	FuncCode;
	uint16_t	TargId;
	uint16_t	ExcCode;
}RF_RETURN_TYPE_NORMAL_t;

typedef struct RF_SAMPLE_TYPE {
	uint16_t 	RfId;
	uint16_t  	Vin;
	uint16_t  	Vout;
	uint16_t 	Iout;
	uint16_t 	Temper;
	uint16_t 	Err;
	uint32_t 	Energy;
} RF_SAMPLE_TYPE_t, *RF_SAMPLE_TYPE_PTR;

/*gateway networking struct*///51命令的底层格式
typedef struct HOP_TABLE {
	uint16_t	DestFfd;
	uint16_t	NextFfd;
	uint32_t	Dummy;
	uint16_t	PreFfd;
	uint16_t	LineNum;
} HOP_TABLE_t, *HOP_TABLE_PTR;
typedef struct RF_ROUTE_TABLE {	//51
	uint16_t	DestId;
	uint8_t		FuncCode;
	uint16_t	TargFfd;
	HOP_TABLE_t	Table[8];
} RF_ROUTE_TABLE_t, *RF_ROUTE_TABLE_PTR;

//66写入rfd的信息到ffd
typedef struct RFD_TABLE { //66 不定长

	uint16_t	DestId;
	uint8_t		FuncCode;
	uint16_t	TargFfd;
	uint16_t	StartAddr;
	uint16_t	NumOfNodes;
	uint8_t		NumOfBytes;
	uint16_t	RfdId[50]; //定长将直接写入Num
} RFD_TABLE_t, *RFD_TABLE_PTR;
//short CMD FuncCode
typedef struct RFD_BIND//45
{
	uint16_t	DestId;
	uint8_t		FuncCode;
	uint16_t	TargFfd;
} RFD_BIND_t, * RFD_BIND_PTR;

typedef struct RF_ID_MODIFY//45
{
	uint16_t	DestId;
	uint8_t		FuncCode;
	uint16_t	TargFfdId;	
	uint16_t	TargRfdId;
	uint16_t	StartAddr;
	uint16_t	NumOfRegs;
	uint8_t		NumOfBytes;
	uint16_t	TargModifyId;
} RF_ID_MODIFY_t, * RF_ID_MODIFY_PTR;

typedef struct NODE_SREG_SET
{
	uint16_t	DestId;
	uint8_t		FuncCode;
	uint16_t	TargId;
	uint16_t	TargRfdId;	
	uint16_t	StartAddr;//Reg Offset Addr
	uint16_t	NumOfRegs;
	uint8_t		NumOfBytes;
	uint16_t	RegTargVal;
}NODE_SREG_SET_t, NODE_SREG_SET_PTR;

//Node Reg Get Struct: 0x03
typedef struct NODE_REG_GET_CMD
{
	uint16_t	DestId;
	uint8_t		FuncCode;
	uint16_t	TargId;
	uint16_t	TargRfdId;
	uint16_t	StartAddr; //cmd_offset
	uint16_t	NumOfRegs;
} NODE_REG_GET_CMD_t;

typedef struct Preamble{ //0x0000
	uint8_t 	magic[2];
	uint32_t 	len;
	uint16_t  	vers;
	uint16_t  	manu;
	uint16_t  	prod;
} Preamble_t;
typedef struct NODE_REG_GET_Preamble{
	uint16_t	DestId;
	uint8_t		FuncCode;
	uint16_t	TargId;
	uint16_t	TargRfdId;
	uint16_t	NumOfBytes;
	Preamble_t 	Reg;
	uint16_t	CRC;
} NODE_REG_GET_Preamble_t;

typedef struct Configurable{ //0x1000
	uint16_t   NODE_ID;
	uint16_t   K_IIN;
	uint16_t   OSET_IIN;
	uint16_t   K_VIN;
	uint16_t   OSET_VIN;
	uint16_t   K_TEMP;
	uint16_t   OSET_TEMP_H;
	uint16_t   OSET_TEMP_L;
	uint16_t   TX_PWR;
	uint16_t   COMM_CHAN; 
	uint16_t   PAN_ID;
	uint16_t   THD_VIN_H;
	uint16_t   THD_VIN_L;
	uint16_t   THD_IIN_H;
	uint16_t   THD_IIN_L;
	uint16_t   THD_TEMP_H;
	uint16_t   THD_TEMP_L;
	uint16_t   SAMP_PERIOD;
	uint16_t   OV_CYC;
	uint16_t   OC_CYC;
	uint16_t   OT_CYC;
	uint16_t   SW_DELAY;
} Configurable_t;      //22
typedef struct NODE_REG_GET_Configurable{
	uint16_t	DestId;
	uint8_t		FuncCode;
	uint16_t	TargId;
	uint16_t	TargRfdId;
	uint16_t	NumOfBytes;
	Configurable_t 	Reg;
	uint16_t	CRC;
} NODE_REG_GET_Configurable_t;

typedef struct NODE_REG_GET_HopTable{
	uint16_t	DestId;
	uint8_t		FuncCode;
	uint16_t	TargId;
	uint16_t	TargRfdId;
	uint16_t	NumOfBytes;
	RF_ROUTE_TABLE_t 	Reg;		//最上面有定义
	uint16_t	CRC;
} NODE_REG_GET_HopTable_t;
//RfdTable为66命令含带的数据
typedef struct NODE_REG_GET_RfdTable{
	uint16_t	DestId;
	uint8_t		FuncCode;
	uint16_t	TargId;
	uint16_t	TargRfdId;
	uint16_t	NumOfBytes;
	uint16_t 	Reg[50];//不定长数据，最上面有定义
} NODE_REG_GET_RfdTable_t;

typedef struct NODE_REALT{
	uint16_t	DestId;
	uint8_t		FuncCode;
	uint16_t	TargId;
}NODE_REALT_t;

typedef struct NODE_SAMPLE{
	uint16_t	DestId;
	uint8_t 	FuncCode;
	uint16_t	TargId;
	uint8_t		StartAddr[2];
	uint16_t 	NodeNum;
}NODE_SAMPLE_t, * NODE_SAMPLE_PTR;

/*****************************************************************
 * MagBdFlashWr 0x33
 **************************************************//************/

typedef struct MagBdPreamble{
	uint16_t	DestId;
	uint8_t		FuncCode;
	uint16_t	TargId;
	uint16_t	TargRfdId;
	uint16_t	NumOfBytes;
	Preamble_t 	MagBdPreamble;//不定长数据
	uint16_t	CRC;
} MagBdPreamble_t;
 
/*****************************************************************
 * SysSync 0x43
 **************************************************//************/
typedef struct THIRD_TIME{
	uint8_t	Day;
	uint8_t	Hour;
	uint8_t	Minute;
	uint8_t	Second;
	uint8_t	MiliSecond_h;
	uint8_t MiliSecond_l;
}THIRD_TIME_t, * THIRD_TIME_PTR;
typedef struct TIME_SYNC{
	uint8_t		DestId[2];
	uint8_t		FuncCode;
	uint8_t		TargId[2];
	THIRD_TIME_t Third_Time;
} TIME_SYNC_t, * TIME_SYNC_PTR;

#pragma   pack()


uint32_t cmd_setrfroute(uchar *targid_ptr, uchar *routetab_ptr);


#endif 