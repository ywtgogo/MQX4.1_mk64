#ifndef __SPIFLASH_CONFIG_H__
#define __SPIFLASH_CONFIG_H__
/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
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
* $FileName: spiflash_config.h$
* $Version : 1.0.0$
* $Date    : Sep-3-2013$
*
*
*END************************************************************************/

#include "nftl.h"

#ifndef BSP_SPI_MEMORY_CHANNEL
	#error This application requires BSP_SPI_MEMORY_CHANNEL to be defined. Please set it to appropriate SPI channel number in user_config.h and recompile BSP with this option.
#endif

#if BSP_SPI_MEMORY_CHANNEL == 0

    #if ! BSPCFG_ENABLE_SPI0
        #error This application requires BSPCFG_ENABLE_SPI0 defined non-zero in user_config.h. Please recompile kernel with this option.
    #else
        #define SPINOR_CHANNEL "spi0:"
    #endif

#elif BSP_SPI_MEMORY_CHANNEL == 1

    #if ! BSPCFG_ENABLE_SPI1
        #error This application requires BSPCFG_ENABLE_SPI1 defined non-zero in user_config.h. Please recompile kernel with this option.
    #else
        #define SPINOR_CHANNEL "spi1:"
    #endif

#elif BSP_SPI_MEMORY_CHANNEL == 2

    #if ! BSPCFG_ENABLE_SPI2
        #error This application requires BSPCFG_ENABLE_SPI2 defined non-zero in user_config.h. Please recompile kernel with this option.
    #else
        #define SPINOR_CHANNEL  "spi2:1"
        //#define SPINOR_CHANNEL  "spi2:"
        //#define SPINOR_CHANNEL  "ispi2:"
    #endif

#else

     #error Unsupported SPI channel number. Please check settings of BSP_SPI_MEMORY_CHANNEL in BSP.

#endif


#if !MQX_USE_TIMER
	#error The FFS lib requires MQX_USE_TIMER defined non-zero in user_config.h. Please recompile PSP with this option.
#endif       
       
#define MERGE_BLOCK_LIMIT	5

#define FLASH_W25Q64    1
#define FLASH_S25FL064  2 
#define AT26DF081       3

//////////////////////////////////////////////////
//     W25Q64 FLASH INFO CONFIGURATION          //
//////////////////////////////////////////////////

#define W25Q64_ADDRESS_BYTES             3

#define W25Q64_BLOCKCOUNT                128
#define W25Q64_BLOCKSIZE		 65536
#define W25Q64_BLOCK_SHIFT_NUMBER        16
#define W25Q64_ERASE_SECTORSIZE          4096
#define W25Q64_ERASE_SECTOR_SHIFT_NUMBER 12
#define W25Q64_PAGE_SIZE                 256
#define W25Q64_ID			0xef16

#define W25Q64_WRITE_STATUS_CMD          0x01
#define W25Q64_WRITE_DATA_CMD            0x02
#define W25Q64_READ_DATA_CMD             0x03
#define W25Q64_WRITE_LATCH_DISABLE_CMD   0x04
#define W25Q64_READ_STATUS_CMD           0x05
#define W25Q64_WRITE_LATCH_ENABLE_CMD    0x06
#define W25Q64_CHIP_ERASE_CMD            0xC7
#define W25Q64_SECTOR_ERASE_CMD	         0x20
#define W25Q64_READ_ID_CMD               0x90
#define W25Q64_BLOCK_ERASE_CMD	    	 0xd8


//////////////////////////////////////////////////
//     S25FL064 FLASH INFO CONFIGURATION          //
//////////////////////////////////////////////////

#define S25FL064_ADDRESS_BYTES             3

#define S25FL064_BLOCKCOUNT                64
#define S25FL064_BLOCKSIZE				   65536
#define S25FL064_BLOCK_SHIFT_NUMBER        16
#define S25FL064_ERASE_SECTORSIZE          4096
#define S25FL064_ERASE_SECTOR_SHIFT_NUMBER 12
#define S25FL064_PAGE_SIZE                 256
#define S25FL064_ID						   0x0116

#define S25FL064_WRITE_STATUS_CMD          0x01
#define S25FL064_WRITE_DATA_CMD            0x02
#define S25FL064_READ_DATA_CMD             0x03
#define S25FL064_WRITE_LATCH_DISABLE_CMD   0x04
#define S25FL064_READ_STATUS_CMD           0x05
#define S25FL064_WRITE_LATCH_ENABLE_CMD    0x06
#define S25FL064_CHIP_ERASE_CMD            0xC7
#define S25FL064_SECTOR_ERASE_CMD	       0x20
#define S25FL064_READ_ID_CMD               0x90
#define S25FL064_BLOCK_ERASE_CMD	       0xd8

