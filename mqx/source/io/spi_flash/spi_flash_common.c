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

#include "mqx.h"
#include "bsp.h"

#include "spi_flash_common.h"
#include "spi_flash.h"

//#define DEBUG_SF

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief    weak function to activate SS gpio pin.
 *
 * @param[in] spi     spi device.
 *
 * @return    status  error code.
 */
#pragma weak spi_cs_activate
int32_t spi_cs_activate(MQX_FILE_PTR spifd)
{
    return 0;
}

/*!
 * @brief    weak function to de-activate SS gpio pin.
 *
 * @param[in] spi     spi device.
 *
 * @return    status  error code.
 */
#pragma weak spi_cs_deactivate
int32_t spi_cs_deactivate(MQX_FILE_PTR spifd)
{
    return 0;
}

/*!
 * @brief    weak function to initialize SS gpio pin.
 *
 * @param[in] bus     spi bus index.
 *
 * @return    status  error code.
 */
#pragma weak spi_cs_gpio_init
int32_t spi_cs_gpio_init(uint32_t bus)
{
    return 0;
}

/*!
 * @brief    Convert address to command array.
 *
 * @param[in] addr     read or write address.
 * @param[out] cmd     command array.
 */
static void spi_flash_addr2cmd(uint32_t addr, uint8_t *cmd)
{
    /* Don't need to convert cmd[0], which is actual command */
    cmd[1] = addr >> 16;
    cmd[2] = addr >> 8;
    cmd[3] = addr >> 0;
}

/*!
 * @brief    Common funcion to read and write a spi flash.
 *
 * @param[in] instance     spi instance index.
 * @param[in] spi          spi slave device.
 * @param[in] cmd          command that will be sent to flash.
 * @param[in] cmd_len      Length of command.
 * @param[out] data_out    Output data buffer.
 * @param[in] data_in      input data buffer.
 * @param[in] data_len     data length.
 *
 * @return    status       error code.
 */
static int32_t spi_flash_common_read_write(MQX_FILE_PTR spifd,
                uint8_t *cmd, size_t cmd_len,
                uint8_t *data_out, uint8_t *data_in,
                size_t data_len)
{
    _mqx_int ret;
    int32_t   ret_val = 0;

    spi_cs_activate(spifd);

    ret = fwrite(cmd, 1, cmd_len, spifd);
    if (ret != cmd_len)
    {
        fprintf(stderr, "SF: Failed to send command (%zu bytes): %d\r\n",
                cmd_len, ret);
        ret_val = 1;
    }
    else if (data_len != 0)
    {
        if (data_in)
        {
            ret = fread(data_in, 1, data_len, spifd);
            if (ret != data_len)
            {
                fprintf(stderr, "SF: Failed to transfer %u bytes of data: %d\r\n",
                        data_len, ret);
                ret_val = 1;
            }
	    }

	    if (data_out)
        {
	        ret = fwrite(data_out, 1, data_len, spifd);
	        if (ret != data_len)
            {
                fprintf(stderr, "SF: Failed to transfer %u bytes of data: %d\r\n",
                        data_len, ret);
                ret_val = 1;
	        }
	    }
    }
    fflush(spifd);

    spi_cs_deactivate(spifd);

    return ret_val;
}

/*!
 * @brief    Common funcion to send a command to spi flash.
 *
 * @param[in] instance     spi instance index.
 * @param[in] spi          spi slave device.
 * @param[in] cmd          command that will be sent to flash.
 * @param[out] response    Output response data.
 * @param[in] len          response data length.
 *
 * @return    status       error code.
 */
int32_t spi_flash_common_send_cmd(MQX_FILE_PTR spifd, uint8_t cmd, void *response, size_t len)
{
    return spi_flash_common_read(spifd, &cmd, 1, response, len);
}

/*!
 * @brief    Common funcion to read data from a spi flash.
 *
 * @param[in] instance     spi instance index.
 * @param[in] spi          spi slave device.
 * @param[in] cmd          command that will be sent to flash.
 * @param[in] cmd_len      Length of command.
 * @param[out] data        Output data buffer.
 * @param[in] data_len     data length.
 *
 * @return    status       error code.
 */
int32_t spi_flash_common_read(MQX_FILE_PTR spifd, const uint8_t *cmd,
                          size_t cmd_len, uint8_t *data, size_t data_len)
{
    return spi_flash_common_read_write(spifd, (uint8_t *)cmd, cmd_len, NULL, data, data_len);
}

/*!
 * @brief    Common funcion to write a page of data to a spi flash.
 *
 * @param[in] instance     spi instance index.
 * @param[in] spi          spi slave device.
 * @param[in] cmd          command that will be sent to flash.
 * @param[in] cmd_len      Length of command.
 * @param[in] data         input data buffer.
 * @param[in] data_len     data length.
 *
 * @return    status       error code.
 */
