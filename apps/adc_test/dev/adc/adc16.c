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
* $FileName: adc16.c$
* $Version : 3.8.35.0$
* $Date    : Sep-21-2012$
*
* Comments:
*
*   This file contains ADC16 module driver.
*
*END************************************************************************/

#include "adc16.h"

static uint8_t su8Hardware[4]={0};
static uint8_t su8DataComplete[4]={0};


void ADC_Init(ADC_MemMapPtr pxAdcMoudelNumber, ADC_PARAMETER* pxAdcParameter)
{
    uint32_t u32SC1aParameter = 0x00;
    uint32_t u32CFG1Parameter = 0x00;
    uint32_t u32CFG2Parameter = 0x00;
    uint32_t u32SC2Parameter  = 0x00;
    uint32_t u32SC3Parameter  = 0x00;
    uint8_t  u8Subscript      = 0x00;

    if(pxAdcMoudelNumber == ADC0)
    {
        u8Subscript = 0;
        SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK;
    }
    else if(pxAdcMoudelNumber == ADC1)
    {
        u8Subscript = 1;
        SIM_SCGC3 |= SIM_SCGC3_ADC1_MASK;
    }

    //硬件模式的SC1n要轮换
    u32SC1aParameter |= ADC_CHANNEL_DISABLE;
    if(pxAdcParameter->xAdcDiff == ADC_DIFF_ENABLE)
    {
        u32SC1aParameter |= ADC_SC1_DIFF_MASK;
    }
    if(pxAdcParameter->xAdcLowpower == ADC_LOWPOWER_ENABLE)
    {
        u32CFG1Parameter |= ADC_CFG1_ADLPC_MASK;
    }
    u32CFG1Parameter |= ADC_CFG1_ADIV(pxAdcParameter->xAdcClockDivsion);
    if(pxAdcParameter->xAdcLongSample == ADC_LONG_SAMPLE_ENABLE)
    {
        u32CFG1Parameter |= ADC_CFG1_ADLSMP_MASK;
    }
    u32CFG1Parameter |= ADC_CFG1_MODE(pxAdcParameter->xAdcMode);
    u32CFG1Parameter |= ADC_CFG1_ADICLK(pxAdcParameter->xAdcClock);

    if(pxAdcParameter->xAdcAck == ADC_ADACK_ENABLE)
    {
        u32CFG2Parameter |= ADC_CFG2_ADACKEN_MASK;
    }
    if(pxAdcParameter->xAdcSpeed == ADC_SPEED_HIGH)
    {
        u32CFG2Parameter |= ADC_CFG2_ADHSC_MASK;
    }
    u32CFG2Parameter |= ADC_CFG2_ADLSTS(pxAdcParameter->xAdcLongSampleTime);

    u32SC2Parameter |= ADC_SC2_REFSEL(pxAdcParameter->xAdcRefsel);
    if(pxAdcParameter->xAdcRefsel == ADC_REFSEL_INTERNAL)
    {
        SIM_SCGC4 |= SIM_SCGC4_VREF_MASK;
        VREF_TRM_REG(VREF_BASE_PTR) = VREF_TRM_TRIM_MASK | VREF_TRM_CHOPEN_MASK;
        VREF_SC_REG(VREF_BASE_PTR) = VREF_SC_VREFEN_MASK | VREF_SC_REGEN_MASK | VREF_SC_ICOMPEN_MASK | VREF_SC_MODE_LV(0x2);
    }

    if(pxAdcParameter->xAdcAverageContinuous == ADC_AVERAGE_CONTINUOUS_X)
    {
        u32SC3Parameter |= ADC_SC3_ADCO_MASK;
    }
    if(pxAdcParameter->xAdcAverage == ADC_AVERAGE_ENABLE)
    {
        u32SC3Parameter |= ADC_SC3_AVGE_MASK;
    }
    u32SC3Parameter |= ADC_SC3_AVGS(pxAdcParameter->xAdcAverageSelect);

    ADC_SC1_REG(pxAdcMoudelNumber,0) = u32SC1aParameter;
    if(pxAdcParameter->xAdcTrigger == ADC_TRIGGER_HARDWARE)
    {
        su8Hardware[u8Subscript] = 1;
        ADC_SC1_REG(pxAdcMoudelNumber,1) = u32SC1aParameter;
    }
    else
    {
        su8DataComplete[u8Subscript] = 0;
        su8Hardware[u8Subscript] = 0;
    }
    ADC_CFG1_REG(pxAdcMoudelNumber) = u32CFG1Parameter;
    ADC_CFG2_REG(pxAdcMoudelNumber) = u32CFG2Parameter;
    ADC_SC2_REG(pxAdcMoudelNumber) = u32SC2Parameter;
    ADC_SC3_REG(pxAdcMoudelNumber) = u32SC3Parameter;
}

