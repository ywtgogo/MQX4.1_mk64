#include "common.h"
#include "pit.h"

void PIT_Init(PIT_CHANNEL xPitNumber)
{
    ASSERT(IS_PIT_ALL_CHANNEL(xPitNumber));
    
    SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;
    PIT_MCR &= ~PIT_MCR_MDIS_MASK;
    PIT_TCTRL(xPitNumber) = 0;
    PIT_TFLG(xPitNumber) |= PIT_TFLG_TIF_MASK;
}

void PIT_Cmd(PIT_CHANNEL xPitNumber, FUNCTIONALSTATE xNewState)
{
    ASSERT(IS_PIT_ALL_CHANNEL(xPitNumber));
    ASSERT(IS_FUNCTIONAL_STATE(xNewState));
    
    if(xNewState != DISABLE)
    {
        PIT_TCTRL(xPitNumber) |= PIT_TCTRL_TEN_MASK;
    }
    else
    {
        PIT_TCTRL(xPitNumber) &= ~PIT_TCTRL_TEN_MASK;
    }
}

void PIT_ITConfig(PIT_CHANNEL xPitNumber, FUNCTIONALSTATE xNewState)
{
    ASSERT(IS_PIT_ALL_CHANNEL(xPitNumber));
    ASSERT(IS_FUNCTIONAL_STATE(xNewState));
    
    if(xNewState != DISABLE)
    {
        enable_irq(68 + xPitNumber);
        PIT_TCTRL(xPitNumber) |= PIT_TCTRL_TIE_MASK;
    }
    else
    {
        disable_irq(68 + xPitNumber);
        PIT_TCTRL(xPitNumber) &= ~PIT_TCTRL_TIE_MASK;
    }
}

void PIT_SetAutoreload(PIT_CHANNEL xPitNumber, uint32 u32LoadNumber)
{
    ASSERT(IS_PIT_ALL_CHANNEL(xPitNumber));
    
    PIT_LDVAL(xPitNumber) = u32LoadNumber;
}

void PIT_SetCounter(PIT_CHANNEL xPitNumber, uint32 u32CounterNumber)
{
    ASSERT(IS_PIT_ALL_CHANNEL(xPitNumber));
    
    PIT_CVAL(xPitNumber) = u32CounterNumber;
}

uint32 PIT_GetCounter(PIT_CHANNEL xPitNumber)
{
    ASSERT(IS_PIT_ALL_CHANNEL(xPitNumber));
    
    return (PIT_CVAL(xPitNumber));
}

FLAGSTATUS PIT_GetFlagStatus(PIT_CHANNEL xPitNumber)
{
    FLAGSTATUS xBitStatus = RESET;
    
    ASSERT(IS_PIT_ALL_CHANNEL(xPitNumber));
    
    if(PIT_TFLG(xPitNumber) & PIT_TFLG_TIF_MASK)
    {
        xBitStatus = SET;
    }
    else
    {
        xBitStatus = RESET;
    }
    
    return (xBitStatus);
}

void PIT_ClearFlag(PIT_CHANNEL xPitNumber)
{
    ASSERT(IS_PIT_ALL_CHANNEL(xPitNumber));
    
    PIT_TFLG(xPitNumber) |= PIT_TFLG_TIF_MASK;
    if(PIT_TFLG(xPitNumber) & PIT_TFLG_TIF_MASK)
    {
        PIT_TFLG(xPitNumber) |= PIT_TFLG_TIF_MASK;
    }
}