int32_t spi_flash_common_write(MQX_FILE_PTR spifd, const uint8_t *cmd, size_t cmd_len,
        const uint8_t *data, size_t data_len)
{
    return spi_flash_common_read_write(spifd, (uint8_t *)cmd, cmd_len, (uint8_t *)data, NULL, data_len);
}

/*!
 * @brief    Common funcion to write multiply page of data to a spi flash.
 *
 * @param[in] flash        spi flash structure.
 * @param[in] offset       offset in spi flash.
 * @param[in] len          data length.
 * @param[in] buf          input data buffer.
 *
 * @return    status       error code.
 */
static int32_t spi_flash_common_write_multi_sectors(struct spi_flash *flash, uint32_t offset, size_t len, const void *buf)
{
    uint32_t page_addr, byte_addr, page_size;
    size_t chunk_len, actual;
    int32_t ret = 0;
    uint8_t cmd[4];

    page_size = flash->page_size;
    page_addr = offset / page_size;
    byte_addr = offset % page_size;

    cmd[0] = CMD_PAGE_PROGRAM;

    if (_mutex_lock(&flash->MUTEX) != MQX_OK)
         fprintf(stderr, "Mutex lock failed.\n");

    for (actual = 0; actual < len; actual += chunk_len)
    {
        chunk_len = min(len - actual, page_size - byte_addr);

        cmd[1] = page_addr >> 8;
        cmd[2] = page_addr;
        cmd[3] = byte_addr;

#ifdef DEBUG_SF
        fprintf(stderr, "PP: 0x%p => cmd = { 0x%x 0x%x 0x%x 0x%x } chunk_len = %u\r\n",
              (buf + actual), cmd[0], cmd[1], cmd[2], cmd[3], chunk_len);
#endif

        ret = spi_flash_common_write_enable(flash->spifd);
        if (ret < 0)
        {
            fprintf(stderr, "SF: Fail to enable write\r\n");
        }
	    else
	    {
            ret = spi_flash_common_write(flash->spifd, cmd, 4,
                          ((uint8_t *)buf + actual), chunk_len);
            if (ret)
            {
                fprintf(stderr, "SF: write failed\r\n");
            }
	        else
	        {
                ret = spi_flash_common_wait_cmd_ready(flash->spifd, SPI_FLASH_PROG_TIMEOUT);
                if (!ret)
                {
                    byte_addr += chunk_len;
                    if (byte_addr == page_size)
                    {
                        page_addr++;
                        byte_addr = 0;
                    }
                }
            }
        }
    }

    if (_mutex_unlock(&flash->MUTEX) != MQX_OK)
        fprintf(stderr, "Mutex unlock failed.\n");

#ifdef DEBUG_SF
    fprintf(stdout, "SF: program %s %u bytes @ 0x%x\r\n",
          ret ? "failure" : "success", len, offset);
#endif

    return ret ? 0 : len;
}

/*!
 * @brief    Common funcion to read data from a spi flash.
 *
 * @param[in] flash        spi flash structure.
 * @param[in] offset       offset in spi flash.
 * @param[in] len          data length.
 * @param[in] buf          output data buffer.
 *
 * @return    status       error code.
 */
static int32_t spi_flash_common_fast_read(struct spi_flash *flash, uint32_t offset, size_t len, void *data)
{
    uint8_t cmd[5];
    int32_t ret;

    cmd[0] = CMD_READ_ARRAY_FAST;
    spi_flash_addr2cmd(offset, cmd);
    cmd[4] = 0x00;

    if (_mutex_lock(&flash->MUTEX) != MQX_OK)
        fprintf(stderr, "Mutex lock failed.\n");

    ret = spi_flash_common_read(flash->spifd, cmd, sizeof(cmd), data, len);

    if (_mutex_unlock(&flash->MUTEX) != MQX_OK)
        fprintf(stderr, "Mutex unlock failed.\n");

#ifdef DEBUG_SF
    fprintf(stderr, "SF: read %s %u bytes @ 0x%x\r\n",
           (ret) ? "failure" : "success", datalen, offset);
#endif

    return ret ? 0 : len;
}

/*!
 * @brief    Send a command to the device and wait for some bit to clear itself.
 *
 * @param[in] flash        spi flash structure.
 * @param[in] timeout      timeout duration.
 * @param[in] cmd          status command.
 * @param[in] sts_bit     status bit that need to be checked.
 *
 * @return    status       error code.
 */
