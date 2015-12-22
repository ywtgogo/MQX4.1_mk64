#include <mqx.h>
#include <bsp.h>
#include "internal_mem.h"

#if !BSPCFG_ENABLE_FLASHX
#error This application requires BSPCFG_ENABLE_FLASHX defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#define         FLASH_NAME 		"flashx:"

//const unsigned  __code UidMac				0x20000023	;	

SUBNET_PTR			SubNetPtr;
CMD_DELAY_PTR		CmdDelayPtr;
PHUB_INFO_PTR		PhubInfoPtr;
COSMOS_INFO_PTR		CosmosInfoPtr;
COSMOS_TCP_PTR		CosmosTcpPtr;
/*
* 写入mem
* 读取mem到ram，将用指针指向分段内存
* 对每块独立mem块使用独立函数接口
* ram空间为全局变量，在internal_mem.c中初始化赋值
*/
//初始化分区
void flashx_init(void)
{
    MQX_FILE_PTR    flashx;
	uint32_t        ioctl_param;
	
	SubNetPtr 		= _mem_alloc_zero(sizeof(SUBNET_t));
	CmdDelayPtr 	= _mem_alloc_zero(sizeof(CMD_DELAY_t));
	PhubInfoPtr		= _mem_alloc_zero(sizeof(PHUB_INFO_t));
	CosmosInfoPtr	= _mem_alloc_zero(sizeof(COSMOS_INFO_t));
	CosmosTcpPtr	= _mem_alloc_zero(sizeof(COSMOS_TCP_t));
    _int_install_unexpected_isr();

    /* Open the flash device */
    flashx = fopen(FLASH_NAME, NULL);
    if (flashx == NULL) {
        printf("\n Unable to open file %s", FLASH_NAME);
        _task_block();
    } else {
        //printf("\n Flash Internel %s opened", FLASH_NAME);
    }

    /* Get the size of the flash file */
    fseek(flashx, 0, IO_SEEK_END);
    printf("\nSize of the flashx: %d Bytes", ftell(flashx));

//    /* Disable Or Enable sector cache */
//    ioctl(flashx, FLASH_IOCTL_ENABLE_SECTOR_CACHE, NULL);
//    printf("\nFlash sector cache enabled.");

//    /* Unprotecting the the FLASH might be required */
//    ioctl_param = 0;
//    ioctl(flash_file, FLASH_IOCTL_WRITE_PROTECT, &ioctl_param);

    fclose(flashx);	
	flashx == NULL;

	sub_net_read();
	cmd_delay_read();
	phub_info_read();
	cosmos_info_read();
	cosmos_tcp_read();
}

uint32_t flashx_read(char *rbuf, uint32_t addr, uint32_t count )
{	
	MQX_FILE_PTR	fdx=NULL;
	uint32_t        ioctl_param;
	uint32_t		len;
	
	fdx = fopen(FLASH_NAME, NULL);
    if (fdx == NULL) {
        printf("\nUnable to open file %s", FLASH_NAME);
		return 1;
    }
	/* Move Pointer */
	fseek(fdx, addr, IO_SEEK_SET);
	/* Disable sector cache */
	ioctl(fdx, FLASH_IOCTL_ENABLE_SECTOR_CACHE, NULL);

	len = read(fdx, rbuf, count);
    if (count != len) {
        printf("\nERROR! Could not read from flash. Exiting...");
		return 1;
	}
	fclose(fdx);
	fdx = NULL;
	return 0;
}

uint32_t flashx_write(char *wbuf, uint32_t addr, uint32_t count)
{	
	MQX_FILE_PTR	fdx=NULL;
	uint32_t        ioctl_param;
	uint32_t		len;
	fdx = fopen(FLASH_NAME, NULL);	
	fseek(fdx, addr, IO_SEEK_SET);
	ioctl(fdx, FLASH_IOCTL_ENABLE_SECTOR_CACHE, NULL);
    /* Unprotecting the the FLASH might be required */ //FALSE
	ioctl_param = 0;
    ioctl(fdx, FLASH_IOCTL_WRITE_PROTECT, &ioctl_param);		
	len = write(fdx, wbuf, count);
    if (len != count) {
        printf("\nError writing to the file. Error code: %d", _io_ferror(fdx));
    }	
	fclose(fdx);
	fdx = NULL;
	return 0;
}

uint32_t sub_net_read(void)
{
	flashx_read((char *)SubNetPtr, SUBNET_LOCATION, sizeof(SUBNET_t));
	return 0;
}
uint32_t sub_net_write(SUBNET_PTR wbuf, uint32_t count)
{
	flashx_write((char *)wbuf, SUBNET_LOCATION, count);
	/* 临时添加 */
	sub_net_read();
	return 0;
}

uint32_t cmd_delay_read(void)
{
	flashx_read((char *)CmdDelayPtr, COSMOS_CMDDELAY_LOCATION, sizeof(CMD_DELAY_t));
	return 0;
}
uint32_t cmd_delay_write(CMD_DELAY_PTR wbuf, uint32_t count)
{
	flashx_write((char *)wbuf, COSMOS_CMDDELAY_LOCATION, count);
	/* 临时添加 */
	cmd_delay_read();
	return 0;
}
uint32_t phub_info_read(void)
{
	flashx_read((char *)PhubInfoPtr, PHUB_INFO_LOCATION, sizeof(PHUB_INFO_t));
	return 0;
}
uint32_t phub_info_write(PHUB_INFO_PTR wbuf, uint32_t count)
{
	flashx_write((char *)wbuf, PHUB_INFO_LOCATION, count);
	/* 临时添加 */
	phub_info_read();
	return 0;
}

uint32_t cosmos_info_read()
{
	flashx_read((char *)CosmosInfoPtr, COSMOS_INFO_LOCATION, sizeof(COSMOS_INFO_t));
	if (CosmosInfoPtr->InitHead == 0xFFFFFFFF) CosmosInfoPtr->InitHead = 0;
	return 0;
}
uint32_t cosmos_info_write(COSMOS_INFO_PTR wbuf, uint32_t count)
{
	flashx_write((char *)wbuf, COSMOS_INFO_LOCATION, count);
	/* 临时添加 */
	cosmos_info_read();
	return 0;
}
uint32_t cosmos_tcp_read()
{
	flashx_read((char *)CosmosTcpPtr, COSMOS_TCP_LOCATION, sizeof(COSMOS_TCP_t));
	return 0;
}
uint32_t cosmos_tcp_write(COSMOS_TCP_PTR wbuf, uint32_t count)
{
	flashx_write((char *)wbuf, COSMOS_TCP_LOCATION, count);
	return 0;
}
