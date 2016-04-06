#ifndef __PIT_H__
#define __PIT_H__

typedef enum
{ 
	PIT0 = 0x00,
	PIT1 = 0x01,
	PIT2 = 0x02,
	PIT3 = 0x03
}PIT_CHANNEL;
#define IS_PIT_ALL_CHANNEL(CHANNEL)  (((CHANNEL) == PIT0) || \
                                     ((CHANNEL) == PIT1) || \
                                     ((CHANNEL) == PIT2) || \
                                     ((CHANNEL) == PIT3))

/* Initialization and Configuration functions *********************************/
void PIT_Init(PIT_CHANNEL xPitNumber);
void PIT_Cmd(PIT_CHANNEL xPitNumber, FUNCTIONALSTATE xNewState);
void PIT_ITConfig(PIT_CHANNEL xPitNumber, FUNCTIONALSTATE xNewState);

/* Read and Write functions **********************************************/
void PIT_SetAutoreload(PIT_CHANNEL xPitNumber, uint32 u32LoadNumber);
void PIT_SetCounter(PIT_CHANNEL xPitNumber, uint32 u32CounterNumber);
uint32 PIT_GetCounter(PIT_CHANNEL xPitNumber);
FLAGSTATUS PIT_GetFlagStatus(PIT_CHANNEL xPitNumber);
void PIT_ClearFlag(PIT_CHANNEL xPitNumber);

#endif
