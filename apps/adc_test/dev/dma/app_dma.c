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
* $FileName: dma.c$
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
#include "app_dma.h"
#include "dma.h"

void DMA_Init(DMA_CHANNEL xDmaChannel, DMAMUX_MemMapPtr xDmamux, DMA_PARAMETER* pxDmaParameter)
{
    if(xDmamux == DMAMUX)
    {
        SIM_SCGC6|=SIM_SCGC6_DMAMUX_MASK;
    }

    SIM_SCGC7|=SIM_SCGC7_DMA_MASK;

    DMAMUX_CHCFG_REG(xDmamux, xDmaChannel) = DMAMUX_CHCFG_SOURCE(pxDmaParameter->xDmaSrcChannel);

    DMA_SADDR_REG(DMA_BASE_PTR, xDmaChannel) = pxDmaParameter->u32SourceAddress;
    DMA_DADDR_REG(DMA_BASE_PTR, xDmaChannel) = pxDmaParameter->u32DestinationAddress;
    DMA_SOFF_REG(DMA_BASE_PTR, xDmaChannel)  = pxDmaParameter->u16SourceNextValueOffset;
    DMA_DOFF_REG(DMA_BASE_PTR, xDmaChannel)  = pxDmaParameter->u16DestinationNextValueOffset;
    DMA_ATTR_REG(DMA_BASE_PTR, xDmaChannel)  = DMA_ATTR_SMOD(pxDmaParameter->u8SourceAddressModulo) | DMA_ATTR_SSIZE(pxDmaParameter->xSourceDataSize) |
                                               DMA_ATTR_DMOD(pxDmaParameter->u8DestinationAddressModulo) | DMA_ATTR_DSIZE(pxDmaParameter->xDestinationDataSize);
    DMA_NBYTES_MLNO_REG(DMA_BASE_PTR, xDmaChannel) = pxDmaParameter->u32ByteTransferCount;
    DMA_SLAST_REG(DMA_BASE_PTR, xDmaChannel) = pxDmaParameter->u32LastSourceAddressAdjustment;
    DMA_DLAST_SGA_REG(DMA_BASE_PTR, xDmaChannel) = pxDmaParameter->u32LastDestinationAddressAdjustment;
    DMA_CITER_ELINKNO_REG(DMA_BASE_PTR, xDmaChannel) =  DMA_CITER_ELINKNO_CITER(pxDmaParameter->u16BufferSize);
    DMA_BITER_ELINKNO_REG(DMA_BASE_PTR, xDmaChannel) = DMA_BITER_ELINKNO_BITER(pxDmaParameter->u16BufferSize);
    DMA_CR_REG(DMA_BASE_PTR) |= DMA_CR_ERCA_MASK;
    DMA_CSR_REG(DMA_BASE_PTR, xDmaChannel) = 0;
}

void DMA_StructInit(DMA_PARAMETER* pxDmaParameter)
{
    pxDmaParameter->xDmaSrcChannel = DMAMUX_SRC_CHANNEL_UART0_RX;
    pxDmaParameter->xDestinationDataSize = DMA_TRANSFER_SIZE_8_BIT;
    pxDmaParameter->xSourceDataSize = DMA_TRANSFER_SIZE_8_BIT;
    pxDmaParameter->u16BufferSize = 0;
    pxDmaParameter->u16DestinationNextValueOffset = 0;
    pxDmaParameter->u16SourceNextValueOffset = 0;
    pxDmaParameter->u32ByteTransferCount = 0;
    pxDmaParameter->u32DestinationAddress = 0;
    pxDmaParameter->u32LastDestinationAddressAdjustment = 0;
    pxDmaParameter->u32LastSourceAddressAdjustment = 0;
    pxDmaParameter->u32SourceAddress = 0;
    pxDmaParameter->u8DestinationAddressModulo = 0;
    pxDmaParameter->u8SourceAddressModulo = 0;
}

