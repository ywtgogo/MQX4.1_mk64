/**HEADER********************************************************************
* 
* Copyright (c) 2013 Freescale Semiconductor;
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
* $FileName: main.h$
* $Version : 3.8.19.0$
* $Date    : Mar-13-2013$
*
* Comments:
*
*
*END************************************************************************/
#ifndef __main_h__
#define __main_h__

#define INIT_TASK_ID            1
#define WATCHDOG_TASK_ID	5
#define GPIO_TOGGLE_TASK_ID	6
#define EXT_SRAM_TASK_ID	7
#define SPIFLASH_TASK_ID	8
#define SHELL_TASK_ID           9
#define RS232_WRITE_TASK_ID  	10
#define RS232_READ_TASK_ID  	11
#define RS485_WRITE_TASK_ID  	12
#define RS485_READ_TASK_ID  	13
#define USB_HOST_TASK_ID        14

#define TM_TASKNUM  2
#define GPIO_TOGGLE_TASK_STATUS_BIT  1
#define SHELL_TASK_STATUS_BIT        2
extern LWSEM_STRUCT TM_Sem;
extern uint32_t TM_TaskStatus;


#endif


