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
* $FileName: adc16.h$
* $Version : 3.8.35.0$
* $Date    : Sep-21-2012$
*
* Comments:
*
*   This file contains ADC16 module driver.
*
*END************************************************************************/

#ifndef __ADC_H__
#define __ADC_H__

#include <mqx.h>
#include <bsp.h>
#include "common.h"

#define ADC0     ADC0_BASE_PTR
#define ADC1     ADC1_BASE_PTR

#define IS_ADC_ALL_PERIPH(PERIPH) (((PERIPH) == ADC0) || ((PERIPH) == ADC1))

typedef enum
{
    ADC_DIFF_DISABLE = 0x00, //差分使能关闭
    ADC_DIFF_ENABLE  = 0x01  //差分使能开启
}ADC_DIFF;

#define IS_ADC_DIFF(DIFF) (((DIFF) == ADC_DIFF_DISABLE) || ((DIFF) == ADC_DIFF_ENABLE))

typedef enum
{
    ADC_CHANNEL_DP0        = 0x00,
    ADC_CHANNEL_DP1        = 0x01,
    ADC_CHANNEL_DP2        = 0x02,
    ADC_CHANNEL_DP3        = 0x03,
    ADC_CHANNEL_DP0_DIFF   = 0x20,
    ADC_CHANNEL_DP1_DIFF   = 0x21,
    ADC_CHANNEL_DP2_DIFF   = 0x22,
    ADC_CHANNEL_DP3_DIFF   = 0x23,
    ADC_CHANNEL_SE4x       = 0x04,
    ADC_CHANNEL_SE5x       = 0x05,
    ADC_CHANNEL_SE6x       = 0x06,
    ADC_CHANNEL_SE7x       = 0x07,
    ADC_CHANNEL_SE8        = 0x08,
    ADC_CHANNEL_SE9        = 0x09,
    ADC_CHANNEL_SE10       = 0x0A,
    ADC_CHANNEL_SE11       = 0x0B,
    ADC_CHANNEL_SE12       = 0x0C,
    ADC_CHANNEL_SE13       = 0x0D,
    ADC_CHANNEL_SE14       = 0x0E,
    ADC_CHANNEL_SE15       = 0x0F,
    ADC_CHANNEL_SE16       = 0x10,
    ADC_CHANNEL_SE17       = 0x11,
    ADC_CHANNEL_SE18       = 0x12,
    ADC_CHANNEL_DM0        = 0x13,
    ADC_CHANNEL_DM1        = 0x14,
    ADC_CHANNEL_SE21       = 0x15,
    ADC_CHANNEL_SE22       = 0x16,
    ADC_CHANNEL_DAC        = 0x17,
    ADC_CHANNEL_TEMPSENEOR = 0x1A,
    ADC_CHANNEL_BANDGAP    = 0x1B,
    ADC_CHANNEL_VREFSH     = 0x1D,
    ADC_CHANNEL_VREFSL     = 0x1E,
    ADC_CHANNEL_DISABLE    = 0x1F
}ADC_CHANNEL;
#define IS_ADC_CHANNEL(CHANNEL) (((CHANNEL) == ADC_CHANNEL_DP0) || ((CHANNEL) == ADC_CHANNEL_DP1) ||\
                                 ((CHANNEL) == ADC_CHANNEL_PGA) || ((CHANNEL) == ADC_CHANNEL_DP3) ||\
                                 ((CHANNEL) == ADC_CHANNEL_SE4x) || ((CHANNEL) == ADC_CHANNEL_SE5x) ||\
                                 ((CHANNEL) == ADC_CHANNEL_SE6x) || ((CHANNEL) == ADC_CHANNEL_SE7x) ||\
                                 ((CHANNEL) == ADC_CHANNEL_SE8) || ((CHANNEL) == ADC_CHANNEL_SE9) ||\
                                 ((CHANNEL) == ADC_CHANNEL_SE10) || ((CHANNEL) == ADC_CHANNEL_SE11) ||\
                                 ((CHANNEL) == ADC_CHANNEL_SE12) || ((CHANNEL) == ADC_CHANNEL_SE13) ||\
                                 ((CHANNEL) == ADC_CHANNEL_SE14) || ((CHANNEL) == ADC_CHANNEL_SE15) ||\
                                 ((CHANNEL) == ADC_CHANNEL_SE16) || ((CHANNEL) == ADC_CHANNEL_SE17) ||\
                                 ((CHANNEL) == ADC_CHANNEL_SE18) || ((CHANNEL) == ADC_CHANNEL_DM0) ||\
                                 ((CHANNEL) == ADC_CHANNEL_DM1) || ((CHANNEL) == ADC_CHANNEL_DAC) ||\
                                 ((CHANNEL) == ADC_CHANNEL_TEMPSENEOR) || ((CHANNEL) == ADC_CHANNEL_BANDGAP) ||\
                                 ((CHANNEL) == ADC_CHANNEL_VREFSH) || ((CHANNEL) == ADC_CHANNEL_VREFSL) ||\
                                 ((CHANNEL) == ADC_CHANNEL_DISABLE))

