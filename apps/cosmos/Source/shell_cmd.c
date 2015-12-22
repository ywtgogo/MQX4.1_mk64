/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 1989-2008 ARC International
*
* This software is owned or controlled by Freescale Semiconductor.
* Use of this software is governed by the Freescale MQX RTOS License
* distributed with this Material.
* See the MQX_RTOS_LICENSE file distributed for more details.
*
* Brief License Summary:
* This software is provided in source form for you to use free of charge,
* but it is not open source software. You are allowed to use this software
* but you cannot redistribute it or derivative works of it in source form.
* The software may be used only in connection with a product containing
* a Freescale microprocessor, microcontroller, or digital signal processor.
* See license agreement file for full license terms including other
* restrictions.
*****************************************************************************
*
* Comments:
*
*   This file contains the RTCS shell.
*
*
*END************************************************************************/


#include <mqx.h>
#include <rtcs.h>
#include <shell.h>
#include <sh_rtcs.h>
#include <ftpc.h> 
#if SHELLCFG_USES_MFS
#include <sh_mfs.h>
#endif
#include "spinorshell.h"
#include "gw_eth.h"

#define GWCMD_ENABLE_TEST		1

#if ! SHELLCFG_USES_RTCS
#error This application requires SHELLCFG_USES_RTCS defined non-zero in user_config.h. Please recompile libraries with this option if any Ethernet interface is available.
#endif

//sh_user
extern int32_t	Shell_update(int32_t argc, char *argv[]);
extern int32_t 	Shell_reboot(int32_t argc, char *argv[]);
extern int32_t	Shell_systime(int32_t argc, char *argv[]);
extern int32_t 	Shell_rf(int32_t argc, char *argv[]);
extern int32_t 	Shell_block(int32_t argc, char *argv[]) ;
extern int32_t 	Shell_wifi_cfgset(int32_t argc, char *argv[]);

const SHELL_COMMAND_STRUCT Shell_commands[] = {
	
//    { "arpadd",    Shell_arpadd },
//    { "arpdel",    Shell_arpdel },
    { "arpdisp",   Shell_arpdisp },
#if RTCSCFG_ENABLE_NAT
    { "dnat",      Shell_dnat },
#endif
    { "email",     Shell_smtp },
#if RTCSCFG_ENABLE_UDP
    { "exec",      Shell_exec },
#endif
    { "gate",      Shell_gate },
    { "gethbn",    Shell_get_host_by_name },//不知道具体是干嘛的！？
    { "getrt",     Shell_getroute },
    { "help",      Shell_help },
    { "ipconfig",  Shell_ipconfig },
#if RTCSCFG_ENABLE_UDP
//    { "load",      Shell_load },
#endif
#if RTCSCFG_ENABLE_NAT
    { "natinit",   	Shell_natinit },
    { "natinfo",   	Shell_natinfo },
#endif
    { "netstat",   	Shell_netstat },
#if RTCSCFG_ENABLE_ICMP
    { "ping",      	Shell_ping },
#endif
#if RTCSCFG_ENABLE_UDP
    { "sendto",    	Shell_sendto },
#endif
    { "telnetd",   	Shell_Telnetd },
#if RTCSCFG_ENABLE_UDP
//    { "tftp",      Shell_TFTP_client },
#endif
	{ "ftp",       	Shell_FTP_client },
    { "walkrt",    	Shell_walkroute },

#if SHELLCFG_USES_MFS
	{ "cd",             Shell_cd },       
	{ "copy",           Shell_copy },
	{ "create",         Shell_create },
	{ "del",            Shell_del },       
//	{ "disect",         Shell_disect},      
	{ "dir",            Shell_dir }, 
	{ "df",             Shell_df },
//	{ "di",             Shell_di },          
	{ "exit",           Shell_exit }, 
	{ "format",         Shell_format },
	{ "mkdir",          Shell_mkdir },     
	{ "pwd",            Shell_pwd },       
	{ "read",           Shell_read },      
	{ "ren",            Shell_rename },     
	{ "rmdir",          Shell_rmdir },
	{ "sh",             Shell_sh },
//	{ "type",           Shell_type },
	{ "write",          Shell_write },     
	{ "fsopen",         Shell_fs_open },     
	{ "fsclose",        Shell_fs_close },     
	{ "norchip",      	Shell_spinor_erase_chip },
	{ "norblock",	    Shell_drivertest},
//	{ "nftl",           Shell_nftl_test},		
#endif
	
	{ "update",    	Shell_update},	
	{ "reboot", 	Shell_reboot },
	{ "wificfg",   	Shell_wifi_cfgset},
	{ "systime",   	Shell_systime},
	{ "shquit", 	Shell_block },
	{ "rf",	   		Shell_rf },
#if GWCMD_ENABLE_TEST	
//	{ "remoteupdate",	   remote_update},
#endif
    { "?",         Shell_command_list },
    { NULL,        NULL }
};

const SHELL_COMMAND_STRUCT Telnetd_shell_commands[] = {
    { "exit",      Shell_exit },
    { "gethbn",    Shell_get_host_by_name },
    { "getrt",     Shell_getroute },
    { "help",      Shell_help },
    { "ipconfig",  Shell_ipconfig },
    { "netstat",   Shell_netstat },
    { "pause",     Shell_pause },
#if RTCSCFG_ENABLE_ICMP
    { "ping",      Shell_ping },
#endif
    { "telnet",    Shell_Telnet_client },
#if RTCSCFG_ENABLE_UDP
    { "tftp",      Shell_TFTP_client },
#endif
    { "walkrt",    Shell_walkroute },
    { "?",         Shell_command_list },
    { NULL,        NULL }
};

const FTPc_COMMAND_STRUCT FTPc_commands[] = {
   { "ascii",        FTPc_ascii       },
   { "binary",       FTPc_binary      },
   { "bye",          FTPc_bye         },
   { "cd",           FTPc_cd          },
   { "close",        FTPc_close       },
   { "delete",       FTPc_delete      },
   { "get",          FTPc_get         },
   { "help",         FTPc_help        },
   { "ls",           FTPc_ls          },
   { "mkdir",        FTPc_mkdir       },
   { "open",         FTPc_open        },
   { "pass",         FTPc_pass        },
   { "put",          FTPc_put         },
   { "pwd",          FTPc_pwd         },
   { "remotehelp",   FTPc_remotehelp  },
   { "rename",       FTPc_rename      },
   { "rmdir",        FTPc_rmdir       },
   { "user",         FTPc_user        },
   { NULL,   NULL }
};






/* EOF */
