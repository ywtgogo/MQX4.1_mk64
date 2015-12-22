/*HEADER**********************************************************************
*
* Copyright 2011 Freescale Semiconductor, Inc.
*
* This software is owned or controlled by Freescale Semiconductor.
* Use of this software is governed by the Freescale MQX RTOS License
* distributed with this Material.
* See the MQX_RTOS_LICENSE file distributed for more details.
*
* Brief License Summary:
* This software is provided in source form for you to use free of charge,
* but it is not open source software. You are allowed to use this software
* but you cannot redistribute it or derivative works of it in source form.
* The software may be used only in connection with a product containing
* a Freescale microprocessor, microcontroller, or digital signal processor.
* See license agreement file for full license terms including other
* restrictions.
*****************************************************************************
*
* Comments:
*
*   This file contains the default initialization record for the
*   flash device. The 8313e-rdb uses a Spansion 29lv640 The
*   8323e-rdb uses a Intel Strata 28f128j3.
*
*
*END************************************************************************/
#ifndef __flash_mpxn20_h__
#define __flash_mpxn20_h__


#include "flashx.h"


/*----------------------------------------------------------------------*/
/* 
**              DEFINED VARIABLES
*/

extern const FLASHX_BLOCK_INFO_STRUCT _mpxn20_block_map[];


#endif

/* EOF */
