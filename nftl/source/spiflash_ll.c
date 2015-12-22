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
**************************************************************************
*
* $FileName: spiflash_ll.c$
* $Version : 1.0.0$
* $Date    : Sep-3-2013$
*
* Comments:
*
*   This file contains the functions which write and read the SPI memories
*   using the SPI driver in polled mode.
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "spiflash_config.h"

static int norflash_addr_to_buffer(uint32_t addr, uint8_t *buffer)
{
    int i;

    for (i = FLASH_ADDRESS_BYTES; i; i--)
    {
        buffer[i-1] = (uint8_t)(addr & 0xFF);
        addr >>= 8;
    }

    return FLASH_ADDRESS_BYTES;
}

bool norflash_set_write_latch (MQX_FILE_PTR spifd, bool enable)
{
    _mqx_int result;
    uint8_t buffer[1];

    if (enable)
    {
        buffer[0] = FLASH_WRITE_LATCH_ENABLE_CMD;
    } else {
        buffer[0] = FLASH_WRITE_LATCH_DISABLE_CMD;
    }

    /* Write instruction */
    result = fwrite (buffer, 1, 1, spifd);
    
    /* Wait till transfer end (and deactivate CS) */
    fflush (spifd);

    if(result!=1)
    {
    	printf("norflash_set_write_latch: failed\n");
    	return FALSE;
    }
    
    return TRUE;
}

uint8_t norflash_read_status (MQX_FILE_PTR spifd)
{
    uint32_t result;
    uint8_t buffer[1];
    uint8_t state = 0xFF;

    buffer[0] = FLASH_READ_STATUS_CMD;

    /* Write instruction */
    result = fwrite (buffer, 1, 1, spifd);

    if (result != 1)
    	printf("norflash_read_status:write buffer failed\n");

    /* Read memory status */
    result = fread (&state, 1, 1, spifd);
    
    if(result!=1)
    	printf("norflash_read_status:read buffer failed\n");

    /* Deactivate CS */
    fflush (spifd);

    return state;
}

bool read_ID(MQX_FILE_PTR spifd, uint32_t*id)
{
    _mqx_int result;
    uint8_t buffer[5];
    int len;

    //printf("\nread id: ");
  
    buffer[0] = FLASH_READ_ID_CMD;
 
    norflash_addr_to_buffer(0x00000000, &(buffer[1]));

    if(BSP_FLASH_DEVICE == AT26DF081)
    {
        len = 1 ;//+ FLASH_ADDRESS_BYTES;
    }
    else
    {
        len = 1 + FLASH_ADDRESS_BYTES;
    }

    result = fwrite (buffer, 1, len, spifd);

    if (result != len)
    {
        printf("norflash_read_ID: tx failed\n");
        fflush (spifd);
        return FALSE;
    }
    
    buffer[0] = buffer[1] = buffer[2] = 0xFF;

    len = 2;

    result = fread (buffer, 1, len, spifd);

    /* Deactivate CS */
    fflush (spifd);

    if (result != len)
    {
        printf("norflash_read_ID: rx failed\n");
        return FALSE;
    }

    *id = buffer[1] | (buffer [0] << 8);

    //printf("%x\n", *id);
    return TRUE;
}

bool norflash_read_ID_ll (NFTL_DEV_INFO_PTR flash_ptr, MQX_FILE_PTR spifd, uint32_t*id)
{
    //while(1)
    //    read_ID(spifd,id);
    return 	read_ID(spifd,id);
}				

bool norflash_chip_erase_ll (NFTL_DEV_INFO_PTR flash_ptr, MQX_FILE_PTR spifd)
{
    _mqx_int result;
    uint8_t buffer[1];

    /* This operation must be write-enabled */
    norflash_set_write_latch (spifd, TRUE);

	_time_delay (10);

    //norflash_read_status (spifd);
    while (norflash_read_status (spifd) & 1){}

	
    buffer[0] = FLASH_CHIP_ERASE_CMD;

    /* Write instruction */
    result = fwrite (buffer, 1, 1, spifd);
    if(result!=1)
    {
    	 fflush (spifd);
    	 printf("norflash_chip_erase: tx failed\n");
    	 return FALSE;
    }

    /* Wait till transfer end (and deactivate CS) */
    fflush (spifd);

	_time_delay (10);
    printf ("Erase chip ...");
    while (norflash_read_status (spifd) & 1){
		_time_delay (500);
		printf (".");
	}
	

    norflash_set_write_latch (spifd, FALSE);
    
    if (result != 1)
    {
        printf("norflash_chip_erase:write failed\n");
        return FALSE;
    }
	printf ("OK\n");
    
    return TRUE;
}

bool norflash_block_erase_ll (NFTL_DEV_INFO_PTR flash_ptr, MQX_FILE_PTR spifd, uint32_t addr)
{
    _mqx_int result;
    uint8_t buffer[1 + FLASH_ADDRESS_BYTES];

    addr += flash_ptr->block_start_offset;

    /* This operation must be write-enabled */
    norflash_set_write_latch (spifd, TRUE);

    norflash_read_status (spifd);

    buffer[0] = FLASH_BLOCK_ERASE_CMD;
    norflash_addr_to_buffer(addr, &(buffer[1]));

    /* Write instruction */
    result = fwrite (buffer, 1, 1 + FLASH_ADDRESS_BYTES, spifd);

    /* Wait till transfer end (and deactivate CS) */
    fflush (spifd);

    while (norflash_read_status (spifd) & 1)
    {
        //_time_delay (5);
    }

    if (result != 1 + FLASH_ADDRESS_BYTES)
    {
    	printf("norflash_block_erase: failed\n");
    	return FALSE;
    }

    return TRUE;
}