int32_t spi_flash_common_check_status(MQX_FILE_PTR spifd, uint32_t timeout,
               uint8_t cmd, uint8_t sts_bit)
{
    int ret;
    uint8_t status = 0xff;
    uint32_t i = 0;

    spi_cs_activate(spifd);

    ret = fwrite(&cmd, 1, 1, spifd);
    if (ret != 1)
    {
        fprintf(stderr, "SF: Failed to send command %02x: %d\r\n", cmd, ret);
        return ret;
    }

    do {
        ret = fread(&status, 1, 1, spifd);
        if (ret != 1)
        {
            return -1;
        }

        if ((status & sts_bit) == 0)
        {
            break;
        }
	    _time_delay(1);
    } while (i < timeout);

    fflush(spifd);

    spi_cs_deactivate(spifd);

    if ((status & sts_bit) == 0)
    {
        return 0;
    }

    /* Timed out */
    fprintf(stderr, "SF: time out!\r\n");
    return -1;
}

/*!
 * @brief    Send the read status command to the device and wait for the wip (write-in-progress) bit to clear itself.
 *
 * @param[in] flash        spi flash structure.
 * @param[in] timeout      timeout duration.
 *
 * @return    status       error code.
 */
int32_t spi_flash_common_wait_cmd_ready(MQX_FILE_PTR spifd, uint32_t timeout)
{
    return spi_flash_common_check_status(spifd, timeout,
        CMD_READ_STATUS, STATUS_WIP);
}

/*!
 * @brief    Common funcion erase blocks in a spi flash.
 *
 * @param[in] flash        spi flash structure.
 * @param[in] offset       offset in spi flash to erase from.
 * @param[in] len          length in spi flash to be erased.
 *
 * @return    status       error code.
 */
static int32_t spi_flash_common_erase(struct spi_flash *flash, uint32_t offset, size_t len)
{
    uint32_t start, end, erase_size;
    int ret = 0;
    uint8_t cmd[4];

    erase_size = flash->sector_size;
    if (offset % erase_size || len % erase_size) {
        fprintf(stderr, "SF: Please use another erase size\r\n"
               "As Erase offset/length should be multiple of sector size(%d)\n",
               flash->sector_size);
        return -1;
    }

    if (erase_size == 4096)
        cmd[0] = CMD_ERASE_4K;
    else
        cmd[0] = CMD_ERASE_64K;
    start = offset;
    end = start + len;

    if (_mutex_lock(&flash->MUTEX) != MQX_OK)
         printf("Mutex lock failed.\n");

    while (offset < end) {
        spi_flash_addr2cmd(offset, cmd);
        offset += erase_size;

#ifdef DEBUG_SF
        fprintf(stderr, "SF: erase %x %x %x %x (%x)\r\n", cmd[0], cmd[1],
        		cmd[2], cmd[3], block);
#endif

        ret = spi_flash_common_write_enable(flash->spifd);
        if (!ret)
        {
            ret = spi_flash_common_write(flash->spifd, cmd, sizeof(cmd), NULL, 0);
            if (!ret) {
                ret = spi_flash_common_wait_cmd_ready(flash->spifd, SPI_FLASH_PAGE_ERASE_TIMEOUT);
                if (ret) {
                    fprintf(stderr, "Wait ready failed!\n");
                }
            }
        }
    }

    _mutex_unlock(&flash->MUTEX);

#ifdef DEBUG_SF
    fprintf(stdout, "SF: erase %s @block %d\r\n",
           (ret) ? "failure" : "success", block);
#endif

    return ret;
}

/*!
 * @brief    Program the status register.
 *
 * @param[in] flash        spi flash instance.
 * @param[in] sr           value that will be set to status register.
 *
 * @return    status       error code.
 */
int32_t spi_flash_common_write_status(MQX_FILE_PTR spifd, uint8_t sts_val)
{
    uint8_t cmd;
    int32_t ret;

    ret = spi_flash_common_write_enable(spifd);
    if (ret < 0)
    {
        fprintf(stderr, "SF: Failed to enable write\r\n");
        return ret;
    }

    cmd = CMD_WRITE_STATUS;
    ret = spi_flash_common_write(spifd, &cmd, 1, &sts_val, 1);
    if (ret)
    {
        fprintf(stderr, "SF: fail to write status register\r\n");
        return ret;
    }

    ret = spi_flash_common_wait_cmd_ready(spifd, SPI_FLASH_PROG_TIMEOUT);
    if (ret < 0)
    {
        fprintf(stderr, "SF: write status register timed out\r\n");
        return ret;
    }

    return 0;
}

#define IDCODE_CONT_LEN 0
#define IDCODE_PART_LEN 5

#define SPI_FLASH_WINBOND_VENDER_ID   0xef

#define IDCODE_LEN (IDCODE_CONT_LEN + IDCODE_PART_LEN)

