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
* $FileName: RTCS.c$
* $Version : 3.8.20.0$
* $Date    : Jun-6-2012$
*
* Comments:
*
*   Example of HVAC using RTCS.
*
*END************************************************************************/

#include "enet_cfg.h"

#if DEMOCFG_USE_WIFI
#include "string.h"
#endif

#include <ipcfg.h>

void enet_initialize_networking(void) 
{
   int32_t                  error;
#if 0
   IPCFG_IP_ADDRESS_DATA   ip_data;
   _enet_address           enet_address;
#endif

#if RTCS_MINIMUM_FOOTPRINT
   /* runtime RTCS configuration for devices with small RAM, for others the default BSP setting is used */
   _RTCSPCB_init = 4;
   _RTCSPCB_grow = 2;
   _RTCSPCB_max = 20;
   _RTCS_msgpool_init = 4;
   _RTCS_msgpool_grow = 2;
   _RTCS_msgpool_max  = 20;
   _RTCS_socket_part_init = 4;
   _RTCS_socket_part_grow = 2;
   _RTCS_socket_part_max  = 20;
#endif

    error = RTCS_create();
    if (error)
        return;

#if 0
    LWDNS_server_ipaddr = ENET_IPGATEWAY;

    ip_data.ip = ENET_IPADDR;
    ip_data.mask = ENET_IPMASK;
    ip_data.gateway = ENET_IPGATEWAY;
    
    ENET_get_mac_address (DEMOCFG_DEFAULT_DEVICE, ENET_IPADDR, enet_address);
    error = ipcfg_init_device (DEMOCFG_DEFAULT_DEVICE, enet_address);

    error = ipcfg_bind_staticip (DEMOCFG_DEFAULT_DEVICE, &ip_data);

#if DEMOCFG_ENABLE_FTP_SERVER
   FTPd_init("FTP_server", 7, 3000 );
#endif

#if DEMOCFG_ENABLE_TELNET_SERVER
   TELNETSRV_init("Telnet_server", 7, 2000, (RTCS_TASK_PTR) &Telnetd_shell_template );
#endif
#endif
   
#if DEMOCFG_ENABLE_KLOG && MQX_KERNEL_LOGGING
   RTCSLOG_enable(RTCSLOG_TYPE_FNENTRY);
   RTCSLOG_enable(RTCSLOG_TYPE_PCB);
#endif
}

/* EOF */
