/**HEADER***********************************************************************
*
* Copyright (c) 2010 Freescale Semiconductor;
* All Rights Reserved
*
********************************************************************************
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
********************************************************************************
*
* $FileName: ntfl_spiflash.c$
* $Version : 1.0.0$
* $Date    : Sep-3-2013$
*
* Comments:
*
*   The file contains functions to interface SPI-NOR Flash Controller module.
*
*END***************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "spi.h"
#include "nftl.h"
#include "spiflash_config.h"
#include "spiflash_api.h"

#define VERIFY_WRITE	0

//LWSEM_STRUCT flash_op_lsem;


bool spiflash_hwinit(void);

const NFTL_IO_INFO bsp_spiflash_info=
{
	/* init */             spiflash_init,
	/* deinit */           spiflash_deinit,
	/* chip_erase */       spiflash_erasechip,
	/* block_erase */      spiflash_eraseblock,
	/* sector_erase */     spiflash_erasesector,
	/* byte_read */        spiflash_byteread,
	/* byte_write */       spiflash_bytewrite,
	/* write_protect */    NULL,//spiflash_writeprotect,
	/* read_flashid */     spiflash_readid,
	/* ioctl */            spiflash_ioctl,
	/* block_count */      FLASH_BLOCKCOUNT,
	/* erase_sector_size*/ FLASH_ERASE_SECTORSIZE,
	/* block_size */       FLASH_BLOCKSIZE,
	/* erase_sector_shift_number*/ FLASH_ERASE_SECTOR_SHIFT_NUMBER,
	/* block_shift_number*/FLASH_BLOCK_SHIFT_NUMBER,
	/* merge_blocks_limit*/ MERGE_BLOCK_LIMIT,
    /* block start offset */ FLASH_FS_START_OFFSET
};

static MQX_FILE_PTR spifd = NULL;

bool spiflash_hwinit(void)
{
    uint32_t param;

    /* Open the SPI driver */
    spifd = fopen (SPINOR_CHANNEL, NULL);
    if (NULL == spifd){
        printf ("spiflash_hwinit: failed to open SPI driver!\n");
        goto fail;
    }

    /* Set a different rate */
    param =20000000;//20000000
    if (SPI_OK != ioctl (spifd, IO_IOCTL_SPI_SET_BAUD, &param))
    {
        printf ("spiflash_hwinit: failed to set band rate\n");
        goto fail;
    }

    /* Set clock mode */
    param = SPI_CLK_POL_PHA_MODE0;
    if (SPI_OK != ioctl (spifd, IO_IOCTL_SPI_SET_MODE, &param))
    {
        printf ("spiflash_hwinit: failed to set clock mode\n");
        goto fail;
    }

    /* Set transfer mode */
    param = SPI_DEVICE_MASTER_MODE;
    if (SPI_OK != ioctl (spifd, IO_IOCTL_SPI_SET_TRANSFER_MODE, &param))
    {
        printf ("spiflash_hwinit: failed to set transfer mode\n");
        goto fail;
    }
    
    if(BSP_FLASH_DEVICE == AT26DF081)
    {
        norflash_set_protection_ll (spifd, FALSE);
    }
    
    return TRUE;

fail:
	if(spifd)
	{
        fclose(spifd);
        spifd=NULL;
    }
    
    return FALSE;
}

void spiflash_shutdown(void)
{
	if(spifd)
	{
		fclose(spifd);
		spifd=NULL;
		//_bsp_power_spinor(FALSE);
	}
}     

uint32_t spiflash_init(NFTL_DEV_INFO_PTR flash_ptr)
{ 
   	uint32_t dev_id;
   
   	if(spifd == NULL)
   	{
		if(!spiflash_hwinit())
			return FLASH_INIT_ERR;
   	}
    
    if(!norflash_read_ID_ll(flash_ptr, spifd, &dev_id))
    {
        printf ("spiflash_init: failed to read id\n");
        spiflash_shutdown();
        return FLASH_INIT_ERR;
    }    	
    
    if(dev_id!=FLASH_ID)
    {
        printf ("spiflash_init: incorrect spi-nor device:id=0x%x\n",dev_id);
        spiflash_shutdown();
        return FLASH_INIT_ERR;
    }   	
  	
  	//_lwsem_create(&flash_op_lsem, 1);
  	
	return FLASH_OK;
}

void spiflash_deinit(NFTL_DEV_INFO_PTR flash_ptr)
{
	
	spiflash_shutdown();
	
	//_lwsem_destroy(&flash_op_lsem);
} 

uint32_t spiflash_readid(NFTL_DEV_INFO_PTR flash_ptr, uint32_t* idtable)
{
	uint32_t dev_id;
		
	if(spifd == NULL)
		return FLASH_NODEV_ERR;
		
	_lwsem_wait(&flash_ptr->lwsem);
	if(!norflash_read_ID_ll(flash_ptr, spifd, &dev_id))
	{
		_lwsem_post(&flash_ptr->lwsem);
		return FLASH_READ_ERR;
	}
		
	_lwsem_post(&flash_ptr->lwsem);
	*(uint32_t*)idtable = dev_id;

	return FLASH_OK;
}