void DMA_ITConfig(DMA_CHANNEL xDmaChannel, DMAMUX_MemMapPtr xDmamux, FUNCTIONALSTATE xNewState)
{

    if(xNewState != DISABLE)
    {
        //enable_irq(xDmaChannel);
        DMA_CSR_REG(DMA_BASE_PTR, xDmaChannel) |= DMA_CSR_INTMAJOR_MASK;
    }
    else
    {
        //disable_irq(xDmaChannel);
        DMA_CSR_REG(DMA_BASE_PTR, xDmaChannel) &= ~DMA_CSR_INTMAJOR_MASK;
    }
}

void DMA_Cmd(DMA_CHANNEL xDmaChannel, DMAMUX_MemMapPtr xDmamux, FUNCTIONALSTATE xNewState)
{

    if(xNewState != DISABLE)
    {
        //DMA_CSR_REG(DMA_BASE_PTR, xDmaChannel) |= DMA_CSR_DREQ_MASK;
        DMAMUX_CHCFG_REG(xDmamux, xDmaChannel) |= DMAMUX_CHCFG_ENBL_MASK;
    }
    else
    {
        //DMA_CSR_REG(DMA_BASE_PTR, xDmaChannel) &= ~DMA_CSR_DREQ_MASK;
        DMAMUX_CHCFG_REG(xDmamux, xDmaChannel) &= ~DMAMUX_CHCFG_ENBL_MASK;
    }
}

void DMA_EnableRequest(DMAMUX_MemMapPtr xDmamux, DMA_CHANNEL xDmaChannel)
{
    uint32_t u32Mask = 1;
    uint8_t i;

    for(i = 0; i < xDmaChannel; i ++)
    {
        u32Mask <<= 1;
    }

    DMA_ERQ |= u32Mask;
}

FLAGSTATUS DMA_GetActiveFlagStatus(DMAMUX_MemMapPtr xDmamux,DMA_CHANNEL xDmaChannel)
{
    FLAGSTATUS xBitStatus = RESET;
    
    if(DMA_CSR_REG(DMA_BASE_PTR, xDmaChannel) & DMA_CSR_ACTIVE_MASK)
    {
        xBitStatus = SET;
    }
    else
    {
        xBitStatus = RESET;
    }
    
    return (xBitStatus);
}

FLAGSTATUS DMA_GetDoneFlagStatus(DMAMUX_MemMapPtr xDmamux, DMA_CHANNEL xDmaChannel)
{
    FLAGSTATUS xBitStatus = RESET;
    
    if(DMA_CSR_REG(DMA_BASE_PTR, xDmaChannel) & DMA_CSR_DONE_MASK)
    {
        xBitStatus = SET;
    }
    else
    {
        xBitStatus = RESET;
    }
    
    return (xBitStatus);
}

FLAGSTATUS DMA_GetFlagStatus(DMAMUX_MemMapPtr xDmamux, DMA_CHANNEL xDmaChannel)
{
    FLAGSTATUS xBitStatus = RESET;
    uint32_t u32Mask = 1;
    uint8_t i;

    for(i = 0; i < xDmaChannel; i ++)
    {
        u32Mask <<= 1;
    }
    
    if(DMA_INT & u32Mask)
    {
        xBitStatus = SET;
    }
    else
    {
        xBitStatus = RESET;
    }
    
    return (xBitStatus);
}

void DMA_ClearDoneFlag(DMAMUX_MemMapPtr xDmamux, DMA_CHANNEL xDmaChannel)
{
    DMA_CDNE = DMA_CDNE_CDNE(xDmaChannel);
}

void DMA_ClearFlag(DMAMUX_MemMapPtr xDmamux, DMA_CHANNEL xDmaChannel)
{
    uint32_t u32Mask = 1;
    uint8_t i;

    for(i = 0; i < xDmaChannel; i ++)
    {
        u32Mask <<= 1;
    }

    DMA_INT |= u32Mask;
}



