//检查以太网连接的有效性
#include "global.h"
#include "task.h"
#include "internal_mem.h"
#include "uplink_data_type.h"
#include <ipcfg.h>
#include "kwtj_GWII.h"

char PlugLedBlinkStat;

#define ENET_DEVICE BSP_DEFAULT_ENET_DEVICE
#ifdef ENET_DEVICE
	#ifndef ENET_IPADDR
		#define ENET_IPADDR  IPADDR(192,168,1,11) 
	#endif
	#ifndef ENET_IPMASK
		#define ENET_IPMASK  IPADDR(255,255,255,0) 
	#endif
	#ifndef ENET_IPGATEWAY
		#define ENET_IPGATEWAY  IPADDR(192,168,1,1)
	#endif
#endif  /* ENET_DEVICE */

bool check_network(void)
{
	uint32_t	error;
	if (!ipcfg_get_link_active(BSP_DEFAULT_ENET_DEVICE)) {
		printf ("\nCable didn't plug");
		lwgpio_set_value(&gw_plug_led, LWGPIO_VALUE_HIGH);
		return FALSE;
	}
	lwgpio_set_value(&gw_plug_led, LWGPIO_VALUE_LOW);

	if (ipcfg_get_state(BSP_DEFAULT_ENET_DEVICE) == IPCFG_STATE_UNBOUND){// IPCFG_STATE_UNBOUND) {/* get uplinkstat manualy */
		//闪烁plug，DHCP失败，或DHCP获取中
		PlugLedBlinkStat = TRUE;
		printf ("\nDhcp binding ");
		error = ipcfg_bind_dhcp_wait(BSP_DEFAULT_ENET_DEVICE, FALSE, NULL);
		if (error != IPCFG_OK) {
			printf ("Error during dhcp bind %08x!\n", error);
			return FALSE;
		}else { 
			printf ("Bound ok! ");
		}
	}
	PlugLedBlinkStat = FALSE;
	lwgpio_set_value(&gw_plug_led, LWGPIO_VALUE_LOW);
	return TRUE;
}

static UplinkSta setup_network(void)
{
	uint32_t     			error;
	UplinkSta 				result = UPLINK_OK;
	_enet_address   		enet_address;
	IPCFG_IP_ADDRESS_DATA   ip_data;
	//IPCFG_IP_ADDRESS_DATA   auto_ip_data;
	
	ip_data.ip = ENET_IPADDR;
	ip_data.mask = ENET_IPMASK;
	ip_data.gateway = ENET_IPGATEWAY;
	/* runtime RTCS configuration for devices with small RAM, for others the default BSP setting is used */
	/* default 4,2,20 */
	//	_RTCSPCB_init = 8;
	//	_RTCSPCB_grow = 4;
	//	_RTCSPCB_max = 32;
	//	_RTCS_msgpool_init = 8;
	//	_RTCS_msgpool_grow = 4;
	//	_RTCS_msgpool_max  = 32;
	//	_RTCS_socket_part_init = 8;
	//	_RTCS_socket_part_grow = 4;
	//	_RTCS_socket_part_max  = 32;
    /* _RTCSTASK_stacksize = 3000;//default */

	error = RTCS_create();
    if (error != RTCS_OK) {
        printf("\nRTCS failed to initialize, error = %X", error);
		return RTCS_ERR;   
	}
    
	//_IP_forward = TRUE;	//路由协议，内网IP转发
	/* Get the Ethernet address of the device */
	ENET_get_mac_address(BSP_DEFAULT_ENET_DEVICE, CosmosInfoPtr->CosmosUid, enet_address);
	/* Initialize the Ehternet device */
	if (IPCFG_OK != ipcfg_init_device(BSP_DEFAULT_ENET_DEVICE, enet_address)) {
		printf ("\nInit eth dev fails");
		return INIT_ERR;
	}
	if (TRUE != ipcfg_get_link_active(BSP_DEFAULT_ENET_DEVICE)) {
		printf ("\nCable didn't plug");
		return  PLUG_ERR;
	}
	//	/* Bind Ethernet device to network using constant (static) IP address information */
	//	error = ipcfg_bind_staticip(BSP_DEFAULT_ENET_DEVICE, &ip_data);
	//	if (error) return STATIC_ERR;
	printf ("\nDhcp binding ");
	error = ipcfg_bind_dhcp_wait(BSP_DEFAULT_ENET_DEVICE, FALSE, NULL);
    if (error != IPCFG_OK) {
        printf ("Error during dhcp bind %08x!\n", error);
        result = DHCP_ERR;
    }else 
		printf ("Bound ok! ");

	return result;
}