uint32_t spiflash_erasechip(NFTL_DEV_INFO_PTR flash_ptr)
{
	bool result,isopened;
	uint32_t dev_id;

   	_lwsem_wait(&flash_ptr->lwsem);
   			
   	if(spifd == NULL)
   	{
   		isopened = FALSE;
   		if(!spiflash_hwinit())
   		{
			_lwsem_post(&flash_ptr->lwsem);
			return FLASH_NODEV_ERR;   				
   		}
   	}	
   	else
   	{
   		isopened = TRUE;
   	}

	norflash_read_ID_ll(flash_ptr, spifd, &dev_id);
	
	result = norflash_chip_erase_ll (flash_ptr, spifd);

	if(!isopened)
		spiflash_shutdown();
			
	_lwsem_post(&flash_ptr->lwsem);
		
	if(result)
		return FLASH_OK;
	else
		return FLASH_ERASE_ERR;
} 

uint32_t spiflash_eraseblock(NFTL_DEV_INFO_PTR flash_ptr,uint32_t block_number)
{
	bool result;
	uint32_t blockaddr = (block_number << flash_ptr->block_shift_number);

	//printf("spiflash_eraseblock: block_number=%d\n",block_number);
	
   	if(spifd == NULL)
   	{
   		return FLASH_NODEV_ERR;
   	}	
		
	_lwsem_wait(&flash_ptr->lwsem);
		
	result = norflash_block_erase_ll(flash_ptr, spifd, blockaddr);
		
	_lwsem_post(&flash_ptr->lwsem);
		
	if(result)
		return FLASH_OK;
	else
		return FLASH_ERASE_ERR;
}

uint32_t spiflash_erasesector(NFTL_DEV_INFO_PTR flash_ptr,uint32_t sector_number)
{
	bool result;
	uint32_t sectoraddr = (sector_number << flash_ptr->erase_sector_shift_number);
		
	//printf("spiflash_erasesector: sector_number=%d\n",sector_number);
   	if(spifd == NULL)
   	{
   		return FLASH_NODEV_ERR;
   	}	
		
	_lwsem_wait(&flash_ptr->lwsem);
		
	result = norflash_sector_erase_ll(flash_ptr, spifd, sectoraddr);
		
	_lwsem_post(&flash_ptr->lwsem);
		
	if(result)
		return FLASH_OK;
	else
		return FLASH_ERASE_ERR;
}

uint32_t spiflash_writeprotect(NFTL_DEV_INFO_PTR flash_ptr, bool protect)
{
	uint32_t result;
	
	//printf("spiflash_writeprotect: protect=%d\n",protect);	
   	if(spifd == NULL)
   	{
		return FLASH_NODEV_ERR;
   	}	
   		
   	_lwsem_wait(&flash_ptr->lwsem);
   	
	result = norflash_set_protection_ll(spifd, protect);
		
	_lwsem_post(&flash_ptr->lwsem);
		
	return result;
}

uint32_t spiflash_byteread(NFTL_DEV_INFO_PTR flash_ptr,uint32_t address, char_ptr buffer_to, uint32_t count)
{
	uint32_t result;
		
	_lwsem_wait(&flash_ptr->lwsem);
		
	result = norflash_read_data_ll(flash_ptr, spifd, address, count, buffer_to);
		
	_lwsem_post(&flash_ptr->lwsem);
		
	if(result)
		return FLASH_OK;
	else
		return FLASH_READ_ERR;
}
		
uint32_t spiflash_bytewrite(NFTL_DEV_INFO_PTR flash_ptr,uint32_t address,char_ptr buffer_from, uint32_t count)
{
	uint32_t result;
		
	//printf("spiflash_bytewrite: logical_pageber=%d\n",logical_pageber);
	if(spifd == NULL)
   	{
		return FLASH_NODEV_ERR;
   	}	
   	
	if (flash_ptr->write_protect) 
	{
		(*flash_ptr->write_protect)(flash_ptr, FALSE);
	}

	_lwsem_wait(&flash_ptr->lwsem);

	result = norflash_write_data_ll(flash_ptr, spifd, address, count, buffer_from);
#if VERIFY_WRITE	
		{
			char*vbuf;

			vbuf =  _mem_alloc_system((_mem_size) count);
			if(vbuf==NULL)
			{
				printf("spiflash_bytewrite:	spiflash_bytewrite alloc error\n");
				while(1);
			}

			norflash_read_data_ll(flash_ptr, spifd,address,count,vbuf);
			for(i=0;i<count;i++)
			{
				if(buffer_from[i]!=vbuf[i])
				{
					printf("spiflash_bytewrite: write=0x%x, read=0x%x\n",buffer_from[i]&0xff,vbuf[i]&0xff);
					while(1);
				}
			}

			_mem_free((pointer)vbuf);	
		}
#endif
	_lwsem_post(&flash_ptr->lwsem);

	if (flash_ptr->write_protect) {
		(*flash_ptr->write_protect)(flash_ptr, TRUE);
	}	
   
	if(result)
		return FLASH_OK;
	else
		return FLASH_READ_ERR;
}

_mqx_int spiflash_ioctl(NFTL_DEV_INFO_PTR flash_ptr,_mqx_uint cmd, pointer param_ptr)
{
    _mqx_uint result = MQX_OK;

	return result;
}