//////////////////////////////////////////////////
//     AT26DF081A FLASH INFO CONFIGURATION          //
//////////////////////////////////////////////////

#define AT26DF081_ADDRESS_BYTES             3

#define AT26DF081_BLOCKCOUNT                16
#define AT26DF081_BLOCKSIZE		    65536
#define AT26DF081_BLOCK_SHIFT_NUMBER        16
#define AT26DF081_ERASE_SECTORSIZE          4096
#define AT26DF081_ERASE_SECTOR_SHIFT_NUMBER 12
#define AT26DF081_PAGE_SIZE                 256
#define AT26DF081_ID			    0x1f45

#define AT26DF081_WRITE_STATUS_CMD          0x01
#define AT26DF081_WRITE_DATA_CMD            0x02
#define AT26DF081_READ_DATA_CMD             0x03
#define AT26DF081_WRITE_LATCH_DISABLE_CMD   0x04
#define AT26DF081_READ_STATUS_CMD           0x05
#define AT26DF081_WRITE_LATCH_ENABLE_CMD    0x06
#define AT26DF081_CHIP_ERASE_CMD            0xC7
#define AT26DF081_SECTOR_ERASE_CMD	    0x20
#define AT26DF081_READ_ID_CMD               0x9f
#define AT26DF081_BLOCK_ERASE_CMD	    0xd8


////////////////////////////////////////////////////////////////////////////
#define FLASH_FS_START_BLOCK                (16)

#if (BSP_FLASH_DEVICE == FLASH_W25Q64)

    #define FLASH_ADDRESS_BYTES              W25Q64_ADDRESS_BYTES

    #define FLASH_BLOCKCOUNT                 (W25Q64_BLOCKCOUNT - FLASH_FS_START_BLOCK)
    #define FLASH_BLOCKSIZE                  W25Q64_BLOCKSIZE
    #define FLASH_BLOCK_SHIFT_NUMBER         W25Q64_BLOCK_SHIFT_NUMBER
    #define FLASH_ERASE_SECTORSIZE           W25Q64_ERASE_SECTORSIZE
    #define FLASH_ERASE_SECTOR_SHIFT_NUMBER  W25Q64_ERASE_SECTOR_SHIFT_NUMBER
    #define FLASH_PHYSICAL_PAGE_SIZE         W25Q64_PAGE_SIZE
    #define FLASH_ID			     		 W25Q64_ID

    #define FLASH_WRITE_STATUS_CMD           W25Q64_WRITE_STATUS_CMD
    #define FLASH_WRITE_DATA_CMD             W25Q64_WRITE_DATA_CMD
    #define FLASH_READ_DATA_CMD              W25Q64_READ_DATA_CMD
    #define FLASH_WRITE_LATCH_DISABLE_CMD    W25Q64_WRITE_LATCH_DISABLE_CMD
    #define FLASH_READ_STATUS_CMD            W25Q64_READ_STATUS_CMD
    #define FLASH_WRITE_LATCH_ENABLE_CMD     W25Q64_WRITE_LATCH_ENABLE_CMD
    #define FLASH_CHIP_ERASE_CMD             W25Q64_CHIP_ERASE_CMD
    #define FLASH_SECTOR_ERASE_CMD	     W25Q64_SECTOR_ERASE_CMD
    #define FLASH_READ_ID_CMD                W25Q64_READ_ID_CMD
    #define FLASH_BLOCK_ERASE_CMD	     W25Q64_BLOCK_ERASE_CMD

