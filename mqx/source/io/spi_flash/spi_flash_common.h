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

#ifndef __SPI_FLASH_INTERNAL_H__
#define __SPI_FLASH_INTERNAL_H__

#include "mqx.h"
#include "bsp.h"
#include "spi_flash.h"

//! @addtogroup spi_flash_internal
//! @{

//! @file

//! @brief  SPI Flash programming timeout.
#define SPI_FLASH_PROG_TIMEOUT		(20 * 1000)

//! @brief  SPI Flash page erase timeout.
#define SPI_FLASH_PAGE_ERASE_TIMEOUT	(50 * 1000)

//! @brief  SPI Flash sector erase timeout.
#define SPI_FLASH_SECTOR_ERASE_TIMEOUT	(100 * 1000)

#define CMD_READ_ID			0x9f 

#define CMD_READ_ARRAY_SLOW		0x03
#define CMD_READ_ARRAY_FAST		0x0b

#define CMD_WRITE_STATUS		0x01
#define CMD_PAGE_PROGRAM		0x02
#define CMD_WRITE_DISABLE		0x04
#define CMD_READ_STATUS			0x05
#define CMD_WRITE_ENABLE		0x06
#define CMD_ERASE_4K			0x20
#define CMD_ERASE_32K			0x52
#define CMD_ERASE_64K			0xd8
#define CMD_ERASE_CHIP			0xc7

//! @brief Common status bit
#define STATUS_WIP			0x01

#if defined(__cplusplus)
extern "C" {
#endif

int32_t spi_flash_common_send_cmd(MQX_FILE_PTR spifd, uint8_t cmd, void *response, size_t len);

int32_t spi_flash_common_read(MQX_FILE_PTR spifd, const uint8_t *cmd,
		size_t cmd_len, uint8_t *data, size_t data_len);

int32_t spi_flash_common_write(MQX_FILE_PTR spifd, const uint8_t *cmd, size_t cmd_len,
		const uint8_t *data, size_t data_len);

static inline int32_t spi_flash_common_write_enable(MQX_FILE_PTR spifd)
{
	return spi_flash_common_send_cmd(spifd, CMD_WRITE_ENABLE, NULL, 0);
}

static inline int32_t spi_flash_common_write_disable(MQX_FILE_PTR spifd)
{
	return spi_flash_common_send_cmd(spifd, CMD_WRITE_DISABLE, NULL, 0);
}

int32_t spi_flash_common_write_status(MQX_FILE_PTR spifd, uint8_t sr);

int32_t spi_flash_common_check_status(MQX_FILE_PTR spifd, uint32_t timeout,
			   uint8_t cmd, uint8_t poll_bit);

int32_t spi_flash_common_wait_cmd_ready(MQX_FILE_PTR spifd, uint32_t timeout);

int32_t spi_flash_winbond_init(MQX_FILE_PTR spifd, struct spi_flash *flash, uint8_t *idcode);

#if defined(__cplusplus)
}
#endif

//! @}

#endif
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