typedef enum
{
    ADC_LOWPOWER_DISABLE = 0x00, //低电压配置关闭
    ADC_LOWPOWER_ENABLE  = 0x01  //低电压配置开启
}ADC_LOWPOWER;
#define IS_ADC_LOWPOWER(LOWPOWER) (((LOWPOWER) == ADC_LOWPOWER_DISABLE) || ((LOWPOWER) == ADC_LOWPOWER_ENABLE))

typedef enum
{
    ADC_CLOCK_DIVSION_1 = 0x00, //ADC时钟不分频
    ADC_CLOCK_DIVSION_2 = 0x01, //ADC时钟2分频
    ADC_CLOCK_DIVSION_4 = 0x02, //ADC时钟4分频
    ADC_CLOCK_DIVSION_8 = 0x03, //ADC时钟8分频
}ADC_CLOCK_DIVSION;
#define IS_ADC_CLOCK_DIVSION(DIVSION) (((DIVSION) == ADC_CLOCK_DIVSION_1) || ((DIVSION) == ADC_CLOCK_DIVSION_2) ||\
                                       ((DIVSION) == ADC_CLOCK_DIVSION_4) || ((DIVSION) == ADC_CLOCK_DIVSION_8))

typedef enum
{
    ADC_LONG_SAMPLE_DISABLE = 0x00, //长采样关闭
    ADC_LONG_SAMPLE_ENABLE  = 0x01  //长采样开启
}ADC_LONG_SAMPLE;
#define IS_ADC_LONG_SAMPLE(SAMPLE) (((SAMPLE) == ADC_LONG_SAMPLE_DISABLE) || ((SAMPLE) == ADC_LONG_SAMPLE_ENABLE))

typedef enum
{
    ADC_MODE_8_BIT  = 0x00, //8位精度
    ADC_MODE_12_BIT = 0x01, //12位精度
    ADC_MODE_10_BIT = 0x02, //10位精度
    ADC_MODE_16_BIT = 0x03  //16位精度
}ADC_MODE;
#define IS_ADC_MODE(MODE) (((MODE) == ADC_MODE_8_BIT) || ((MODE) == ADC_MODE_12_BIT) ||\
                           ((MODE) == ADC_MODE_10_BIT) || ((MODE) == ADC_MODE_16_BIT))

typedef enum
{
    ADC_CLOCK_BUSCLK      = 0x00, //总线时钟
    ADC_CLOCK_BUSCLK_DIV2 = 0x01, //总线时钟/2
    ADC_CLOCK_ALTCLK      = 0x02, //交替时钟
    ADC_CLOCK_ADACK       = 0x03  //异步时钟
}ADC_CLOCK;
#define IS_ADC_CLOCK(CLOCK) (((CLOCK) == ADC_CLOCK_BUSCLK) || ((CLOCK) == ADC_CLOCK_BUSCLK_DIV2) ||\
                             ((CLOCK) == ADC_CLOCK_ALTCLK) || ((CLOCK) == ADC_CLOCK_ADACK))

typedef enum
{
    ADC_MUXSEL_ADxxa = 0x00, //复用a
    ADC_MUXSEL_ADxxb = 0x01  //复用b
}ADC_MUXSEL;
#define IS_ADC_MUXSEL(MUXSEL) (((MUXSEL) == ADC_MUXSEL_ADxxa) || ((MUXSEL) == ADC_MUXSEL_ADxxb))

typedef enum
{
    ADC_ADACK_DISABLE = 0x00, //取消异步时钟
    ADC_ADACK_ENABLE  = 0x01  //开启异步时钟
}ADC_ADACK;
#define IS_ADC_ADACK(ADACK) (((ADACK) == ADC_ADACK_DISABLE) || ((ADACK) == ADC_ADACK_ENABLE))

typedef enum
{
    ADC_SPEED_NORMAL = 0x00, //正常转换次序
    ADC_SPEED_HIGH   = 0x01  //高速转换次序
}ADC_SPEED;
#define IS_ADC_SPEED(SPEED) (((SPEED) == ADC_SPEED_NORMAL) || ((SPEED) == ADC_SPEED_HIGH))

