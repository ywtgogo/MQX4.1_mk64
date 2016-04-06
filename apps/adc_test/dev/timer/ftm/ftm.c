#include "common.h"
#include "vectors.h"
#include "ftm.h"


void FTM_Init(FTM_MemMapPtr pxFtmNumber, FTM_PARAMETER* pxParameter)
{
    ASSERT(IS_FTM_ALL_PERIPH(pxFtmNumber));
    ASSERT(IS_FTM_PRESCALE_FACTOR(pxParameter->xFtmPF));
    ASSERT(IS_FTM_COUNTING_MODE(pxParameter->xFtmCountingMode));
    
    if(pxFtmNumber == FTM0)
    {
        SIM_SCGC6 |= SIM_SCGC6_FTM0_MASK;
    }
    else if(pxFtmNumber == FTM1)
    {
        SIM_SCGC6 |= SIM_SCGC6_FTM1_MASK;
    }
    else if(pxFtmNumber == FTM2)
    {
        SIM_SCGC3 |= SIM_SCGC3_FTM2_MASK;
    }
    else if(pxFtmNumber == FTM3)
    {
        SIM_SCGC3 |= SIM_SCGC3_FTM3_MASK;
    }
    
    FTM_MODE_REG(pxFtmNumber) |= FTM_MODE_WPDIS_MASK;
    FTM_SC_REG(pxFtmNumber) = FTM_SC_PS(pxParameter->xFtmPF) | ((uint32)(pxParameter->xFtmCountingMode) << FTM_SC_CPWMS_SHIFT);
}


void FTM_StructInit(FTM_PARAMETER* pxParameter)
{
    pxParameter->xFtmClock          = FTM_CLOCK_SYSTEM;
    pxParameter->xFtmCountingMode   = FTM_COUNTING_MODE_UP;
    pxParameter->xFtmPF             = FTM_PRESCALE_FACTOR_128;
}


void FTM_Cmd(FTM_MemMapPtr pxFtmNumber, FTM_PARAMETER* pxParameter)
{
    ASSERT(IS_FTM_ALL_PERIPH(pxFtmNumber));
    ASSERT(IS_FTM_CLOCK(pxParameter->xFtmClock));
    
    FTM_SC_REG(pxFtmNumber) &= ~FTM_SC_CLKS_MASK;
    FTM_SC_REG(pxFtmNumber) |= FTM_SC_CLKS(pxParameter->xFtmClock);
}


void FTM_PwmStructInit(FTM_PWM_PARAMETER* pxPwmParameter)
{
    pxPwmParameter->xFtmPwmLoadingUpdate    = FTM_PWM_LOADING_UPDATE_DISABLE;
    pxPwmParameter->xFtmPwmMode             = FTM_PWM_MODE_EDGE;
    pxPwmParameter->xFtmPwmTrue             = FTM_PWM_TRUE_HIGH;
}


void FTM_PwmInit(FTM_MemMapPtr pxFtmNumber, FTM_CHANNEL xFtmChannel, FTM_PWM_PARAMETER* pxPwmParameter)
{
    ASSERT(IS_FTM_ALL_PERIPH(pxFtmNumber));
    ASSERT(IS_FTM_CHANNEL(xFtmChannel));
    ASSERT(IS_FTM_PWM_LOADING_UPDATE(pxPwmParameter->xFtmPwmLoadingUpdate));
    ASSERT(IS_FTM_PWM_MODE(pxPwmParameter->xFtmPwmMode));
    ASSERT(IS_FTM_PWM_TRUE(pxPwmParameter->xFtmPwmTrue));
    
    FTM_PWMLOAD_REG(pxFtmNumber) &= ~FTM_PWMLOAD_LDOK_MASK;
    FTM_PWMLOAD_REG(pxFtmNumber) |= ((uint32)(pxPwmParameter->xFtmPwmTrue) << FTM_PWMLOAD_LDOK_SHIFT);
    
    FTM_COMBINE_REG(pxFtmNumber) &= ~((uint32)(1) << (FTM_COMBINE_COMBINE1_SHIFT)*(xFtmChannel/2)+FTM_COMBINE_DECAPEN0_SHIFT);
    
    FTM_COMBINE_REG(pxFtmNumber) &= ~((uint32)(1) << (FTM_COMBINE_COMBINE1_SHIFT)*(xFtmChannel/2));
    FTM_SC_REG(pxFtmNumber) &= ~((uint32)(1) << FTM_SC_CPWMS_SHIFT);
    FTM_CnSC_REG(pxFtmNumber, xFtmChannel) &= ~FTM_CnSC_MSB_MASK;
    FTM_CnSC_REG(pxFtmNumber, xFtmChannel) &= ~FTM_CnSC_ELSB_MASK;
    FTM_CnSC_REG(pxFtmNumber, xFtmChannel) &= ~FTM_CnSC_ELSA_MASK;
    
    if(pxPwmParameter->xFtmPwmMode == FTM_PWM_MODE_COMBINE)
    {
        FTM_COMBINE_REG(pxFtmNumber) |= ((uint32)(1) << (FTM_COMBINE_COMBINE1_SHIFT)*(xFtmChannel/2));
    }
    
    if(pxPwmParameter->xFtmPwmMode == FTM_PWM_MODE_CENTER)
    {
        FTM_SC_REG(pxFtmNumber) |= ((uint32)(1) << FTM_SC_CPWMS_SHIFT);
    }
    
    if(pxPwmParameter->xFtmPwmMode == FTM_PWM_MODE_EDGE)
    {
        FTM_CnSC_REG(pxFtmNumber, xFtmChannel) |= FTM_CnSC_MSB_MASK;
    }
    
    if(pxPwmParameter->xFtmPwmTrue == FTM_PWM_TRUE_HIGH)
    {
        FTM_CnSC_REG(pxFtmNumber, xFtmChannel) |= FTM_CnSC_ELSB_MASK;
        FTM_CnSC_REG(pxFtmNumber, xFtmChannel) &= ~FTM_CnSC_ELSA_MASK;
    }
    else
    {
        FTM_CnSC_REG(pxFtmNumber, xFtmChannel) |= FTM_CnSC_ELSA_MASK;
    }
}


