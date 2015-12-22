/*
 * Copyright (c) 2013, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _SPI_FLASH_H_
#define _SPI_FLASH_H_

#include "ioctl.h"
#include "mutex.h"

#define IO_IOCTL_SPIFLASH_ERASE             _IO(IO_TYPE_SPIFLASH, 0x01)
     
typedef struct spi_flash_init_struct
{
    const char *spidev;
    /* Default transfer parameters for low level driver */
    SPI_PARAM_STRUCT      PARAMS;
} SPI_FLASH_INIT_STRUCT, _PTR_ SPI_FLASH_INIT_STRUCT_PTR;

typedef const SPI_FLASH_INIT_STRUCT _PTR_ SPI_FLASH_INIT_STRUCT_CPTR;

struct spi_flash {
        MQX_FILE_PTR    spifd;

        const char      *name;

        /* Total flash size */
        uint32_t             size;
        /* Write (page) size */
        uint32_t             page_size;
        /* Erase (sector) size */
        uint32_t             sector_size;

        int32_t         (*read)(struct spi_flash *flash, uint32_t offset,
                                size_t len, void *buf);
        int32_t         (*write)(struct spi_flash *flash, uint32_t offset,
                                size_t len, const void *buf);
        int32_t         (*erase)(struct spi_flash *flash, uint32_t offset,
                                size_t len);
        MUTEX_STRUCT    MUTEX;
};

typedef struct spi_flash_driver_data_struct
{
    /* LWSEM for locking for concurrent access from several tasks */
    LWSEM_STRUCT          BUS_LOCK;

    /* Pointer to low level driver */
    SPI_DEVIF_STRUCT_CPTR DEVIF;

    /* Pointer to runtime data specific for low level driver */
    pointer               DEVIF_DATA;

    /* Default transfer parameters for low level driver */
    SPI_PARAM_STRUCT      PARAMS;
} SPI_FLASH_DRIVER_DATA_STRUCT, _PTR_ SPI_FLASH_DRIVER_DATA_STRUCT_PTR;

/*
** Context information for open fd
*/
typedef struct spi_flash_dev_data_struct
{
    /* Inidicates that BUS_LOCK is being held by this fd */
    bool                  BUS_OWNER;

    /* Transfer parameters for low level driver */
    SPI_PARAM_STRUCT      PARAMS;

    /* Indicates necessity to re-set parameters before transfer */
    bool                  PARAMS_DIRTY;

    /* Open flags for this channel */
    uint32_t               FLAGS;
} SPI_FLASH_DEV_DATA_STRUCT, _PTR_ SPI_FLASH_DEV_DATA_STRUCT_PTR;

//! @addtogroup spi_flash_common
//! @{

//! @file

#define ROUND(a,b)	(((a) + (b)) & ~((b) - 1))

#ifndef min
#define min(a, b)   ((a) < (b) ? (a) : (b))
#endif
#ifndef max
#define max(a, b)   ((a) > (b) ? (a) : (b))
#endif

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

#if defined(__cplusplus)
extern "C" {
#endif

_mqx_int spi_flash_common_init(const void _PTR_ init_data_ptr, pointer *io_info_ptr_ptr);
_mqx_int spi_flash_common_free(pointer io_info_ptr_ptr);

static inline _mqx_int spi_flash_read(pointer io_info_ptr, uint32_t offset, size_t len, void *buf)
{
    SPI_FLASH_DRIVER_DATA_STRUCT_PTR driver_data = (SPI_FLASH_DRIVER_DATA_STRUCT_PTR)io_info_ptr;
    struct spi_flash *flash = (struct spi_flash *)driver_data->DEVIF_DATA;

    return flash->read(flash, offset, len, buf);
}

static inline _mqx_int spi_flash_write(pointer io_info_ptr, uint32_t offset, size_t len, void *buf)
{
    SPI_FLASH_DRIVER_DATA_STRUCT_PTR driver_data = (SPI_FLASH_DRIVER_DATA_STRUCT_PTR)io_info_ptr;
    struct spi_flash *flash = (struct spi_flash *)driver_data->DEVIF_DATA;

    return flash->write(flash, offset, len, buf);
}

static inline _mqx_int spi_flash_erase(pointer io_info_ptr, uint32_t offset, size_t len)
{
    SPI_FLASH_DRIVER_DATA_STRUCT_PTR driver_data = (SPI_FLASH_DRIVER_DATA_STRUCT_PTR)io_info_ptr;
    struct spi_flash *flash = (struct spi_flash *)driver_data->DEVIF_DATA;

    return flash->erase(flash, offset, len);
}

extern _mqx_int _io_spi_flash_install(char_ptr identifier, SPI_FLASH_INIT_STRUCT_CPTR init_data_ptr);

#if defined(__cplusplus)
}
#endif

//! @}

#endif // _SPI_FLASH_H_

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
