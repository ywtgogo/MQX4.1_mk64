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
* $FileName: dma.h$
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

#ifndef __APP_DMA_H__
#define __APP_DMA_H__

#include <mqx.h>
#include <bsp.h>
#include "common.h"

#define DMAMUX DMAMUX_BASE_PTR

typedef enum
{
    DMAMUX_SRC_CHANNEL_UART0_RX        = 2,
    DMAMUX_SRC_CHANNEL_UART0_TX        = 3,
    DMAMUX_SRC_CHANNEL_UART1_RX        = 4,
    DMAMUX_SRC_CHANNEL_UART1_TX        = 5,
    DMAMUX_SRC_CHANNEL_UART2_RX        = 6,
    DMAMUX_SRC_CHANNEL_UART2_TX        = 7,
    DMAMUX_SRC_CHANNEL_UART3_RX        = 8,
    DMAMUX_SRC_CHANNEL_UART3_TX        = 9,
    DMAMUX_SRC_CHANNEL_UART4_TXRX      = 10,
    DMAMUX_SRC_CHANNEL_UART5_TXRX      = 11,
    DMAMUX_SRC_CHANNEL_I2S0_RX         = 12,
    DMAMUX_SRC_CHANNEL_I2S0_TX         = 13,
    DMAMUX_SRC_CHANNEL_SPI0_RX         = 14,
    DMAMUX_SRC_CHANNEL_SPI0_TX         = 15,
    DMAMUX_SRC_CHANNEL_SPI1_TXRX       = 16,
    DMAMUX_SRC_CHANNEL_SPI2_TXRX       = 17,
    DMAMUX_SRC_CHANNEL_I2C0            = 18,
    DMAMUX_SRC_CHANNEL_I2C1_I2C2       = 19,
    DMAMUX_SRC_CHANNEL_FTM0_CH0        = 20,
    DMAMUX_SRC_CHANNEL_FTM0_CH1        = 21,
    DMAMUX_SRC_CHANNEL_FTM0_CH2        = 22,
    DMAMUX_SRC_CHANNEL_FTM0_CH3        = 23,
    DMAMUX_SRC_CHANNEL_FTM0_CH4        = 24,
    DMAMUX_SRC_CHANNEL_FTM0_CH5        = 25,
    DMAMUX_SRC_CHANNEL_FTM0_CH6        = 26,
    DMAMUX_SRC_CHANNEL_FTM0_CH7        = 27,
    DMAMUX_SRC_CHANNEL_FTM1_CH0        = 28,
    DMAMUX_SRC_CHANNEL_FTM1_CH1        = 29,
    DMAMUX_SRC_CHANNEL_FTM2_CH0        = 30,
    DMAMUX_SRC_CHANNEL_FTM2_CH1        = 31,
    DMAMUX_SRC_CHANNEL_FTM3_CH0        = 32,
    DMAMUX_SRC_CHANNEL_FTM3_CH1        = 33,
    DMAMUX_SRC_CHANNEL_FTM3_CH2        = 34,
    DMAMUX_SRC_CHANNEL_FTM3_CH3        = 35,
    DMAMUX_SRC_CHANNEL_FTM3_CH4        = 36,
    DMAMUX_SRC_CHANNEL_FTM3_CH5        = 37,
    DMAMUX_SRC_CHANNEL_FTM3_CH6        = 38,
    DMAMUX_SRC_CHANNEL_FTM3_CH7        = 39,
    DMAMUX_SRC_CHANNEL_ADC0            = 40,
    DMAMUX_SRC_CHANNEL_ADC1            = 41,
    DMAMUX_SRC_CHANNEL_CMP0            = 42,
    DMAMUX_SRC_CHANNEL_CMP1            = 43,
    DMAMUX_SRC_CHANNEL_CMP2            = 44,
    DMAMUX_SRC_CHANNEL_DAC0            = 45,
    DMAMUX_SRC_CHANNEL_DAC1            = 46,
    DMAMUX_SRC_CHANNEL_CMT             = 47,
    DMAMUX_SRC_CHANNEL_PDB             = 48,
    DMAMUX_SRC_CHANNEL_PORTA           = 49,
    DMAMUX_SRC_CHANNEL_PORTB           = 50,
    DMAMUX_SRC_CHANNEL_PORTC           = 51,
    DMAMUX_SRC_CHANNEL_PORTD           = 52,
    DMAMUX_SRC_CHANNEL_PORTE           = 53,
    DMAMUX_SRC_CHANNEL_IEEE_1588_T0    = 54,
    DMAMUX_SRC_CHANNEL_IEEE_1588_T1    = 55,
    DMAMUX_SRC_CHANNEL_IEEE_1588_T2    = 56,
    DMAMUX_SRC_CHANNEL_IEEE_1588_T3    = 57,    
}DMAMUX_SRC_CHANNEL;
#define IS_DMA_SRC_CHANNEL(CHANNEL) (((CHANNEL) >= DMAMUX_SRC_CHANNEL_UART0_RX) && ((CHANNEL) <= DMAMUX_SRC_CHANNEL_IEEE_1588_T3))

