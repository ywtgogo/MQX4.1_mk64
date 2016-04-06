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
* $FileName: Shell_Task.c$
* $Version : 3.7.5.0$
* $Date    : Jan-17-2011$
*
* Comments:
*
*   
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h> 
#include <shell.h>
#include <rtcs.h>
#include <sh_rtcs.h>

extern int32_t Shell_Ntp(int32_t argc, char *argv[] );
/*TASK*-----------------------------------------------------------------
*
* Function Name  : Shell_Task
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/
const SHELL_COMMAND_STRUCT Shell_commands[] = {
#if DEMOCFG_ENABLE_USB_FILESYSTEM 
   { "cd",        Shell_cd },      
   { "copy",      Shell_copy },     
   { "del",       Shell_del },       
   { "dir",       Shell_dir },      
   { "mkdir",     Shell_mkdir },     
   { "pwd",       Shell_pwd },       
   { "read",      Shell_read },      
   { "ren",       Shell_rename },    
   { "rmdir",     Shell_rmdir },
   { "type",      Shell_type },
   { "write",     Shell_write },      
#endif
   { "exit",      Shell_exit },      

   { "gate",      Shell_gate },
   { "help",      Shell_help },
   { "ipconfig",  Shell_ipconfig },
   { "netstat",   Shell_netstat },
#if RTCSCFG_ENABLE_ICMP
   { "ping",      Shell_ping },
#endif
#if RTCSCFG_ENABLE_SNMP_TEST
   { "snmpd",     Shell_SNMPd },
#endif
#if DEMOCFG_ENABLE_TELNET_TEST
   { "telnet",    Shell_Telnet_client },
   { "telnetd",   Shell_Telnetd },
#endif
#if DEMOCFG_ENABLE_PPP
   { "ppp",       Shell_ppp},
#endif
   {"ntp",        Shell_Ntp},
   { "?",         Shell_command_list },     
   { NULL,        NULL } 
};

void Shell_Task(uint32_t param)
{ 
   /* Run the shell on the serial port */
   for(;;)  {
      Shell(Shell_commands, NULL);
      printf("Shell exited, restarting...\n");
   }
}

/* EOF */