uint8_t ADC_Cal(ADC_MemMapPtr pxAdcMoudelNumber)
{
	uint16_t cal_var = 0x00;

    if(pxAdcMoudelNumber == ADC0)
    {
        SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK;
    }
    else if(pxAdcMoudelNumber == ADC1)
    {
        SIM_SCGC3 |= SIM_SCGC3_ADC1_MASK;
    }

    ADC_SC3_REG(pxAdcMoudelNumber) |= ADC_SC3_CAL_MASK; //校准
    while(ADC_SC3_REG(pxAdcMoudelNumber) & ADC_SC3_CAL_MASK);

	if(ADC_SC3_REG(pxAdcMoudelNumber)&ADC_SC3_CALF_MASK)
		return 1;

	// Calculate plus-side calibration
	cal_var = 0x00;
	  
	cal_var =  ADC_CLP0_REG(pxAdcMoudelNumber); 
	cal_var += ADC_CLP1_REG(pxAdcMoudelNumber);
	cal_var += ADC_CLP2_REG(pxAdcMoudelNumber);
	cal_var += ADC_CLP3_REG(pxAdcMoudelNumber);
	cal_var += ADC_CLP4_REG(pxAdcMoudelNumber);
	cal_var += ADC_CLPS_REG(pxAdcMoudelNumber);

	cal_var = cal_var/2;
	cal_var |= 0x8000; // Set MSB

	ADC_PG_REG(pxAdcMoudelNumber) = ADC_PG_PG(cal_var);
	 

	// Calculate minus-side calibration
	cal_var = 0x00;

	cal_var =  ADC_CLM0_REG(pxAdcMoudelNumber); 
	cal_var += ADC_CLM1_REG(pxAdcMoudelNumber);
	cal_var += ADC_CLM2_REG(pxAdcMoudelNumber);
	cal_var += ADC_CLM3_REG(pxAdcMoudelNumber);
	cal_var += ADC_CLM4_REG(pxAdcMoudelNumber);
	cal_var += ADC_CLMS_REG(pxAdcMoudelNumber);

	cal_var = cal_var/2;

	cal_var |= 0x8000; // Set MSB

	ADC_MG_REG(pxAdcMoudelNumber) = ADC_MG_MG(cal_var); 

	/* Read result register to clear COCO flag */
	ADC_R_REG(pxAdcMoudelNumber, 0);

	return 0;
	//ADC_SC3_REG(pxAdcMoudelNumber) &= ~ADC_SC3_CAL_MASK ; /* Clear CAL bit */
}

void ADC_StructInit(ADC_PARAMETER* pxAdcParameter)
{
    pxAdcParameter->xAdcAck = ADC_ADACK_DISABLE;
    pxAdcParameter->xAdcAverage = ADC_AVERAGE_DISABLE;
    pxAdcParameter->xAdcAverageContinuous = ADC_AVERAGE_CONTINUOUS_1;
    pxAdcParameter->xAdcAverageSelect = ADC_AVERAGE_SELECT_4;
    pxAdcParameter->xAdcClock = ADC_CLOCK_BUSCLK;
    pxAdcParameter->xAdcClockDivsion = ADC_CLOCK_DIVSION_4;
    pxAdcParameter->xAdcDiff = ADC_DIFF_DISABLE;
    pxAdcParameter->xAdcLongSample = ADC_LONG_SAMPLE_DISABLE;
    pxAdcParameter->xAdcLongSampleTime = ADC_LONG_SAMPLE_TIME_20ADCK;
    pxAdcParameter->xAdcLowpower = ADC_LOWPOWER_DISABLE;
    pxAdcParameter->xAdcMode = ADC_MODE_8_BIT;
    pxAdcParameter->xAdcRefsel = ADC_REFSEL_EXTERNAL;
    pxAdcParameter->xAdcSpeed = ADC_SPEED_NORMAL;
    pxAdcParameter->xAdcTrigger = ADC_TRIGGER_SOFTWARE;
	pxAdcParameter->old_isr = NULL;
	pxAdcParameter->old_isr_data = NULL;
}

