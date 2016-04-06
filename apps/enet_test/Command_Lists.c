/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
* All Rights Reserved
*
*************************************************************************** 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: Command_Lists.c$
* $Version : 3.5.16.0$
* $Date    : Dec-2-2009$
*
* Comments:
*
*   
*
*END************************************************************************/

#include "enet_cfg.h"
#include "sh_mfs.h"

extern int32_t  Shell_update(int32_t argc, char *argv[] );

const SHELL_COMMAND_STRUCT Shell_commands[] = {
#if DEMOCFG_ENABLE_SPIFLASH_FILESYSTEM 
   { "cd",             Shell_cd },
    { "copy",           Shell_copy },
    { "create",         Shell_create },
    { "del",            Shell_del },
    { "dir",            Shell_dir },
    { "exit",           Shell_exit },
    { "format",         Shell_format },
    { "mkdir",          Shell_mkdir },
    { "pwd",            Shell_pwd },
    { "read",           Shell_read },
    { "ren",            Shell_rename },
    { "rmdir",          Shell_rmdir },
    { "type",           Shell_type },
    { "write",          Shell_write },
#endif
   { "exit",      Shell_exit },      

   //{ "gate",      Shell_gate },
   { "help",      Shell_help },
   { "ipconfig",  Shell_ipconfig },
   //{ "netstat",   Shell_netstat },
#if RTCSCFG_ENABLE_ICMP
   { "ping",      Shell_ping },
#endif
#if RTCSCFG_ENABLE_SNMP_TEST
   { "snmpd",     Shell_SNMPd },
#endif
#if DEMOCFG_ENABLE_TELNET_TEST
   //{ "telnet",    Shell_Telnet_client },
   { "telnetd",   Shell_Telnetd },
#endif
#if DEMOCFG_ENABLE_FTP_TEST
   { "ftp",       Shell_FTP_client },
   { "ftpsrv",      Shell_ftpsrv },
#endif
#if RTCSCFG_ENABLE_UDP
#if DEMOCFG_ENABLE_TFTP_TEST
   { "tftp",      Shell_TFTP_client },
   { "tftpd",     Shell_TFTPd },
#endif
#endif

#if DEMOCFG_ENABLE_PPP
   { "ppp"    ,  Shell_ppp},
#endif
   { "update",    Shell_update},

   { "?",         Shell_command_list },     
   { NULL,        NULL } 
};

#if DEMOCFG_ENABLE_TELNET_TEST
const SHELL_COMMAND_STRUCT Telnetd_shell_commands[] = {
    { "exit",      Shell_exit },
    { "help",      Shell_help },
    { "ipconfig",  Shell_ipconfig },
    { "netstat",   Shell_netstat },
    { "pause",     Shell_pause },
#if RTCSCFG_ENABLE_ICMP
    { "ping",      Shell_ping },
#endif
#if DEMOCFG_ENABLE_TELNET_TEST
    { "telnet",    Shell_Telnet_client },
#endif
#if DEMOCFG_ENABLE_FTP_TEST
    { "ftpsrv",    Shell_ftpsrv },
#endif
#if RTCSCFG_ENABLE_UDP
#if DEMOCFG_ENABLE_TFTP_TEST
    { "tftp",      Shell_TFTP_client },
#endif
#endif
    { "walkrt",    Shell_walkroute },
    { "update",    Shell_update},
    { "?",         Shell_command_list },
    { NULL,        NULL }
};
#endif

#if DEMOCFG_ENABLE_FTP_TEST
// ftp root dir
char FTPd_rootdir[] = {"c:\\"}; 

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
#endif

/* EOF */
