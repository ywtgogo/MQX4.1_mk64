/**HEADER********************************************************************
*
* Copyright (c) 2012 Freescale Semiconductor;
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
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>

#include <stdlib.h>

#include "spi.h"
#include "spi_prv.h"
#include "spi_flash.h"


/* Forward declarations */
static _mqx_int _io_spi_flash_open(MQX_FILE_PTR fd_ptr, char _PTR_ open_name_ptr, char _PTR_ flags);
static _mqx_int _io_spi_flash_close(MQX_FILE_PTR fd_ptr);
static _mqx_int _io_spi_flash_read(MQX_FILE_PTR fd_ptr, char_ptr data_ptr, _mqx_int n);
static _mqx_int _io_spi_flash_write(MQX_FILE_PTR fd_ptr, char_ptr data_ptr, _mqx_int n);
static _mqx_int _io_spi_flash_ioctl(MQX_FILE_PTR fd_ptr, uint32_t cmd, pointer param_ptr);
static _mqx_int _io_spi_flash_uninstall(IO_DEVICE_STRUCT_PTR io_dev_ptr);


/*FUNCTION****************************************************************
*
* Function Name    : _io_spi_flash_install
* Returned Value   : MQX error code
* Comments         :
*    Installs SPI Flash device.
*
*END**********************************************************************/
_mqx_int _io_spi_flash_install
    (
        /* [IN] A string that identifies the device for fopen */
        char_ptr                       identifier,

        /* [IN] Pointer to driver initialization data */
        SPI_FLASH_INIT_STRUCT_CPTR           init_data_ptr
    )
{
    SPI_FLASH_DRIVER_DATA_STRUCT_PTR driver_data;
    _mqx_int error_code;

    driver_data = (SPI_FLASH_DRIVER_DATA_STRUCT_PTR)_mem_alloc_system_zero((_mem_size)sizeof(SPI_FLASH_DRIVER_DATA_STRUCT));
    if (driver_data == NULL)
    {
        return MQX_OUT_OF_MEMORY;
    }
    _mem_set_type(driver_data, MEM_TYPE_IO_SPI_POLLED_DEVICE_STRUCT);

    driver_data->PARAMS = init_data_ptr->PARAMS;

    /* initialize low level driver */
    error_code = spi_flash_common_init(init_data_ptr, &(driver_data->DEVIF_DATA));

    if (error_code != MQX_OK)
    {
        _mem_free(driver_data);
        return error_code;
    }

    error_code = _io_dev_install_ext(identifier,
        _io_spi_flash_open, _io_spi_flash_close,
        _io_spi_flash_read, _io_spi_flash_write,
        _io_spi_flash_ioctl,
        _io_spi_flash_uninstall,
        (pointer)driver_data);

    if (error_code)
    {
        _mem_free(driver_data);
        return error_code;
    }

    return MQX_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : _io_spi_uninstall
* Returned Value   : MQX error code
* Comments         :
*    Uninstalls SPI device.
*
*END**********************************************************************/
static _mqx_int _io_spi_flash_uninstall
    (
        /* [IN] The IO device structure for the device */
        IO_DEVICE_STRUCT_PTR           io_dev_ptr
    )
{
    SPI_DRIVER_DATA_STRUCT_PTR driver_data;

    _mqx_int error_code = MQX_OK;

    driver_data = (SPI_DRIVER_DATA_STRUCT_PTR)(io_dev_ptr->DRIVER_INIT_PTR);

    /* deinitialize low level driver */
    error_code = spi_flash_common_free(driver_data->DEVIF_DATA);

    if (error_code != MQX_OK)
    {
        return error_code;
    }

    _mem_free(driver_data);

    return MQX_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : _io_spi_flash_open
* Returned Value   : MQX error code
* Comments         :
*    This routine opens the SPI I/O channel.
*
*END**********************************************************************/
static _mqx_int _io_spi_flash_open
    (
        /* [IN] The file handle for the device being opened */
        MQX_FILE_PTR         fd_ptr,

        /* [IN] The remaining portion of the name of the device */
        char                 _PTR_ open_name_ptr,

        /* [IN] The flags to be used during operation */
        char                 _PTR_ flags
    )
{
    SPI_FLASH_DEV_DATA_STRUCT_PTR    dev_data;
    SPI_FLASH_DRIVER_DATA_STRUCT_PTR driver_data;

    char_ptr file_name_ptr;
    char_ptr endptr;

    uint32_t  cs = 0; /* initialization required to avoid compilation warning */

    file_name_ptr = open_name_ptr;
    while (*file_name_ptr && *file_name_ptr++ != ':') {}

    if (*file_name_ptr)
    {
        cs = strtoul(file_name_ptr, &endptr, 0);
        if (*endptr)
        {
            return IO_ERROR_DEVICE_INVALID;
        }
    }

    dev_data = _mem_alloc_system_zero(sizeof(SPI_FLASH_DEV_DATA_STRUCT));
    if (dev_data == NULL )
    {
        return MQX_OUT_OF_MEMORY;
    }
    fd_ptr->DEV_DATA_PTR = dev_data;

    driver_data = (SPI_FLASH_DRIVER_DATA_STRUCT_PTR)(fd_ptr->DEV_PTR->DRIVER_INIT_PTR);

    /* copy default parameters */
    dev_data->PARAMS = driver_data->PARAMS;

    /* optionally assign CS according to file name */
    if (*file_name_ptr)
    {
        dev_data->PARAMS.CS = cs;
    }

    /* open flags */
    dev_data->FLAGS = (uint32_t)flags;

    return MQX_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : _io_spi_close
* Returned Value   : MQX error code
* Comments         :
*    This routine closes the SPI I/O channel.
*
*END**********************************************************************/
static _mqx_int _io_spi_flash_close
    (
        /* [IN] The file handle for the device being closed */
        MQX_FILE_PTR         fd_ptr
    )
{
    if (fd_ptr->DEV_DATA_PTR)
        _mem_free(fd_ptr->DEV_DATA_PTR);

    return MQX_OK;
}


/*FUNCTION****************************************************************
*
* Function Name    : _io_spi_read
* Returned Value   : Number of bytes read
* Comments         :
*    POSIX wrapper to perform SPI transfer storing received data into the buffer.
*    Returns number of bytes received.
*
*END*********************************************************************/
static _mqx_int _io_spi_flash_read
    (
        /* [IN] The handle returned from _fopen */
        MQX_FILE_PTR                   fd_ptr,

        /* [OUT] Where the characters are to be stored */
        char_ptr                       data_ptr,

        /* [IN] The number of bytes to read */
        _mqx_int                       n
    )
{
    SPI_FLASH_DRIVER_DATA_STRUCT_PTR driver_data = (SPI_FLASH_DRIVER_DATA_STRUCT_PTR)fd_ptr->DEV_PTR->DRIVER_INIT_PTR;

    return spi_flash_read(driver_data, fd_ptr->LOCATION, n, data_ptr);
}


/*FUNCTION****************************************************************
*
* Function Name    : _io_spi_write
* Returned Value   : Number of bytes to write
* Comments         :
*    POSIX wrapper to perform SPI transfer sending data from the buffer.
*    Returns number of bytes received.
*
*END**********************************************************************/
static _mqx_int _io_spi_flash_write
    (
        /* [IN] The handle returned from _fopen */
        MQX_FILE_PTR                   fd_ptr,

        /* [IN] Where the bytes are stored */
        char_ptr                       data_ptr,

        /* [IN] The number of bytes to output */
        _mqx_int                       n
    )
{
    SPI_FLASH_DRIVER_DATA_STRUCT_PTR driver_data = (SPI_FLASH_DRIVER_DATA_STRUCT_PTR)fd_ptr->DEV_PTR->DRIVER_INIT_PTR;

    return spi_flash_write(driver_data, fd_ptr->LOCATION, n, data_ptr);
}


/*FUNCTION*****************************************************************
*
* Function Name    : _io_spi_ioctl
* Returned Value   : MQX error code
* Comments         :
*    Returns result of SPI ioctl operation.
*
*END*********************************************************************/
static _mqx_int _io_spi_flash_ioctl
    (
        /* [IN] the file handle for the device */
        MQX_FILE_PTR                   fd_ptr,

        /* [IN] the ioctl command */
        uint32_t                        cmd,

        /* [IN] the ioctl parameters */
        pointer                        param_ptr
    )
{
	  _mqx_int ret = 0;
    switch (cmd) {
    case IO_IOCTL_SEEK:
        break;
    case IO_IOCTL_SPIFLASH_ERASE:
        {
            SPI_FLASH_DRIVER_DATA_STRUCT_PTR driver_data = (SPI_FLASH_DRIVER_DATA_STRUCT_PTR)fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
            uint32_t offset = 0, len = 0;
            uint32_t *erase_param = param_ptr;
            
            offset = erase_param[0];
            len = erase_param[1];

            ret = spi_flash_erase(driver_data, offset, len);
        }
        break;
    default:
        break;
    }

    return ret;
}

/* EOF */