typedef enum
{
    DMA_CHANNEL_0        = 0,
    DMA_CHANNEL_1        = 1,
    DMA_CHANNEL_2        = 2,
    DMA_CHANNEL_3        = 3,
    DMA_CHANNEL_4        = 4,
    DMA_CHANNEL_5        = 5,
    DMA_CHANNEL_6        = 6,
    DMA_CHANNEL_7        = 7,
    DMA_CHANNEL_8        = 8,
    DMA_CHANNEL_9        = 9,
    DMA_CHANNEL_10       = 10,
    DMA_CHANNEL_11       = 11,
    DMA_CHANNEL_12       = 12,
    DMA_CHANNEL_13       = 13,
    DMA_CHANNEL_14       = 14,
    DMA_CHANNEL_15       = 15,
    DMA_CHANNEL_16       = 16
}DMA_CHANNEL;
#define IS_DMA_CHANNEL(CHANNEL) ((CHANNEL) <= DMA_CHANNEL_16)

typedef enum
{
    DMA_TRANSFER_SIZE_8_BIT   = 0,
    DMA_TRANSFER_SIZE_16_BIT  = 1,
    DMA_TRANSFER_SIZE_32_BIT  = 2,
    DMA_TRANSFER_SIZE_16_BYTE = 4,
    DMA_TRANSFER_SIZE_32_BYTE = 5
}DMA_TRANSFER_SIZE;
#define IS_DMA_TRANSFER_SIZE(SIZE)  (((SIZE) == DMA_TRANSFER_SIZE_8_BIT)  || ((SIZE) == DMA_TRANSFER_SIZE_16_BIT) ||\
                                     ((SIZE) == DMA_TRANSFER_SIZE_32_BIT) || ((SIZE) == DMA_TRANSFER_SIZE_16_BYTE) ||\
                                     ((SIZE) == DMA_TRANSFER_SIZE_32_BYTE))




typedef struct
{
    DMAMUX_SRC_CHANNEL      xDmaSrcChannel;                         //DMA资源通道号
    uint32_t                  u32SourceAddress;                       //源地址
    uint32_t                  u32DestinationAddress;                  //目标地址
    uint16_t                  u16SourceNextValueOffset;               //源值一下数据项的地址偏移
    uint16_t                  u16DestinationNextValueOffset;          //目标值下一个数据项的地址偏移
    uint8_t                   u8SourceAddressModulo;                  //2的N次方，这个值将决定最后一个数据项的位置，并把指针指回数据项的首地址
    uint8_t                   u8DestinationAddressModulo;             //2的N次方，这个值将决定最后一个数据项的位置，并把指针指回数据项的首地址
    DMA_TRANSFER_SIZE       xSourceDataSize;                        //源数据项的长度
    DMA_TRANSFER_SIZE       xDestinationDataSize;                   //目标数据项的长度
    uint32_t                  u32ByteTransferCount;                   //每次传输的字节数
    uint32_t                  u32LastSourceAddressAdjustment;         //最后一个源的地址调整，调整方式:增加
    uint32_t                  u32LastDestinationAddressAdjustment;    //最后一个目标地址调整，调整方式:增加
    uint16_t                  u16BufferSize;                          //传输数据数量
}DMA_PARAMETER;

/* Initialization and Configuration functions *********************************/
void DMA_Init(DMA_CHANNEL xDmaChannel, DMAMUX_MemMapPtr xDmamux, DMA_PARAMETER* pxDmaParameter);
void DMA_StructInit(DMA_PARAMETER* pxDmaParameter);
void DMA_ITConfig(DMA_CHANNEL xDmaChannel, DMAMUX_MemMapPtr xDmamux, FUNCTIONALSTATE xNewState);
void DMA_Cmd(DMA_CHANNEL xDmaChannel, DMAMUX_MemMapPtr xDmamux, FUNCTIONALSTATE xNewState);
void DMA_EnableRequest(DMAMUX_MemMapPtr xDmamux, DMA_CHANNEL xDmaChannel);


/* Read and Write functions **********************************************/
FLAGSTATUS DMA_GetActiveFlagStatus(DMAMUX_MemMapPtr xDmamux, DMA_CHANNEL xDmaChannel);
FLAGSTATUS DMA_GetDoneFlagStatus(DMAMUX_MemMapPtr xDmamux, DMA_CHANNEL xDmaChannel);
FLAGSTATUS DMA_GetFlagStatus(DMAMUX_MemMapPtr xDmamux, DMA_CHANNEL xDmaChannel);
void DMA_ClearDoneFlag(DMAMUX_MemMapPtr xDmamux, DMA_CHANNEL xDmaChannel);
void DMA_ClearFlag(DMAMUX_MemMapPtr xDmamux, DMA_CHANNEL xDmaChannel);


#endif