static MQX_FILE_PTR spi_flash_setup_spi(const char *spi_channel, uint32_t max_hz, uint32_t mode)
{
    MQX_FILE_PTR spifd;
    uint32_t param;
    
    /* Open the SPI driver */
    spifd = fopen(spi_channel, NULL);
    if (NULL == spifd) {
        fprintf (stderr, "spiflash_hwinit: failed to open SPI driver!\n");
        goto fail;
    }

    /* Set a different rate */
    if (SPI_OK != ioctl (spifd, IO_IOCTL_SPI_SET_BAUD, &max_hz))
    {
        fprintf(stderr, "spiflash_hwinit: failed to set band rate\n");
        goto fail;
    }

    /* Set clock mode */
    if (SPI_OK != ioctl(spifd, IO_IOCTL_SPI_SET_MODE, &mode))
    {
        fprintf(stderr, "spiflash_hwinit: failed to set clock mode\n");
        goto fail;
    }

    /* Set transfer mode */
    param = SPI_DEVICE_MASTER_MODE;
    if (SPI_OK != ioctl(spifd, IO_IOCTL_SPI_SET_TRANSFER_MODE, &param))
    {
        fprintf (stderr, "spiflash_hwinit: failed to set transfer mode\n");
        goto fail;
    }
    
    return spifd;
  
fail:
    if(spifd)
         fclose(spifd);
    
    return NULL;
}

_mqx_int spi_flash_common_init(const void _PTR_ init_data_ptr, pointer *io_info_ptr_ptr)
{
    int ret;
    uint8_t idcode[IDCODE_LEN];
    MQX_FILE_PTR spifd;
    struct spi_flash *flash = NULL;
    SPI_FLASH_INIT_STRUCT_PTR sf_init_ptr = (SPI_FLASH_INIT_STRUCT_PTR)init_data_ptr;
    MUTEX_ATTR_STRUCT mutexattr;

    /* Open the SPI driver */
    spifd = spi_flash_setup_spi(sf_init_ptr->spidev, sf_init_ptr->PARAMS.BAUDRATE, sf_init_ptr->PARAMS.MODE);
    if (!spifd) {
        return -1;
    }

    /* Send CMD_READ_ID to get flash chip ID codes */
    ret = spi_flash_common_send_cmd(spifd, CMD_READ_ID, &idcode, sizeof(idcode));
    if (ret)
    {
        return -1;
    }

#ifdef DEBUG_SF
    fprintf(stderr, "SF: Got idcode %x %x %x %x %x\r\n",
        idcode[0], idcode[1], idcode[2], idcode[3], idcode[4]);
#endif

    flash = (struct spi_flash *)_mem_alloc_system_zero(sizeof(struct spi_flash));
    if (!flash) {
        fprintf(stderr, "SF: Failed to allocate memory\n");
        return -1;
    }
	memset(flash, 0, sizeof(struct spi_flash));
    if (SPI_FLASH_WINBOND_VENDER_ID == idcode[0]) {
        if (spi_flash_winbond_init(spifd, flash, idcode)) {
            fprintf(stderr, "SF: Unsupported manufacturer %02x\r\n", idcode[0]);
            return -1;
        }
    }
    else
    {
        fprintf(stderr, "Can't find spi flash chip!\r\n");
        return -1;
    }

    if (!flash->read)
        flash->read = spi_flash_common_fast_read;
    if (!flash->write)
        flash->write = spi_flash_common_write_multi_sectors;
    if (!flash->erase)
        flash->erase = spi_flash_common_erase;

    flash->spifd = spifd;
    *io_info_ptr_ptr = flash;

    /* Initialize mutex attributes */
    if (_mutatr_init(&mutexattr) != MQX_OK)
      fprintf(stderr, "Initialize mutex attributes failed.\n");

    /* Initialize the mutex */ 
    if (_mutex_init(&flash->MUTEX, &mutexattr) != MQX_OK)
       fprintf(stderr, "Initialize print mutex failed.\n");

//#ifdef DEBUG_SF
    fprintf(stdout, "SF: Detected %s with page size %d, total %d\r\n",
           flash->name, (int)flash->sector_size, (int)flash->size);
//#endif
    return 0;
}

/*!
 * @brief    free a spi flash device.
 *
 * @param[in] flash        spi flash instance.
 *
 * @return    void
 */
_mqx_int spi_flash_common_free(pointer io_info_ptr)
{
    struct spi_flash *flash = (struct spi_flash *)io_info_ptr;

    if (_mutex_destroy(&flash->MUTEX) != MQX_OK)
        fprintf(stderr, "Error - unable to destroy MUTEX.\n");

    if (NULL != io_info_ptr)
        _mem_free(io_info_ptr);
    
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
