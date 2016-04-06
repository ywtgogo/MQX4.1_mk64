/**HEADER********************************************************************
* 
* Copyright (c) 2008-2009 Freescale Semiconductor;
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
* $FileName: adc_demo.c$
* $Version : 3.8.35.0$
* $Date    : Sep-21-2012$
*
* Comments:
*
*   This file contains the source for the ADC example program.
*   Two channels are running, one is running in loop, the second one
*   only once.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "adc16.h"
#include "dma.h"
#include "app_dma.h"

#define     ADC_VALUE_MASK      ((uint32_t)(0xff))
#define     ADC_SAMPLE_FREQ     ((uint32_t)(256))
#define     ADC0_TABLE          {ADC_CHANNEL_DP3_DIFF}
#define     ADC0_TABLE_LEN      (1)

//define if testing DIFF channel
#define TEST_DIFF_CH 

typedef union 
{
    struct 
    {
        uint16_t  u16_channel[ADC0_TABLE_LEN];
    }ux_p_each_value[ADC_VALUE_MASK + 1];
    uint16_t  u16_p_all_value[ADC0_TABLE_LEN*(ADC_VALUE_MASK + 1)];
} ADC0_VALUE;

uint32_t          u32_p_adc0_channel_table[ADC0_TABLE_LEN] = ADC0_TABLE;
ADC0_VALUE      ux_adc0_value;

uint32_t cnt = 0;

/* Task IDs */
#define MAIN_TASK 5

/* Function prototypes */
extern void main_task(uint32_t);

const TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
    /* Task Index,  Function,  Stack,  Priority,    Name,       Attributes,             Param,  Time Slice */
    {MAIN_TASK,     main_task, 2048,      8,        "Main",     MQX_AUTO_START_TASK,    0,      0           },
    {0}
};