typedef enum
{
    ADC_LONG_SAMPLE_TIME_20ADCK = 0x00, //额外20个ADCK周期
    ADC_LONG_SAMPLE_TIME_12ADCK = 0x01, //额外12个ADCK周期
    ADC_LONG_SAMPLE_TIME_6ADCK  = 0x02, //额外6个ADCK周期
    ADC_LONG_SAMPLE_TIME_2ADCK  = 0x03  //额外2个ADCK周期
}ADC_LONG_SAMPLE_TIME;
#define IS_ADC_LONG_SAMPLE_TIME(TIME) (((TIME) == ADC_LONG_SAMPLE_TIME_20ADCK) || ((TIME) == ADC_LONG_SAMPLE_TIME_12ADCK) ||\
                                       ((TIME) == ADC_LONG_SAMPLE_TIME_6ADCK) || ((TIME) == ADC_LONG_SAMPLE_TIME_2ADCK))

typedef enum
{
    ADC_TRIGGER_SOFTWARE = 0x00, //软件激活转换
    ADC_TRIGGER_HARDWARE = 0x01  //硬件激活转换
}ADC_TRIGGER;
#define IS_ADC_TRIGGER(TRIGGER) (((TRIGGER) == ADC_TRIGGER_SOFTWARE) || ((TRIGGER) == ADC_TRIGGER_HARDWARE))

typedef enum
{
    ADC_COMPARE_DISABLE = 0x00, //比较功能关闭
    ADC_COMPARE_ENABLE  = 0x01  //比较功能开启
}ADC_COMPARE;
#define IS_ADC_COMPARE(COMPARE) (((COMPARE) == ADC_COMPARE_DISABLE) || ((COMPARE) == ADC_COMPARE_ENABLE))

typedef enum
{
    ADC_REFSEL_EXTERNAL = 0x00, //参考外部电压
    ADC_REFSEL_INTERNAL = 0x01  //参考内部电压
}ADC_REFSEL;
#define IS_ADC_REFSEL(REFSEL) (((REFSEL) == ADC_REFSEL_EXTERNAL) || ((REFSEL) == ADC_REFSEL_INTERNAL))

typedef enum
{
    ADC_AVERAGE_DISABLE = 0x00, //硬件均值关闭
    ADC_AVERAGE_ENABLE  = 0x01  //硬件均值开启
}ADC_AVERAGE;
#define IS_ADC_AVERAGE(AVERAGE) (((AVERAGE) == ADC_AVERAGE_DISABLE) || ((AVERAGE) == ADC_AVERAGE_ENABLE))

typedef enum
{
    ADC_AVERAGE_SELECT_4    = 0x00, //4个采样值取平均
    ADC_AVERAGE_SELECT_8    = 0x01, //8个采样值取平均
    ADC_AVERAGE_SELECT_16   = 0x02, //16个采样值取平均
    ADC_AVERAGE_SELECT_32   = 0x03  //32个采样值取平均
}ADC_AVERAGE_SELECT;
#define IS_ADC_AVERAGE_SELECT(SELECT) (((SELECT) == ADC_AVERAGE_SELECT_4) || ((SELECT) == ADC_AVERAGE_SELECT_8) ||\
                                       ((SELECT) == ADC_AVERAGE_SELECT_16) || ((SELECT) == ADC_AVERAGE_SELECT_32))

typedef enum
{
    ADC_AVERAGE_CONTINUOUS_1  = 0x00, //均值期间只有1次转换
    ADC_AVERAGE_CONTINUOUS_X  = 0x01  //均值期间多次转换
}ADC_AVERAGE_CONTINUOUS;
#define IS_ADC_AVERAGE_CONTINUOUS(CONTINUOUS) (((CONTINUOUS) == ADC_AVERAGE_CONTINUOUS_1) || ((CONTINUOUS) == ADC_AVERAGE_CONTINUOUS_X))

typedef enum
{
    ADC_PGA_DISABLE = 0x00, //程控放大器关闭
    ADC_PGA_ENABLE  = 0x01  //程控放大器开启
}ADC_PGA;
#define IS_ADC_PGA(PGA) (((PGA) == ADC_PGA_DISABLE) || ((PGA) == ADC_PGA_ENABLE))

typedef enum
{
    ADC_PGA_POWER_LOW     = 0x00, //程控放大器在低功耗下运行
    ADC_PGA_POWER_NORMAL  = 0x01  //程控放大器在正常功耗下运行
}ADC_PGA_POWER;
#define IS_ADC_PGA_POWER(POWER) (((POWER) == ADC_PGA_POWER_LOW) || ((POWER) == ADC_PGA_POWER_NORMAL))