bool norflash_sector_erase_ll (NFTL_DEV_INFO_PTR flash_ptr, MQX_FILE_PTR spifd, uint32_t addr)
{
    _mqx_int result;
    uint8_t buffer[1 + FLASH_ADDRESS_BYTES];

    addr += flash_ptr->block_start_offset;

    /* This operation must be write-enabled */
    norflash_set_write_latch (spifd, TRUE);

    norflash_read_status (spifd);

    buffer[0] = FLASH_SECTOR_ERASE_CMD;
    norflash_addr_to_buffer(addr, &(buffer[1]));

    /* Write instruction */
    result = fwrite (buffer, 1, 1 + FLASH_ADDRESS_BYTES, spifd);

    /* Wait till transfer end (and deactivate CS) */
    fflush (spifd);

    while (norflash_read_status (spifd) & 1)
    {
        _time_delay (5);
    }

    if (result != 1 + FLASH_ADDRESS_BYTES)
    {
    	printf("norflash_block_erase: failed\n");
    	return FALSE;
    }

    return TRUE;
}



bool norflash_read_data_ll (NFTL_DEV_INFO_PTR flash_ptr, MQX_FILE_PTR spifd, uint32_t addr, uint32_t size, char_ptr data)
{
    _mqx_int result;
    uint8_t buffer[5];

    addr += flash_ptr->block_start_offset;

    /* Read instruction, address */
    buffer[0] = FLASH_READ_DATA_CMD;
    norflash_addr_to_buffer(addr, &(buffer[1]));

    result = fwrite (buffer, 1, 1 + FLASH_ADDRESS_BYTES, spifd);

    if (result != 1 + FLASH_ADDRESS_BYTES)
    {
        /* Stop transfer */
        fflush (spifd);
        printf("norflash_read_data: tx failed\n");
        return FALSE;
    }

    /* Read size bytes of data */
    result = fread (data, 1, (_mqx_int)size, spifd);

    /* De-assert CS */
    fflush (spifd);

    if (result != size)
    {
        printf ("norflash_read_data: rx failed\n");
        return FALSE;
    }

    return TRUE;
}

bool norflash_write_data_ll (NFTL_DEV_INFO_PTR flash_ptr, MQX_FILE_PTR spifd, uint32_t addr, uint32_t size, char_ptr data)
{
    _mqx_int result;
    uint32_t len;
    uint32_t count = size;
    uint8_t buffer[1 + FLASH_ADDRESS_BYTES];

    addr += flash_ptr->block_start_offset;

    while (count > 0)
    {
       /* Each write operation must be enabled in memory */
       norflash_set_write_latch (spifd, TRUE);

       while((norflash_read_status (spifd) & 0x02)==0){}

       len = count;
       if (len > FLASH_PHYSICAL_PAGE_SIZE - (addr & (FLASH_PHYSICAL_PAGE_SIZE - 1))) 
       	len = FLASH_PHYSICAL_PAGE_SIZE - (addr & (FLASH_PHYSICAL_PAGE_SIZE - 1));
       count -= len;

       /* Write instruction, address */
       buffer[0] = FLASH_WRITE_DATA_CMD;
       norflash_addr_to_buffer(addr, &(buffer[1]));

       result = fwrite (buffer, 1, 1 + FLASH_ADDRESS_BYTES, spifd);

       if (result != 1 + FLASH_ADDRESS_BYTES)
       {
           /* Stop transfer */
           fflush (spifd);
           printf("norflash_write_data: tx_addr failed\n");
           return FALSE;
       }

       /* Write data */
       result = fwrite (data, 1, (long)len, spifd);

       /* Deactivate CS */
       fflush (spifd);

       if (result != len)
       {
           printf("norflash_write_data: tx_data failed\n");
           return FALSE;
       }

	
		while(norflash_read_status (spifd)&0x1)
		{
			//_time_delay (1);
		}
		
       /* Move to next block */
       addr += len;
       data += len;

    }

    return TRUE;
}

bool norflash_set_protection_ll (MQX_FILE_PTR spifd, bool protect)
{
    _mqx_int result, i;
    uint8_t protection;
    uint8_t buffer[2];

    /* Must do it twice to ensure right transitions in protection status register */
    for (i = 0; i < 2; i++)
    {
        /* Each write operation must be enabled in memory */
        norflash_set_write_latch (spifd, TRUE);

        norflash_read_status (spifd);

        if (protect)
        {
            protection = 0xFF;
        } else {
            protection = 0x00;
        }

        buffer[0] = FLASH_WRITE_STATUS_CMD;
        buffer[1] = protection;

        /* Write instruction */
        result = fwrite (buffer, 1, 2, spifd);

        /* Wait till transfer end (and deactivate CS) */
        fflush (spifd);
				
        if (result != 2)
        {
            printf("norflash_set_protection:falied\n");
            return FALSE;
        }
    }
    
    return TRUE;
}
