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
* $FileName: gpio_toggle_task.c$
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

#define GPIO_TOGGLE_TASK_DELAY		500		//ms
#define GPIO_LED1_PORT			PORTB_BASE_PTR
#define GPIO_LED1_GPIO_PORT		PTB_BASE_PTR
#define GPIO_LED1_PIN			8
#define GPIO_LED2_PORT			PORTB_BASE_PTR
#define GPIO_LED2_GPIO_PORT		PTB_BASE_PTR
#define GPIO_LED2_PIN			9

void Gpio_Toggle_Task(uint32_t para)
{
    printf("GPIO toggle task  started.\n");
	
    PORT_PCR_REG(GPIO_LED1_PORT, GPIO_LED1_PIN) = PORT_PCR_MUX(0x01) | PORT_PCR_PE_MASK;
    GPIO_PDDR_REG(GPIO_LED1_GPIO_PORT) |= 1 << GPIO_LED1_PIN; 
    GPIO_PDOR_REG(GPIO_LED1_GPIO_PORT) |= 1 << GPIO_LED1_PIN; 
    PORT_PCR_REG(GPIO_LED2_PORT, GPIO_LED2_PIN) = PORT_PCR_MUX(0x01) | PORT_PCR_PE_MASK;
    GPIO_PDDR_REG(GPIO_LED2_GPIO_PORT) |= 1 << GPIO_LED2_PIN; 
    GPIO_PDOR_REG(GPIO_LED2_GPIO_PORT) |= 1 << GPIO_LED2_PIN; 
	
    for(;;)
    {
//        _lwsem_wait(&TM_Sem);
//        TM_TaskStatus |= GPIO_TOGGLE_TASK_STATUS_BIT;
//        _lwsem_post(&TM_Sem);
      	GPIO_PTOR_REG(GPIO_LED1_GPIO_PORT) |= 1 << GPIO_LED1_PIN;
        GPIO_PTOR_REG(GPIO_LED2_GPIO_PORT) |= 1 << GPIO_LED2_PIN;
	_time_delay(GPIO_TOGGLE_TASK_DELAY);
//        _lwsem_wait(&TM_Sem);
//        TM_TaskStatus |= GPIO_TOGGLE_TASK_STATUS_BIT;
//        _lwsem_post(&TM_Sem);
        GPIO_PTOR_REG(GPIO_LED1_GPIO_PORT) |= 1 << GPIO_LED1_PIN;
        GPIO_PTOR_REG(GPIO_LED2_GPIO_PORT) |= 1 << GPIO_LED2_PIN;
        _time_delay(GPIO_TOGGLE_TASK_DELAY);
    }
}