void ADC_DMACmd(ADC_MemMapPtr pxAdcMoudelNumber, FUNCTIONALSTATE xNewState)
{
    if(xNewState != DISABLE)
    {
        ADC_SC2_REG(pxAdcMoudelNumber) |= ADC_SC2_DMAEN_MASK;
    }
    else
    {
        ADC_SC2_REG(pxAdcMoudelNumber) &= ~ADC_SC2_DMAEN_MASK;
    }
}

void ADC_ITConfig(ADC_MemMapPtr pxAdcMoudelNumber, ADC_PARAMETER* pxAdcParameter, FUNCTIONALSTATE xNewState)
{
    uint8_t u8Subscript = 0;
    
    if(pxAdcMoudelNumber == ADC0)
    {
        u8Subscript = 0;
    }
    else if(pxAdcMoudelNumber == ADC1)
    {
        u8Subscript = 1;
    }

    if(xNewState != DISABLE)
    {
        if(pxAdcMoudelNumber == ADC0)
        {
            pxAdcParameter->old_isr_data = _int_get_isr_data(INT_ADC0);
			pxAdcParameter->old_isr = _int_install_isr(INT_ADC0, adc0_isr, NULL);
			_bsp_int_init(INT_ADC0, BSP_ADC0_VECTOR_PRIORITY, 0, TRUE);
        }
        else if(pxAdcMoudelNumber == ADC1)
        {
            pxAdcParameter->old_isr_data = _int_get_isr_data(INT_ADC1);
			pxAdcParameter->old_isr = _int_install_isr(INT_ADC1, adc1_isr, NULL);
			_bsp_int_init(INT_ADC1, BSP_ADC1_VECTOR_PRIORITY, 0, TRUE);
        }
        
        ADC_SC1_REG(pxAdcMoudelNumber,0) |= ADC_SC1_AIEN_MASK;
        if(su8Hardware[u8Subscript] == 1)
        {
            ADC_SC1_REG(pxAdcMoudelNumber,1) |= ADC_SC1_AIEN_MASK;
        }
    }
    else
    {
        if(pxAdcMoudelNumber == ADC0)
        {
            _int_install_isr(INT_ADC0, pxAdcParameter->old_isr, pxAdcParameter->old_isr_data);
        }
        else if(pxAdcMoudelNumber == ADC1)
        {
            _int_install_isr(INT_ADC1, pxAdcParameter->old_isr, pxAdcParameter->old_isr_data);
        }
        
        ADC_SC1_REG(pxAdcMoudelNumber,0) &= ~ADC_SC1_AIEN_MASK;
        if(su8Hardware[u8Subscript] == 1)
        {
            ADC_SC1_REG(pxAdcMoudelNumber,1) &= ~ADC_SC1_AIEN_MASK;
        }
    }
}

void ADC_MuxCmd(ADC_MemMapPtr pxAdcMoudelNumber, ADC_MUXSEL xAdcMux)
{
    if(xAdcMux == ADC_MUXSEL_ADxxa)
    {
        ADC_CFG2_REG(pxAdcMoudelNumber) &= ~ADC_CFG2_MUXSEL_MASK;
    }
    else if(xAdcMux == ADC_MUXSEL_ADxxb)
    {
        ADC_CFG2_REG(pxAdcMoudelNumber) |= ADC_CFG2_MUXSEL_MASK;
    }
}

void ADC_Cmd(ADC_MemMapPtr pxAdcMoudelNumber, ADC_CHANNEL xChannel)
{
    uint8_t u8Subscript = 0;
    uint32_t u32SC1aParameter = 0x00;

    if(pxAdcMoudelNumber == ADC0)
    {
        u8Subscript = 0;
    }
    else if(pxAdcMoudelNumber == ADC1)
    {
        u8Subscript = 1;
    }

    u32SC1aParameter = ADC_SC1_REG(pxAdcMoudelNumber,0) & 0xffffffe0;
    u32SC1aParameter |= ADC_SC1_ADCH(xChannel);
    ADC_SC1_REG(pxAdcMoudelNumber,0) = u32SC1aParameter;
    if(su8Hardware[u8Subscript] == 1)
    {
        ADC_SC1_REG(pxAdcMoudelNumber,1) = u32SC1aParameter;
    }
}