typedef enum
{
    ADC_PGA_GAIN_1  = 0x00, //放大倍数
    ADC_PGA_GAIN_2  = 0x01,
    ADC_PGA_GAIN_4  = 0x02,
    ADC_PGA_GAIN_8  = 0x03,
    ADC_PGA_GAIN_16 = 0x04,
    ADC_PGA_GAIN_32 = 0x05,
    ADC_PGA_GAIN_64 = 0x06
}ADC_PGA_GAIN;
#define IS_ADC_PGA_GAIN(GAIN) (((GAIN) == ADC_PGA_GAIN_1) || ((GAIN) == ADC_PGA_GAIN_2) ||\
                               ((GAIN) == ADC_PGA_GAIN_4) || ((GAIN) == ADC_PGA_GAIN_8) ||\
                               ((GAIN) == ADC_PGA_GAIN_16) || ((GAIN) == ADC_PGA_GAIN_32) ||\
                               ((GAIN) == ADC_PGA_GAIN_64))

typedef struct
{
    ADC_CLOCK               xAdcClock;              //时钟
    ADC_CLOCK_DIVSION       xAdcClockDivsion;       //时钟分频
    ADC_ADACK               xAdcAck;                //异步时钟输出
    ADC_DIFF                xAdcDiff;               //差分使能
    ADC_LOWPOWER            xAdcLowpower;           //低功耗模式
    ADC_MODE                xAdcMode;               //采样精度
    ADC_LONG_SAMPLE         xAdcLongSample;         //长采样
    ADC_LONG_SAMPLE_TIME    xAdcLongSampleTime;     //长采样时间
    ADC_SPEED               xAdcSpeed;              //转换速度
    ADC_TRIGGER             xAdcTrigger;            //触发方式
    ADC_REFSEL              xAdcRefsel;             //参考电压
    ADC_AVERAGE             xAdcAverage;            //硬件均值
    ADC_AVERAGE_SELECT      xAdcAverageSelect;      //硬件均值采样数量
    ADC_AVERAGE_CONTINUOUS  xAdcAverageContinuous;  //硬件均值连续转换次数
	void                    (*old_isr)(pointer);    //中断服务程序入口
	pointer                 old_isr_data;           //中断服务程序参数
}ADC_PARAMETER;

typedef struct
{
    ADC_PGA_POWER           xAdcPgaPower;           //运行环境
    ADC_PGA_GAIN            xAdcPgaGain;            //放大倍数
}ADC_PGA_PARAMETER;

#define ADC_PGA_PGLPB_MASK                        0x100000u


/* Initialization and Configuration functions *********************************/
void ADC_Init(ADC_MemMapPtr pxAdcMoudelNumber, ADC_PARAMETER* pxAdcParameter);
uint8_t ADC_Cal(ADC_MemMapPtr pxAdcMoudelNumber);
void ADC_StructInit(ADC_PARAMETER* pxAdcParameter);
void ADC_DMACmd(ADC_MemMapPtr pxAdcMoudelNumber, FUNCTIONALSTATE xNewState);
void ADC_ITConfig(ADC_MemMapPtr pxAdcMoudelNumber, ADC_PARAMETER* pxAdcParameter, FUNCTIONALSTATE xNewState);
void ADC_MuxCmd(ADC_MemMapPtr pxAdcMoudelNumber, ADC_MUXSEL xAdcMux);
void ADC_Cmd(ADC_MemMapPtr pxAdcMoudelNumber, ADC_CHANNEL xChannel);
#if 0
void ADC_PGA_Init(ADC_MemMapPtr pxAdcMoudelNumber, ADC_PGA_PARAMETER* pxAdcPgaParameter);
void ADC_PGA_StructInit(ADC_PGA_PARAMETER* pxAdcPgaParameter);
void ADC_PGA_Cmd(ADC_MemMapPtr pxAdcMoudelNumber, FUNCTIONALSTATE xNewState);
#endif

/* Read and Write functions **********************************************/
uint16_t ADC_GetDataResult(ADC_MemMapPtr pxAdcMoudelNumber, ADC_PARAMETER* pxAdcParameter);
FLAGSTATUS ADC_GetFlagConversionProgress(ADC_MemMapPtr pxAdcMoudelNumber);
FLAGSTATUS ADC_GetFlagStatus(ADC_MemMapPtr pxAdcMoudelNumber);
//void ADC_ClearFlag(ADC_MemMapPtr pxAdcMoudelNumber); 读取AD值或者写通道值，都会擦除标志位。

/* ADC ISRs */
void adc0_isr(pointer isr);
void adc1_isr(pointer isr);

#endif  /* __ADC_H */

