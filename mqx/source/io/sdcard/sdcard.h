#ifndef __sdcard_h__
#define __sdcard_h__
/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
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
*   The file contains functions prototype, defines, structure 
*   definitions private to the SD card.
*
*
*END************************************************************************/

/*----------------------------------------------------------------------*/
/*
**                          CONSTANT DEFINITIONS
*/

#define IO_SDCARD_BLOCK_SIZE_POWER   (9)
#define IO_SDCARD_BLOCK_SIZE         (1 << IO_SDCARD_BLOCK_SIZE_POWER)

#define IO_IOCTL_SDCARD_GET_CID      _IO(IO_TYPE_SDCARD,0x01)
#define IO_IOCTL_SDCARD_GET_DEVICE_INFO _IO(IO_TYPE_SDCARD, 0x02)

/*----------------------------------------------------------------------*/
/*
**                    DATATYPE DEFINITIONS
*/

/* MMC version */
typedef enum mmcStandardVersion {
    MmcV4_0 = 0,
    MmcV4_1 = 1,
    MmcV4_2 = 2,
    MmcV4_3 = 3,
    Obsolete = 4,
    MmcV4_41 = 5,
    MmcV4_5_or_V4_51 = 6,
    MmcV5_0 = 7
} MMC_STANDARD_VERSION;

typedef enum mmcEstLifetimePercent {
    Used_0_to_10 = 1,
    Used_10_to_20 = 2,
    Used_20_to_30 = 3,
    Used_30_to_40 = 4,
    Used_40_to_50 = 5,
    Used_50_to_60 = 6,
    Used_60_to_70 = 7,
    Used_70_to_80 = 8,
    Used_80_to_90 = 9,
    Used_90_to_100 = 10,
    Exceeded_Est_Life = 11
} MMC_EST_LIFETIME_PERCENT;

typedef enum mmcReservedBlockCondition {
    Normal = 1,
    Warning_80PercentPlus = 2,
    Urgent = 3
} MMC_RESERVED_BLOCK_CONDITION;

/*
 * EMMC_DEVICE_DATA STRUCT
 * Contains device specific information about the MMC device
 */
typedef struct emmcDeviceInformation
{
    /* Manufacturer ID code defined and allocated to an e•MMC manufacturer by the MMCA/JEDEC. */
    uint8_t ManufacturerID;

    /* Device type - 0 = Removable, 1 = BGA (e•MMC), 2 = POP, 3 = Reserved. */
    uint8_t DeviceType;

    /*
     * OEM application ID - identifies the Device OEM and/or the Device contents.
     * Controlled, defined and allocated to an e•MMC manufacturer by the MMCA/JEDEC.
     */
    uint8_t OemApplicationId;

    /* 6 letter string (excluding null termination) containing part name */
    char PartName[7];

    /* Product revision (major.minor) */
    uint8_t MajorProductRevision;
    uint8_t MinorProductRevision;

    /* Product Serial Number */
    uint32_t SerialNumber;

    /* Manufacture Date (Month and Year) */
    uint8_t ManufactureMonth;
    uint16_t ManufactureYear;

    /* Version of the MMC standard supported*/
    MMC_STANDARD_VERSION MmcStandardVersion;

    MMC_EST_LIFETIME_PERCENT MemALifetimeUsed;

    MMC_EST_LIFETIME_PERCENT MemBLifetimeUsed;

    MMC_RESERVED_BLOCK_CONDITION ReservedBlockStatus;

} EMMC_DEVICE_DATA;

/*
** SDCARD_INIT STRUCT
**
** The address of this structure is used to maintain sd card init
** information.
*/
typedef struct sdcard_init_struct
{
   /* The function to call to initialize sd card */
   bool  (_CODE_PTR_ INIT_FUNC)(MQX_FILE_PTR);

   /* The function to call to read blocks from sd card */
   _mqx_int (_CODE_PTR_ READ_FUNC)(MQX_FILE_PTR, unsigned char *, uint32_t, uint32_t);

   /* The function to call to write blocks to sd card */
   _mqx_int (_CODE_PTR_ WRITE_FUNC)(MQX_FILE_PTR, unsigned char *, uint32_t, uint32_t);
   
   /* Signal specification */
   uint32_t             SIGNALS;

} SDCARD_INIT_STRUCT, * SDCARD_INIT_STRUCT_PTR;


#ifdef __cplusplus
extern "C" {
#endif

extern _mqx_int _io_sdcard_install(char *,SDCARD_INIT_STRUCT_PTR,MQX_FILE_PTR);

#ifdef __cplusplus
}
#endif


#endif

/* EOF */