#if 0
void ADC_PGA_Init(ADC_MemMapPtr pxAdcMoudelNumber, ADC_PGA_PARAMETER* pxAdcPgaParameter)
{
    uint32_t u32PGAParameter = 0x00;
    ASSERT(IS_ADC_ALL_PERIPH(pxAdcMoudelNumber));
    ASSERT(IS_ADC_PGA_GAIN(pxAdcPgaParameter->xAdcPgaGain));
    ASSERT(IS_ADC_PGA_POWER(pxAdcPgaParameter->xAdcPgaPower));

    u32PGAParameter |= ADC_PGA_PGAG(pxAdcPgaParameter->xAdcPgaGain);
    if(pxAdcPgaParameter->xAdcPgaPower == ADC_PGA_POWER_NORMAL)
    {
        u32PGAParameter |= ADC_PGA_PGLPB_MASK;
    }
    ADC_PGA_REG(pxAdcMoudelNumber) = u32PGAParameter;
}

void ADC_PGA_StructInit(ADC_PGA_PARAMETER* pxAdcPgaParameter)
{
    pxAdcPgaParameter->xAdcPgaPower = ADC_PGA_POWER_LOW;
    pxAdcPgaParameter->xAdcPgaGain = ADC_PGA_GAIN_1;
}

void ADC_PGA_Cmd(ADC_MemMapPtr pxAdcMoudelNumber, FUNCTIONALSTATE xNewState)
{
    ASSERT(IS_ADC_ALL_PERIPH(pxAdcMoudelNumber));
    ASSERT(IS_FUNCTIONAL_STATE(xNewState));

    if(xNewState != DISABLE)
    {
        ADC_PGA_REG(pxAdcMoudelNumber) |= ADC_PGA_PGAEN_MASK;
    }
    else
    {
        ADC_PGA_REG(pxAdcMoudelNumber) &= ~ADC_PGA_PGAEN_MASK;
    }
}
#endif

uint16_t ADC_GetDataResult(ADC_MemMapPtr pxAdcMoudelNumber, ADC_PARAMETER* pxAdcParameter)
{
    uint16_t u16Value;
    uint8_t u8Subscript = 0;

    if(pxAdcMoudelNumber == ADC0)
    {
        u8Subscript = 0;
    }
    else if(pxAdcMoudelNumber == ADC1)
    {
        u8Subscript = 1;
    }


    if(su8Hardware[u8Subscript] == 1)
    {
        u16Value = ADC_R_REG(pxAdcMoudelNumber, su8DataComplete[u8Subscript]);
    }
    else
    {
        u16Value = ADC_R_REG(pxAdcMoudelNumber, 0);
    }
    if(pxAdcParameter->xAdcDiff == ADC_DIFF_ENABLE)
    {
        switch(pxAdcParameter->xAdcMode)
        {
        case ADC_MODE_8_BIT:
            u16Value &= 0x80FF;
            break;
        case ADC_MODE_10_BIT:
            u16Value &= 0x83FF;
            break;
        case ADC_MODE_12_BIT:
            u16Value &= 0x8FFF;
            break;
        case ADC_MODE_16_BIT:
            break;
        default:
            break;
        }
    }

    return u16Value;
}

FLAGSTATUS ADC_GetFlagConversionProgress(ADC_MemMapPtr pxAdcMoudelNumber)
{
    FLAGSTATUS xBitStatus;
    
    if(ADC_SC2_REG(pxAdcMoudelNumber) & ADC_SC2_ADACT_MASK)
    {
        xBitStatus = SET;
    }
    else
    {
        xBitStatus = RESET;
    }
    
    return xBitStatus;
}

FLAGSTATUS ADC_GetFlagStatus(ADC_MemMapPtr pxAdcMoudelNumber)
{
    FLAGSTATUS xBitStatus;
    uint8_t u8Subscript = 0;

    if(pxAdcMoudelNumber == ADC0)
    {
        u8Subscript = 0;
    }
    else if(pxAdcMoudelNumber == ADC1)
    {
        u8Subscript = 1;
    }


    if(su8Hardware[u8Subscript] == 1)
    {
        if(ADC_SC1_REG(pxAdcMoudelNumber,0) & ADC_SC1_COCO_MASK)
        {
            su8DataComplete[u8Subscript] = 0;
            xBitStatus = SET;
        }
        else if(ADC_SC1_REG(pxAdcMoudelNumber,1) & ADC_SC1_COCO_MASK)
        {
            su8DataComplete[u8Subscript] = 1;
            xBitStatus = SET;
        }
        else
        {
            xBitStatus = RESET;
        }
    }
    else
    {
        if(ADC_SC1_REG(pxAdcMoudelNumber,0) & ADC_SC1_COCO_MASK)
        {
            xBitStatus = SET;
        }
        else
        {
            xBitStatus = RESET;
        }
    }

    return xBitStatus;
}

void adc0_isr(pointer isr)
{

}

void adc1_isr(pointer isr)
{

}
