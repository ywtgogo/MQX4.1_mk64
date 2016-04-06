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
*END************************************************************************/

#ifndef __ENET_CFG_H__
#define __ENET_CFG_H__


#include <mqx.h>
#include <bsp.h>
#include <lwevent.h>
#include <message.h>
#if MQX_KERNEL_LOGGING
#include <klog.h>
#endif

#define DEMOCFG_ENABLE_SERIAL_SHELL    1   /* enable shell task for serial console */
#define DEMOCFG_ENABLE_SPIFLASH_FILESYSTEM  1   /* enable USB mass storage */
#define DEMOCFG_ENABLE_FTP_TEST        1   /* enable ftp server */
#define DEMOCFG_ENABLE_TELNET_TEST     1   /* enable telnet server */
#define DEMOCFG_ENABLE_TFTP_TEST       0   /* enable tftp server */
#define RTCSCFG_ENABLE_SNMP_TEST       0
#define DEMOCFG_ENABLE_KLOG            0   /* enable kernel logging */

//#if (DEMOCFG_ENABLE_FTP_TEST + DEMOCFG_ENABLE_TELNET_TEST + DEMOCFG_ENABLE_TFTP_TEST + RTCSCFG_ENABLE_SNMP_TEST) != 1
//#warning Please enable one of the network services. The restriction is only for RAM size limited devices.
//#endif

/* default addresses for external memory pools and klog */
#include <rtcs.h>
#include <ftpc.h> 
#include <ftpsrv.h> 

//extern void HVAC_initialize_networking(void);

#ifndef ENET_IPADDR
   #define ENET_IPADDR  IPADDR(192,168,1,6) 
#endif

#ifndef ENET_IPMASK
   #define ENET_IPMASK  IPADDR(255,255,255,0) 
#endif

#ifndef ENET_IPGATEWAY
   #define ENET_IPGATEWAY  IPADDR(169,254,0,1) 
#endif

#ifndef DEMOCFG_DEFAULT_DEVICE
   #define DEMOCFG_DEFAULT_DEVICE   0
#endif

#if DEMOCFG_ENABLE_SPIFLASH_FILESYSTEM
#define DEMOCFG_SPIFLASH_FS_DEVICE      "nftl:"
#define DEMOCFG_SPIFLASH_FS_NAME        "a:"
#endif

/* Update cfg */
#define DEMOCFG_SPI_FLASH_CLK_MODE    SPI_CLK_POL_PHA_MODE0
#define DEMOCFG_SPI_FLASH_CLK_FREQ    20000000
#define DEMOCFG_SPI_FLASH_CHANNEL     "spi1:"

#define DEMOCFG_SPI_FLASH_DEV_NAME     "spiflash:"

//#define DEMOCFG_NEW_IMAGE_OFFSET      (256 * 1024)    /* 256K */
//#define DEMOCFG_NEW_IMAGE_AREA_LEN    (256 * 1024)    /* 256K */
//#define DEMOCFG_RECOVERY_IMAGE_OFFSET (512 * 1024)    /* 512K */
//#define DEMOCFG_RECOVERY_IMAGE_AREA_LEN    (256 * 1024)    /* 256K */
/* ywt modify */
#define DEMOCFG_NEW_IMAGE_OFFSET      (256 * 1024)    /* 256K */
#define DEMOCFG_NEW_IMAGE_AREA_LEN    (512 * 1024)    /* 512K */

#define DEMOCFG_UPDATE_BUFFER_SIZE    (1024 * 1)

#include <shell.h>

enum 
{
   SHELL_TASK = 1,
   LOGGING_TASK,
};

void Shell_Task(uint32_t param);
void Logging_task(uint32_t param);
void USB_task(uint32_t param);

#endif