/*
*	socket连接函数
*	传入_ip_address，port
*   return 错误/标号
*/
static uint32_t socket_create(_ip_address ser_ipadder, uint16_t	ser_port)
{
	uint32_t 			client_sock;
	uint32_t 			error, is_error;
	sockaddr_in 		server_addr;
	uint32_t 			option;
	uint32_t 			result;
	uint32_t			sys_time;
	TIME_STRUCT			mqx_time;
	//char 	sendbuf[] = {0x20, 0x20,0x00,0x01,0x1c,0x00,0x00,0x01,0x08,0x21,0x00,0x00};//03 08 06 05 28 版本号+发布时间
	
	memset((char *) &server_addr, 0, sizeof(sockaddr_in));
	client_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (client_sock == RTCS_SOCKET_ERROR) {
		shutdown(client_sock, FLAG_ABORT_CONNECTION);
		return RTCS_ERROR;
	}
#if 1	//设置eth参数
	option = 2048;   
	is_error = setsockopt(client_sock, SOL_TCP, OPT_TBSIZE, &option, sizeof(option));
//	option = SOCKTSRVCFG_CONNECT_TIMEOUT;
//	is_error = setsockopt(clinet_primarysock, SOL_TCP, OPT_CONNECT_TIMEOUT, &option, sizeof(option));
//	option = SOCKTSRVCFG_TIMEWAIT_TIMEOUT;
//	is_error = setsockopt(clinet_primarysock, SOL_TCP, OPT_TIMEWAIT_TIMEOUT, &option, sizeof(option));

	option = 60000UL;	//60s连接超时
	is_error = setsockopt(client_sock, SOL_TCP, OPT_CONNECT_TIMEOUT, &option, sizeof(option));	
	option = 3000;
	is_error = setsockopt(client_sock, SOL_TCP, OPT_MAXRTO, &option, sizeof(option));
	option = FALSE;//TRUE
	is_error = setsockopt(client_sock, SOL_TCP, OPT_SEND_NOWAIT, &option, sizeof(option));	
	option = 500;
	is_error = setsockopt(client_sock, SOL_TCP, OPT_SEND_TIMEOUT, &option, sizeof(option));			
	option = TRUE;//FALSE
	is_error = setsockopt(client_sock, SOL_TCP, OPT_RECEIVE_NOWAIT, &option, sizeof(option)); //不等待缓冲区，不阻塞
	option = TRUE;
	is_error = setsockopt(client_sock, SOL_TCP, OPT_RECEIVE_PUSH, &option, sizeof(option));	//主动返回	
	option = SOCKTSRVCFG_RECEIVE_TIMEOUT;
	is_error = setsockopt(client_sock, SOL_TCP, OPT_RECEIVE_TIMEOUT, &option, sizeof(option));	
    if (is_error)
	{
		return RTCS_ERROR;
	}
#endif
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = ser_port;
	server_addr.sin_addr.s_addr = ser_ipadder;
	
	printf("\n\nconnecting to data center: %d.%d.%d.%d  port:%d", IPBYTES(server_addr.sin_addr.s_addr), server_addr.sin_port);
	result = connect(client_sock, &server_addr, sizeof(server_addr));
	if (result != RTCS_OK)
	{
//		shutdown(clinetsock, FLAG_ABORT_CONNECTION);
//		if (RTCS_OK != connect(clinetsock, &server_addr, sizeof(server_addr)))
//		{
			printf(" failed");
			printf("\nError--connect() failed with error code %lx.", result);
			print_sys_time();
			shutdown(client_sock, FLAG_ABORT_CONNECTION);
			return RTCS_ERROR;
//		}
	}	
	printf(" ---connected");
	
	return client_sock;
}

