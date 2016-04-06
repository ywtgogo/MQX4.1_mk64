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
* $FileName: ext_sram_test.c$
* $Version : 3.8.19.0$
* $Date    : Mar-13-2013$
*
* Comments:
*
*
*END************************************************************************/
#include <mqx.h>
#include <bsp.h> 
#include <fio.h>
#include "main.h"

LWSEM_STRUCT TM_Sem;
uint32_t TM_TaskStatus;

void Init_Task(uint32_t initial_data);
extern void spiflash_nftl_install(void);
extern void Watchdog_Task(uint32_t);
extern void Gpio_Toggle_Task(uint32_t);
extern void Shell_Task(uint32_t);
extern void Ext_Sram_Task(uint32_t);
extern void Shell_Task(uint32_t);
extern void RS485_Write_Task( uint32_t);
extern void RS485_Read_Task(uint32_t);
extern void _bsp_ext_watchdog_kick(void);
extern void USB_Task(uint32_t param);
unsigned char a[200]={0x55,0x22};
const TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{ 
    /* Task Index,   	    Function,   	Stack,  Priority, Name,     Attributes,          Param, Time Slice */
    {INIT_TASK_ID,          Init_Task,          2000,   8,    "inittask",    MQX_AUTO_START_TASK, 0,     0 },
    {WATCHDOG_TASK_ID,      Watchdog_Task, 	1500,   9,    "watchdog",    0,                   0,     0 },
    {GPIO_TOGGLE_TASK_ID,   Gpio_Toggle_Task, 	1500,   10,   "gpiotoggle",  0,                   0,     0 },
    {EXT_SRAM_TASK_ID,      Ext_Sram_Task, 	1500,   11,   "extsram",     0,                   0,     0 },
    {SHELL_TASK_ID, 	    Shell_Task, 	1500,   15,   "shell",       0,                   0,     0 },
    {RS485_WRITE_TASK_ID,   RS485_Write_Task,   1500,   12,   "rs485write",  0,                   0,     0 },
    {RS485_READ_TASK_ID,    RS485_Read_Task, 	1500,   13,   "re484read",   0,                   0,     0 },
    {USB_HOST_TASK_ID,      USB_Task, 		1500,   14,   "usb host",    0,  	          0,     0 },
    { 0 }
};

void Init_Task(uint32_t initial_data)
{	
    _task_create(0,WATCHDOG_TASK_ID,0);
    _task_create(0,SHELL_TASK_ID, 0);
    _task_create(0,GPIO_TOGGLE_TASK_ID, 0);
    _task_create(0,USB_HOST_TASK_ID, 0);
    spiflash_nftl_install();
    _task_block();
    _mqx_exit(0);
}

void TaskManageDet(void)
{
    uint8_t i;
    
    _lwsem_wait(&TM_Sem);
    for(i=1; i<=TM_TASKNUM; i++)
    {
        if((TM_TaskStatus & i) != i)
        {
            while(1);
        }
        TM_TaskStatus &= ~i;
    }
    _lwsem_post(&TM_Sem);
}

void Watchdog_Task(uint32_t initial_data)
{
    while(1)
    {
        //TaskManageDet();
        _bsp_ext_watchdog_kick();
	_time_delay(900);
    }
}

