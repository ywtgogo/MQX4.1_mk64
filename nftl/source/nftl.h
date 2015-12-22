#ifndef __NFTL_H__
#define __NFTL_H__

#include "ioctl.h"

#define NFTL_IOCTL_GET_PHY_PAGE_SIZE     _IO(0x33,0x01)
#define NFTL_IOCTL_GET_BLOCK_SIZE        _IO(0x33,0x02)
#define NFTL_IOCTL_GET_NUM_BLOCKS        _IO(0x33,0x03)
#define NFTL_IOCTL_GET_ERASE_SECTOR_SIZE _IO(0x33,0x04)
#define NFTL_IOCTL_GET_VIRT_PAGE_SIZE    _IO(0x33,0x05)
#define NFTL_IOCTL_ERASE_BLOCK           _IO(0x33,0x06)
#define NFTL_IOCTL_ERASE_SECTOR          _IO(0x33,0x07)
#define NFTL_IOCTL_ERASE_CHIP            _IO(0x33,0x08)
#define NFTL_IOCTL_WRITE_PROTECT         _IO(0x33,0x09)
#define NFTL_IOCTL_GET_FLASHID           _IO(0x33,0x0a)
#define NFTL_IOCTL_SET_BLOCK_START       _IO(0x33,0x0b)

#define IO_SPIFLASH_ATTRIBS  (IO_DEV_ATTR_READ  | IO_DEV_ATTR_SEEK | \
    IO_DEV_ATTR_WRITE | IO_DEV_ATTR_BLOCK_MODE)
    
#define FLASH_OK	      0
#define FLASH_NODEV_ERR 1
#define FLASH_INIT_ERR	2
#define FLASH_READ_ERR	3
#define FLASH_ERASE_ERR	4
#define FLASH_WRITE_ERR 5

#define nftl_assert(cond) { \
        if (!(cond)) {          \
            printf("assertion failed: %s, file %s, line %d \n",#cond,__FILE__,__LINE__); \
            while (1){}; \
        } }


struct _nftl_dev_struct;

typedef struct _nftl_io_struct{
	uint32_t      (*init)(struct _nftl_dev_struct*);
	void         (*deinit)(struct _nftl_dev_struct*);
	uint32_t      (*chip_erase)(struct _nftl_dev_struct*);
	uint32_t      (*block_erase)(struct _nftl_dev_struct*, uint32_t block);
	uint32_t      (*sector_erase)(struct _nftl_dev_struct*, uint32_t sector);
	uint32_t      (*byte_read)(struct _nftl_dev_struct*,uint32_t address, char_ptr buffer_to, uint32_t count);
	uint32_t      (*byte_write)(struct _nftl_dev_struct*,uint32_t address,char_ptr buffer_from, uint32_t count);
	uint32_t      (*write_protect)(struct _nftl_dev_struct*,bool);
	uint32_t      (*read_flashid)(struct _nftl_dev_struct*,uint32_t* idtable);
	_mqx_int     (*ioctl)(struct _nftl_dev_struct*, _mqx_uint, pointer);
	
	uint32_t      block_count;
	uint32_t      erase_sector_size;
	uint32_t      block_size;
	uint32_t      erase_sector_shift_number;
	uint32_t      block_shift_number;
	uint32_t      merge_blocks_limit;
    uint32_t      block_start_offset;

} NFTL_IO_INFO, *NFTL_IO_INFO_PTR;
    
typedef struct _nftl_dev_struct{
	uint32_t      (*init)(struct _nftl_dev_struct*);
	void         (*deinit)(struct _nftl_dev_struct*);
	uint32_t      (*chip_erase)(struct _nftl_dev_struct*);
	uint32_t      (*block_erase)(struct _nftl_dev_struct*, uint32_t block);
	uint32_t      (*sector_erase)(struct _nftl_dev_struct*, uint32_t sector);
	uint32_t      (*byte_read)(struct _nftl_dev_struct*,uint32_t address, char_ptr buffer_to, uint32_t count);
	uint32_t      (*byte_write)(struct _nftl_dev_struct*,uint32_t address,char_ptr buffer_from, uint32_t count);
	uint32_t      (*write_protect)(struct _nftl_dev_struct*,bool);
	uint32_t      (*read_flashid)(struct _nftl_dev_struct*,uint32_t* idtable);
	_mqx_int     (*ioctl)(struct _nftl_dev_struct*, _mqx_uint, pointer);
	
	uint32_t      block_count;
 	uint32_t      erase_sector_size;
	uint32_t      block_size;
	uint32_t      erase_sector_shift_number;
	uint32_t      block_shift_number;
	uint32_t      merge_blocks_limit;
    uint32_t      block_start_offset;
	
	LWSEM_STRUCT lwsem;
	_mqx_uint    count;
	
} NFTL_DEV_INFO, *NFTL_DEV_INFO_PTR;

_mqx_uint nftl_install(NFTL_IO_INFO *flash_info, char* device_id);
_mqx_int nftl_uninstall(IO_DEVICE_STRUCT_PTR   io_dev_ptr);
_mqx_int nftl_open(MQX_FILE_PTR fd_ptr,char_ptr open_name_ptr,char_ptr flags);
_mqx_int nftl_close(MQX_FILE_PTR fd_ptr);
_mqx_int nftl_read(MQX_FILE_PTR fd_ptr,char_ptr   data_ptr,_mqx_int   num);
_mqx_int nftl_write(MQX_FILE_PTR fd_ptr,char_ptr   data_ptr,_mqx_int   num);
_mqx_int nftl_ioctl(MQX_FILE_PTR fd_ptr,_mqx_uint  cmd,pointer    param_ptr);

#endif


