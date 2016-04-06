#ifndef __FTM_H__
#define __FTM_H__

#define FTM0     FTM0_BASE_PTR
#define FTM1     FTM1_BASE_PTR
#define FTM2     FTM2_BASE_PTR
#define FTM3     FTM3_BASE_PTR
#define IS_FTM_ALL_PERIPH(PERIPH) (((PERIPH) == FTM0) || ((PERIPH) == FTM1) ||\
                                   ((PERIPH) == FTM2) || ((PERIPH) == FTM3) )

typedef enum
{
    FTM_CHANNEL_0        = 0,
    FTM_CHANNEL_1        = 1,
    FTM_CHANNEL_2        = 2,
    FTM_CHANNEL_3        = 3,
    FTM_CHANNEL_4        = 4,
    FTM_CHANNEL_5        = 5,
    FTM_CHANNEL_6        = 6,
    FTM_CHANNEL_7        = 7
}FTM_CHANNEL;
#define IS_FTM_CHANNEL(CHANNEL) (((CHANNEL) == FTM_CHANNEL_0) || ((CHANNEL) == FTM_CHANNEL_1) ||\
                                 ((CHANNEL) == FTM_CHANNEL_2) || ((CHANNEL) == FTM_CHANNEL_3) ||\
                                 ((CHANNEL) == FTM_CHANNEL_4) || ((CHANNEL) == FTM_CHANNEL_5) ||\
                                 ((CHANNEL) == FTM_CHANNEL_6) || ((CHANNEL) == FTM_CHANNEL_7) ) 

typedef enum
{
    FTM_CLOCK_DISABLE   = 0x00, //关闭时钟
    FTM_CLOCK_SYSTEM    = 0x01, //系统时钟
    FTM_CLOCK_FIXED     = 0x02, //固定频率
    FTM_CLOCK_EXTEMAL   = 0x03  //外部时钟
}FTM_CLOCK;
#define IS_FTM_CLOCK(CLOCK) (((CLOCK) == FTM_CLOCK_DISABLE) || ((CLOCK) == FTM_CLOCK_SYSTEM) ||\
                             ((CLOCK) == FTM_CLOCK_FIXED)   || ((CLOCK) == FTM_CLOCK_EXTEMAL) )

typedef enum
{
    FTM_PRESCALE_FACTOR_1   = 0x00, //分频因子
    FTM_PRESCALE_FACTOR_2   = 0x01,
    FTM_PRESCALE_FACTOR_4   = 0x02,
    FTM_PRESCALE_FACTOR_8   = 0x03,
    FTM_PRESCALE_FACTOR_16  = 0x04,
    FTM_PRESCALE_FACTOR_32  = 0x05,
    FTM_PRESCALE_FACTOR_64  = 0x06,
    FTM_PRESCALE_FACTOR_128 = 0x07
}FTM_PRESCALE_FACTOR;
#define IS_FTM_PRESCALE_FACTOR(PF) (((PF) == FTM_PRESCALE_FACTOR_1)  || ((PF) == FTM_PRESCALE_FACTOR_2)  ||\
                                    ((PF) == FTM_PRESCALE_FACTOR_4)  || ((PF) == FTM_PRESCALE_FACTOR_8)  ||\
                                    ((PF) == FTM_PRESCALE_FACTOR_16) || ((PF) == FTM_PRESCALE_FACTOR_32) ||\
                                    ((PF) == FTM_PRESCALE_FACTOR_64) || ((PF) == FTM_PRESCALE_FACTOR_128) )

typedef enum
{
    FTM_COUNTING_MODE_UP        = 0x00, //向上计数
    FTM_COUNTING_MODE_UP_DOWN   = 0x01  //上下计数，中间对齐
}FTM_COUNTING_MODE;
#define IS_FTM_COUNTING_MODE(COUNTING_MODE) (((COUNTING_MODE) == FTM_COUNTING_MODE_UP) || ((COUNTING_MODE) == FTM_COUNTING_MODE_UP_DOWN))

typedef enum
{
    FTM_PWM_MODE_EDGE       = 0x01, //边沿对称模式
    FTM_PWM_MODE_CENTER     = 0x02, //中间对称模式
    FTM_PWM_MODE_COMBINE    = 0x03  //联合输出模式
}FTM_PWM_MODE;
#define IS_FTM_PWM_MODE(MODE) (((MODE) == FTM_PWM_MODE_EDGE) || ((MODE) == FTM_PWM_MODE_CENTER) ||\
                               ((MODE) == FTM_PWM_MODE_COMBINE) )

typedef enum
{
    FTM_PWM_TRUE_HIGH   = 0x00, //正极性
    FTM_PWM_TRUE_LOW    = 0x01  //负极性
}FTM_PWM_TRUE;
#define IS_FTM_PWM_TRUE(MODE) (((MODE) == FTM_PWM_TRUE_HIGH) || ((MODE) == FTM_PWM_TRUE_LOW))

typedef enum
{
    FTM_PWM_LOADING_UPDATE_DISABLE   = 0x00, //不允许重载
    FTM_PWM_LOADING_UPDATE_ENABLE    = 0x01  //允许重载
}FTM_PWM_LOADING_UPDATE;
#define IS_FTM_PWM_LOADING_UPDATE(LOADING) (((LOADING) == FTM_PWM_LOADING_UPDATE_DISABLE) || ((LOADING) == FTM_PWM_LOADING_UPDATE_ENABLE))

typedef struct
{
    FTM_PWM_MODE            xFtmPwmMode;            //PWM模式
    FTM_PWM_TRUE            xFtmPwmTrue;            //正负极性
    FTM_PWM_LOADING_UPDATE  xFtmPwmLoadingUpdate;   //重载
}FTM_PWM_PARAMETER;

typedef struct
{
    FTM_CLOCK               xFtmClock;              //时钟源
    FTM_PRESCALE_FACTOR     xFtmPF;                 //预分频因子
    FTM_COUNTING_MODE       xFtmCountingMode;       //计数模式
}FTM_PARAMETER;

/* Initialization and Configuration functions *********************************/
void FTM_Init(FTM_MemMapPtr pxFtmNumber, FTM_PARAMETER* pxParameter);
void FTM_StructInit(FTM_PARAMETER* pxParameter);
void FTM_Cmd(FTM_MemMapPtr pxFtmNumber, FTM_PARAMETER* pxParameter);
void FTM_ITConfig(FTM_MemMapPtr pxFtmNumber, FUNCTIONALSTATE xNewState);

void FTM_PwmInit(FTM_MemMapPtr pxFtmNumber, FTM_CHANNEL xFtmChannel, FTM_PWM_PARAMETER* pxPwmParameter);
void FTM_PwmStructInit(FTM_PWM_PARAMETER* pxPwmParameter);

/* Read and Write functions **********************************************/
void FTM_SetModulo(FTM_MemMapPtr pxFtmNumber, uint16 u16LoadNumber);
void FTM_SetCounter(FTM_MemMapPtr pxFtmNumber, uint16 u16LoadNumber);
void FTM_SetCounterInitial(FTM_MemMapPtr pxFtmNumber, uint16 u16LoadNumber);
void FTM_SetChannelValue(FTM_MemMapPtr pxFtmNumber, FTM_CHANNEL xFtmChannel, uint16 u16LoadNumber);
FLAGSTATUS FTM_GetChannelFlagStatus(FTM_MemMapPtr pxFtmNumber, FTM_CHANNEL xFtmChannel);

#endif