void FTM_ITConfig(FTM_MemMapPtr pxFtmNumber, FUNCTIONALSTATE xNewState)
{
    ASSERT(IS_FTM_ALL_PERIPH(pxFtmNumber));
    ASSERT(IS_FUNCTIONAL_STATE(xNewState));
    
    if(xNewState)
    {
        if(pxFtmNumber == FTM0)
        {
            enable_irq(FTM0_irq_no);
        }
        else if(pxFtmNumber == FTM1)
        {
            enable_irq(FTM1_irq_no);
        }
        else if(pxFtmNumber == FTM2)
        {
            enable_irq(FTM2_irq_no);
        }
        else
        {
            enable_irq(FTM3_irq_no);
        }
        
        FTM_SC_REG(pxFtmNumber) |=  FTM_SC_TOIE_MASK;
    }
    else
    {
        if(pxFtmNumber == FTM0)
        {
            disable_irq(FTM0_irq_no);
        }
        else if(pxFtmNumber == FTM1)
        {
            disable_irq(FTM1_irq_no);
        }
        else if(pxFtmNumber == FTM2)
        {
            disable_irq(FTM2_irq_no);
        }
        else
        {
            disable_irq(FTM3_irq_no);
        }

        FTM_SC_REG(pxFtmNumber) &= ~FTM_SC_TOIE_MASK;
    }
}


void FTM_SetModulo(FTM_MemMapPtr pxFtmNumber, uint16 u16LoadNumber)
{
    ASSERT(IS_FTM_ALL_PERIPH(pxFtmNumber));
    ASSERT(u16LoadNumber);
    
    FTM_MOD_REG(pxFtmNumber) = u16LoadNumber;
}


void FTM_SetCounter(FTM_MemMapPtr pxFtmNumber, uint16 u16LoadNumber)
{
    ASSERT(IS_FTM_ALL_PERIPH(pxFtmNumber));
    
    FTM_CNT_REG(pxFtmNumber) = u16LoadNumber;
}

void FTM_SetCounterInitial(FTM_MemMapPtr pxFtmNumber, uint16 u16LoadNumber)
{
    ASSERT(IS_FTM_ALL_PERIPH(pxFtmNumber));
    
    FTM_CNTIN_REG(pxFtmNumber) = u16LoadNumber;
}


void FTM_SetChannelValue(FTM_MemMapPtr pxFtmNumber, FTM_CHANNEL xFtmChannel, uint16 u16LoadNumber)
{
    ASSERT(IS_FTM_ALL_PERIPH(pxFtmNumber));
    ASSERT(IS_FTM_CHANNEL(xFtmChannel));
    
    FTM_CnV_REG(pxFtmNumber, xFtmChannel) = u16LoadNumber;
}


FLAGSTATUS FTM_GetChannelFlagStatus(FTM_MemMapPtr pxFtmNumber, FTM_CHANNEL xFtmChannel)
{
    ASSERT(IS_FTM_ALL_PERIPH(pxFtmNumber));
    ASSERT(IS_FTM_CHANNEL(xFtmChannel));
    
    FLAGSTATUS xBitStatus = RESET;
    
    if(FTM_CnSC_REG(pxFtmNumber, xFtmChannel) & FTM_CnSC_CHF_MASK)
    {
        xBitStatus = SET;
    }
    
    return (xBitStatus);
}