#elif(BSP_FLASH_DEVICE == FLASH_S25FL064)

    #define FLASH_ADDRESS_BYTES              S25FL064_ADDRESS_BYTES

    #define FLASH_BLOCKCOUNT                 S25FL064_BLOCKCOUNT
    #define FLASH_BLOCKSIZE                  S25FL064_BLOCKSIZE
    #define FLASH_BLOCK_SHIFT_NUMBER         S25FL064_BLOCK_SHIFT_NUMBER
    #define FLASH_ERASE_SECTORSIZE           S25FL064_ERASE_SECTORSIZE
    #define FLASH_ERASE_SECTOR_SHIFT_NUMBER  S25FL064_ERASE_SECTOR_SHIFT_NUMBER
    #define FLASH_PHYSICAL_PAGE_SIZE         S25FL064_PAGE_SIZE
    #define FLASH_ID						 S25FL064_ID

    #define FLASH_WRITE_STATUS_CMD           S25FL064_WRITE_STATUS_CMD
    #define FLASH_WRITE_DATA_CMD             S25FL064_WRITE_DATA_CMD
    #define FLASH_READ_DATA_CMD              S25FL064_READ_DATA_CMD
    #define FLASH_WRITE_LATCH_DISABLE_CMD    S25FL064_WRITE_LATCH_DISABLE_CMD
    #define FLASH_READ_STATUS_CMD            S25FL064_READ_STATUS_CMD
    #define FLASH_WRITE_LATCH_ENABLE_CMD     S25FL064_WRITE_LATCH_ENABLE_CMD
    #define FLASH_CHIP_ERASE_CMD             S25FL064_CHIP_ERASE_CMD
    #define FLASH_SECTOR_ERASE_CMD	         S25FL064_SECTOR_ERASE_CMD
    #define FLASH_READ_ID_CMD                S25FL064_READ_ID_CMD
    #define FLASH_BLOCK_ERASE_CMD			 S25FL064_BLOCK_ERASE_CMD

#elif(BSP_FLASH_DEVICE == AT26DF081)

    #define FLASH_ADDRESS_BYTES              AT26DF081_ADDRESS_BYTES

    #define FLASH_BLOCKCOUNT                 AT26DF081_BLOCKCOUNT
    #define FLASH_BLOCKSIZE                  AT26DF081_BLOCKSIZE
    #define FLASH_BLOCK_SHIFT_NUMBER         AT26DF081_BLOCK_SHIFT_NUMBER
    #define FLASH_ERASE_SECTORSIZE           AT26DF081_ERASE_SECTORSIZE
    #define FLASH_ERASE_SECTOR_SHIFT_NUMBER  AT26DF081_ERASE_SECTOR_SHIFT_NUMBER
    #define FLASH_PHYSICAL_PAGE_SIZE         AT26DF081_PAGE_SIZE
    #define FLASH_ID						 AT26DF081_ID

    #define FLASH_WRITE_STATUS_CMD           AT26DF081_WRITE_STATUS_CMD
    #define FLASH_WRITE_DATA_CMD             AT26DF081_WRITE_DATA_CMD
    #define FLASH_READ_DATA_CMD              AT26DF081_READ_DATA_CMD
    #define FLASH_WRITE_LATCH_DISABLE_CMD    AT26DF081_WRITE_LATCH_DISABLE_CMD
    #define FLASH_READ_STATUS_CMD            AT26DF081_READ_STATUS_CMD
    #define FLASH_WRITE_LATCH_ENABLE_CMD     AT26DF081_WRITE_LATCH_ENABLE_CMD
    #define FLASH_CHIP_ERASE_CMD             AT26DF081_CHIP_ERASE_CMD
    #define FLASH_SECTOR_ERASE_CMD	         AT26DF081_SECTOR_ERASE_CMD
    #define FLASH_READ_ID_CMD                AT26DF081_READ_ID_CMD
    #define FLASH_BLOCK_ERASE_CMD			 AT26DF081_BLOCK_ERASE_CMD

#else
	#error please config your SPI-FLASH device in spiflash_config.h
#endif

#define FLASH_FS_START_OFFSET            (FLASH_BLOCKSIZE * FLASH_FS_START_BLOCK)

bool norflash_read_ID_ll (NFTL_DEV_INFO_PTR flash_ptr, MQX_FILE_PTR spifd, uint32_t*id);
bool norflash_chip_erase_ll (NFTL_DEV_INFO_PTR flash_ptr, MQX_FILE_PTR spifd);
bool norflash_block_erase_ll (NFTL_DEV_INFO_PTR flash_ptr, MQX_FILE_PTR spifd, uint32_t addr);
bool norflash_sector_erase_ll (NFTL_DEV_INFO_PTR flash_ptr, MQX_FILE_PTR spifd, uint32_t addr);
bool norflash_read_data_ll (NFTL_DEV_INFO_PTR flash_ptr, MQX_FILE_PTR spifd, uint32_t addr, uint32_t size, char_ptr data);
bool norflash_write_data_ll (NFTL_DEV_INFO_PTR flash_ptr, MQX_FILE_PTR spifd, uint32_t addr, uint32_t size, char_ptr data);
bool norflash_set_protection_ll (MQX_FILE_PTR spifd, bool protect);

#endif 