static UplinkSta tcp_connect(uint8_t counttest, uchar *cfginfotest)
{
	uint32_t	ipaddr;
	UplinkSta	result;
	bool	changed = FALSE;
	bool	last = FALSE;
	
	for (int i=0 ;i<CosmosTcpPtr->CellCount; i++)
	{		
		_mem_copy(CosmosTcpPtr->TcpCell[i].TcpDns, socket_info[i].Domian, strlen(CosmosTcpPtr->TcpCell[i].TcpDns));
		socket_info[i].IpPort = CosmosTcpPtr->TcpCell[i].TcpPort;
		/* 验证连接的有效性 */
		if (socket_info[i].Status != SOCKET_OK)
		{	
			_time_delay(100);
			if (TRUE != RTCS_resolve_ip_address(socket_info[i].Domian, &ipaddr, NULL, 0)) {
				//ipaddr = socket_info[i].IpAddr;	
				printf("\n\nParse the data server domain timeout %s", socket_info[i].Domian);
				socket_info[i].Status = SOCKET_DNS_ERR;
				result = DNS_ERR;
				continue;
			}
			//socket_info[i].Status = connect_ser(socket_info[i].ClientSock, ipaddr, socket_info[i].IpPort);
			socket_info[i].ClientSock = socket_create(ipaddr, socket_info[i].IpPort);
			if (socket_info[i].ClientSock == RTCS_ERROR){
				//printf("\nError--connect() failed with error code %lx.", socket_info[i].ClientSock);
				socket_info[i].Status = SOCKET_TIMEOUT;
				result = CONNECT_ERR;
			}else{
				socket_info[i].Status = SOCKET_OK;
				result = UPLINK_OK;
				uplk_send_connreg();
				/* 标记状态 */
				changed = TRUE;
			}
		}else{
			socket_info[i].Status = SOCKET_OK;
			result = UPLINK_OK;
		}
	}
	/* 触发离线数据上传任务---暂定创建一个新的 */
	if (changed != last)
		_task_create(0,UPLK_OFFLINE_TASK,0);
	return result;
}

void uplk_check_task(uint32_t parameter)
{
	UplinkSta		uplinkstat;
	uplinkstat = setup_network();
	
	systime_get();
	
	ipcfg_task_create(8, 2000);
	if (! ipcfg_task_status()) _task_block();
	
	/* while(1) if not set Uid || not set TCP */
	while(1) {
		if (CosmosInfoPtr->InitHead&INIT_HEAD_COSMOS_TCP\
			&&CosmosInfoPtr->InitHead&INIT_HEAD_COSMOS_INFO)
		{
			printf("\nUplk task runing...");
			_time_delay(1000);
			break;
		}else{
			printf("\nPlease config TCP infomation & Cosmos UID 0x%08x", CosmosInfoPtr->InitHead);
			_time_delay(8000);
			continue;
		}
	}	
	
	if (CosmosTcpPtr->CellCount > 5) {
		/* need to restart */
		CosmosTcpPtr->CellCount = 1;
	}
	for (int i=0; i<CosmosTcpPtr->CellCount; i++){
		socket_info[i].Status = SOCKET_INIT;
	}
	if (uplinkstat != UPLINK_OK) {
	//		printf("\nUPLINK_ERR task need restart ");/*task_restart*/
	//		printf ("Error uplinkstat %02x!\n", uplinkstat);
	}else {
		/* 连接不成功暂阻塞60s*/
		tcp_connect(CosmosTcpPtr->CellCount, (uchar *)CosmosTcpPtr->TcpCell);
	}
	/* test	code */
	/* 需要在任务内部获得网络连接的状态 */
	_task_create(0,UPLK_READ_TASK,0);
	_task_create(0,HEARTBEAT_TASK,0);
	
	if (CosmosInfoPtr->FuncEn&FUNC_EN_RF) {
		_task_create(0,SCAN_TASK,0);
	}else {
		printf("\nFUNC_RF has been set disable.");
	}
#if 0
	if (CosmosInfoPtr->FuncEn&FUNC_EN_RS485) {
		_task_create(0,RS485LK_READ_TASK,0);
	}else {
		printf("\nFUNC_RS485 has been set disable.");
	}
#endif
	/* verify socket connect */
	while(1)
	{
		if (check_network()){/* get uplinkstat manualy */			
			uplinkstat = tcp_connect(3, NULL);
		}else{
			/* 重新处理物理连接 network setup 功能*/
			printf("\nPlease check the network cable, and DHCP server");
			for (int i=0; i<CosmosTcpPtr->CellCount; i++){
				socket_info[i].Status = SOCKET_INIT;
			}
			_time_delay(5*1000);
		}
		_time_delay(5*1000);
	}

	/* need to be restart */
	//	_time_delay(5*1000);
	//	_task_block();
}