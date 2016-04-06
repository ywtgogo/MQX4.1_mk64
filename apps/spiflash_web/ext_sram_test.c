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

#define EXT_SRAM_TASK_DELAY		500		//ms

void Ext_Sram_Task(uint32_t para)
{
    static uint32_t failure_cnt = 0;
    uint32_t test_times = para;
    
    printf("ext sram task started. This task will wr/rd the external SRAM periodic.\n");
    
    while(test_times--){
        if(MQX_OK != _mem_verify(BSP_EXTERNAL_MRAM_RAM_BASE, 
                                 (pointer)((uint32_t)BSP_EXTERNAL_MRAM_RAM_BASE + BSP_EXTERNAL_MRAM_RAM_SIZE))){
                                     failure_cnt++;
                                     printf("The %dth ext SRAM verification failed.\n", failure_cnt);
                                 }
        
        _time_delay(EXT_SRAM_TASK_DELAY);
    }
    
    printf("The extern SRAM tested %d times with %d failure.\n", para, failure_cnt);
    
    _task_abort(MQX_NULL_TASK_ID);
}