/*TASK*-----------------------------------------------------
** 
** Task Name    : main_task
** Comments     :
**
*END*-----------------------------------------------------*/
void main_task
   (
      uint32_t initial_data
   )
{ 
#ifdef TEST_DIFF_CH
    short buf[ADC0_TABLE_LEN*(ADC_VALUE_MASK + 1)];
#endif
    
    ADC_PARAMETER   ux_Adc_Parameter;
    
    ADC_StructInit(&ux_Adc_Parameter);
	/* Before calibration, set hardware average to maximum and set ADC clock to be less than or equal to 4MHz,
	using external reference VREFH = VDDA */
    //ux_Adc_Parameter.xAdcAck               = ADC_ADACK_ENABLE;
    ux_Adc_Parameter.xAdcClock              = ADC_CLOCK_BUSCLK;
    ux_Adc_Parameter.xAdcClockDivsion       = ADC_CLOCK_DIVSION_8;
    ux_Adc_Parameter.xAdcMode               = ADC_MODE_16_BIT;
    ux_Adc_Parameter.xAdcLongSample         = ADC_LONG_SAMPLE_ENABLE;
    ux_Adc_Parameter.xAdcLongSampleTime     = ADC_LONG_SAMPLE_TIME_6ADCK;
    /* Select internal reference */
    ux_Adc_Parameter.xAdcRefsel             = ADC_REFSEL_INTERNAL;
	/* Set ADC speed and low power mode to select ADACK value */
    ux_Adc_Parameter.xAdcSpeed              = ADC_SPEED_HIGH;

	/* hardware average enable */
    ux_Adc_Parameter.xAdcAverage            = ADC_AVERAGE_ENABLE;
    ux_Adc_Parameter.xAdcAverageContinuous  = ADC_AVERAGE_CONTINUOUS_1;
    ux_Adc_Parameter.xAdcAverageSelect      = ADC_AVERAGE_SELECT_32;

    ADC_Init(ADC0, &ux_Adc_Parameter);
	ADC_Cal(ADC0);

	/* hardware average enable */
    //ux_Adc_Parameter.xAdcAverage            = ADC_AVERAGE_DISABLE;
    ux_Adc_Parameter.xAdcAverageSelect      = ADC_AVERAGE_SELECT_4;
	ADC_Init(ADC0, &ux_Adc_Parameter);
	
    ADC_DMACmd(ADC0, ENABLE);
        
    DMA_PARAMETER   ux_Dma_Parameter;

    DMA_StructInit(&ux_Dma_Parameter);
    ux_Dma_Parameter.xDmaSrcChannel = DMAMUX_SRC_CHANNEL_PDB;
    ux_Dma_Parameter.u32SourceAddress = (uint32_t)&(u32_p_adc0_channel_table[0]);
    ux_Dma_Parameter.u32DestinationAddress = (uint32_t)&(ADC0_SC1A);
    ux_Dma_Parameter.u16SourceNextValueOffset = sizeof(uint32_t);
    ux_Dma_Parameter.u16DestinationNextValueOffset = 0;
    ux_Dma_Parameter.u8SourceAddressModulo = 0;
    ux_Dma_Parameter.u8DestinationAddressModulo = 0;
    ux_Dma_Parameter.xSourceDataSize = DMA_TRANSFER_SIZE_32_BIT;
    ux_Dma_Parameter.xDestinationDataSize = DMA_TRANSFER_SIZE_32_BIT;
    ux_Dma_Parameter.u32ByteTransferCount = sizeof(uint32_t);
    ux_Dma_Parameter.u32LastSourceAddressAdjustment = -(ADC0_TABLE_LEN * ux_Dma_Parameter.u16SourceNextValueOffset);
    ux_Dma_Parameter.u32LastDestinationAddressAdjustment = 0;
    ux_Dma_Parameter.u16BufferSize = ADC0_TABLE_LEN;
    DMA_Init(DMA_CHANNEL_0, DMAMUX, &ux_Dma_Parameter);
    DMA_Cmd(DMA_CHANNEL_0, DMAMUX, ENABLE);
    
    ux_Dma_Parameter.xDmaSrcChannel = DMAMUX_SRC_CHANNEL_ADC0;
    ux_Dma_Parameter.u32SourceAddress = (uint32_t)&(ADC0_RA);
    ux_Dma_Parameter.u32DestinationAddress = (uint32_t)&(ux_adc0_value.u16_p_all_value[0]);
    ux_Dma_Parameter.u16SourceNextValueOffset = 0;
    ux_Dma_Parameter.u16DestinationNextValueOffset = sizeof(uint16_t);
    ux_Dma_Parameter.u8SourceAddressModulo = 0;
    ux_Dma_Parameter.u8DestinationAddressModulo = 0;
    ux_Dma_Parameter.xSourceDataSize = DMA_TRANSFER_SIZE_16_BIT;
    ux_Dma_Parameter.xDestinationDataSize = DMA_TRANSFER_SIZE_16_BIT;
    ux_Dma_Parameter.u32ByteTransferCount = sizeof(uint16_t);
    ux_Dma_Parameter.u32LastSourceAddressAdjustment = 0;
    ux_Dma_Parameter.u32LastDestinationAddressAdjustment = -((ADC0_TABLE_LEN*(ADC_VALUE_MASK + 1)) * ux_Dma_Parameter.u16DestinationNextValueOffset);
    ux_Dma_Parameter.u16BufferSize = ADC0_TABLE_LEN*(ADC_VALUE_MASK + 1);
    DMA_Init(DMA_CHANNEL_1, DMAMUX, &ux_Dma_Parameter);
    DMA_Cmd(DMA_CHANNEL_1, DMAMUX, ENABLE);
    
    DMA_EnableRequest(DMAMUX, DMA_CHANNEL_0);
    DMA_EnableRequest(DMAMUX, DMA_CHANNEL_1);
    
    
    SIM_SCGC6 |= SIM_SCGC6_PDB_MASK;

    PDB_SC_REG(PDB0_BASE_PTR) |= PDB_SC_PDBEN_MASK ;
    PDB_SC_REG(PDB0_BASE_PTR) |= PDB_SC_TRGSEL(0xf) | PDB_SC_CONT_MASK;
    PDB_MOD_REG(PDB0_BASE_PTR) = BSP_BUS_CLOCK / 50 / ADC_SAMPLE_FREQ;
    PDB_IDLY_REG(PDB0_BASE_PTR) = 0;
    PDB_SC_REG(PDB0_BASE_PTR) |= PDB_SC_LDOK_MASK;
    PDB_SC_REG(PDB0_BASE_PTR) |= PDB_SC_DMAEN_MASK;
    PDB_SC_REG(PDB0_BASE_PTR) |= PDB_SC_SWTRIG_MASK;
    
    
    while(1)
    {
        for(long int i = 0; i < 8000000; i ++);
        ADC_DMACmd(ADC0, DISABLE);
        
        for(cnt = 0; cnt < ADC0_TABLE_LEN*(ADC_VALUE_MASK + 1); cnt++)
        {
#ifdef TEST_DIFF_CH
            //when ADC operate in differential mode, converted 16bit value is in 2's complement value
            if((ux_adc0_value.u16_p_all_value[cnt])&0x8000)
                buf[cnt] = -(~(ux_adc0_value.u16_p_all_value[cnt])+1);
            else 
                buf[cnt] = ux_adc0_value.u16_p_all_value[cnt];
            printf("%d\n", buf[cnt]);
#else
            printf("%d\n", ux_adc0_value.u16_p_all_value[cnt]);
#endif
        }
        ADC_DMACmd(ADC0, ENABLE);
        
        _time_delay(100);
    }    
    //_task_block();
}  

/* EOF */
