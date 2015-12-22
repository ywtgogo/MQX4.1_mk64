#include "global.h"
#include "spi_flash.h"
#include "nftl.h"
#include "update.h"
#include <rtcs.h>
#include <ftpc.h>
#include <string.h>

#define UPDATE_EXIT_SUCCESS   	0
#define UPDATE_EXIT_ERROR      -1

extern void reset_cpu();
extern const SPI_FLASH_INIT_STRUCT _bsp_spi_flash_init;
extern int32_t MFS_get_freesize(MQX_FILE_PTR);

int32_t remoute_ftpget(int32_t argc, char *argv[])
{
	int32_t				return_code		= UPDATE_EXIT_SUCCESS;
	FTPc_CONTEXT_PTR  	ftpc_context_ptr;
	uint32_t           	i;
	_mqx_int          	response;
	MQX_FILE_PTR 		fd;
	char				path[20];
	
   ftpc_context_ptr = _mem_alloc_zero( sizeof( FTPc_CONTEXT_STRUCT ));
   if (ftpc_context_ptr == NULL)  {
      printf("Error, unable to allocate memory for FTP client.\n");
      return IO_ERROR;
   }

	_mem_set_type(ftpc_context_ptr, MEM_TYPE_FTPc_CONTEXT_STRUCT);
	printf("\nRTCS v%d.%02d.%02d FTP client\n", RTCS_VERSION_MAJOR, RTCS_VERSION_MINOR, RTCS_VERSION_REV);
	
	if (argv[0]) {
		ftpc_context_ptr->ARGC = 2;
		ftpc_context_ptr->ARGV[0] = "open";
		ftpc_context_ptr->ARGV[1] = argv[0];
	} else  {
	  	ftpc_context_ptr->ARGC = 0;
		return_code = UPDATE_EXIT_ERROR;
	}
	
	if (ftpc_context_ptr->ARGC > 0) {
		/* FTP_open */		
		_io_strtolower(ftpc_context_ptr->ARGV[0]); 	//有地址传入，接下来将打开该TCP
		response = FTPc_open(ftpc_context_ptr, ftpc_context_ptr->ARGC, ftpc_context_ptr->ARGV);		 
		if (response == RTCS_ERROR)  {
			printf("FTP connection closed.\n");
			response = 0;
			ftpc_context_ptr->HOSTADDR = 0;
			ftpc_context_ptr->HANDLE = NULL;
			return_code = UPDATE_EXIT_ERROR;
		} else  {
			while ((response >= 100) && (response < 200))   {
				response = FTP_receive_message(ftpc_context_ptr->HANDLE,stdout);
			}
		}
		/* FTP_User&Pass */			
		printf("User: %s\r\n", argv[1]);
		sprintf(ftpc_context_ptr->BUFFER,"USER %s\r\n", argv[1] );
		response = FTP_command(ftpc_context_ptr->HANDLE,ftpc_context_ptr->BUFFER,stdout);		
		if ((response >= 300) && (response < 400))  {
			printf("Password: %s\r\n", argv[2] );
			sprintf(ftpc_context_ptr->BUFFER,"PASS %s\r\n", argv[2]);
			response = FTP_command(ftpc_context_ptr->HANDLE,ftpc_context_ptr->BUFFER,stdout);
			if (response == 530) return_code = UPDATE_EXIT_ERROR;
		}
		/* FTP_get */
		printf("Image: %s\r\n", argv[3] );
		snprintf(path, sizeof(path), "%s%s", "a:\\", argv[3]);
		fd = fopen(path, "w");
		if (!fd)  {
		 	printf("Unable to open local file: %s\n",path);
		} else {
			/* 检查SF的剩余空间 */
			printf("SF: Free Size %d Bytes\n", MFS_get_freesize(fd));
			sprintf(ftpc_context_ptr->BUFFER,"RETR %s\r\n", argv[3]);			
			response = FTP_command_data(ftpc_context_ptr->HANDLE, ftpc_context_ptr->BUFFER, stdout, fd, FTPMODE_PORT | FTPDIR_RECV);
			if (response == 550) return_code = UPDATE_EXIT_ERROR;
			fclose(fd);
		}
	}
	/* FTP_bye */	
	if (ftpc_context_ptr->HANDLE != NULL)  {
		response = FTP_close(ftpc_context_ptr->HANDLE,stdout);
		ftpc_context_ptr->HANDLE = NULL;
		ftpc_context_ptr->HOSTADDR = 0;
	}
	ftpc_context_ptr->EXIT=TRUE; 	
	
	printf("FTP terminated\n");
	_mem_free(ftpc_context_ptr);


	return return_code;
}

int32_t remote_update(int32_t argc, char *argv[] )
{
	int32_t				return_code		= UPDATE_EXIT_SUCCESS;
	char              	image_name[20];
	uint32_t          	image_offset 	= DEMOCFG_NEW_IMAGE_OFFSET;
	uint32_t			area_size		= DEMOCFG_NEW_IMAGE_AREA_LEN;
	char              	*buff 			= NULL;
	MQX_FILE_PTR      	fp, sf_fd;
	size_t           	size = 0, actual = 0, chunk_len = 0;
	uint32_t          	sf_erase_param[2] = { 0 };
	
	/* ftp下载升级文件 */
	if(UPDATE_EXIT_ERROR==remoute_ftpget(argc, argv))
	{
		return UPDATE_EXIT_ERROR;
	};
	/* Install image_name */
	snprintf(image_name, sizeof(image_name), "%s%s", "a:\\", argv[3]);
	fp = fopen(image_name, "rb");
	if (NULL == fp) {
		printf("Can't open file %s\n", image_name);
		return UPDATE_EXIT_ERROR; 
	}
	/* Install spi_flash */
	_io_spi_flash_install(DEMOCFG_SPI_FLASH_DEV_NAME, &_bsp_spi_flash_init);
	sf_fd = fopen(DEMOCFG_SPI_FLASH_DEV_NAME, NULL);
	if (!sf_fd) {
		printf("Error open spi flash device!\n");
		fclose(fp);
		return UPDATE_EXIT_ERROR; 
	}
	/* Get file size */
	fseek(fp, 0, IO_SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, IO_SEEK_SET);
	/* Erase image area */
	sf_erase_param[0] = image_offset;
	sf_erase_param[1] = area_size;
	if (ioctl(sf_fd, IO_IOCTL_SPIFLASH_ERASE, sf_erase_param)) {
		printf("Erase image area failed!\r\n");
		return_code = UPDATE_EXIT_ERROR;
		goto err_rtn;
	}
	/* Read from fs Write to spi_flash */
	buff = _mem_alloc_zero(DEMOCFG_UPDATE_BUFFER_SIZE);
	if (NULL == buff) {
		printf("Can't malloc buffer for update!\r\n");
		return_code = UPDATE_EXIT_ERROR;
		goto err_rtn;
	}
	for (actual = 0; actual < size; actual += chunk_len) {
		chunk_len = min(size - actual, DEMOCFG_UPDATE_BUFFER_SIZE);
		if (fseek(fp, actual, IO_SEEK_SET)) {
			printf("Error seek offset in image file!\n");
			return_code = UPDATE_EXIT_ERROR;
			goto err_rtn;
		}
		if (chunk_len != fread(buff, 1, chunk_len, fp)) {
			printf("Error read firmware image data!\n");
			return_code = UPDATE_EXIT_ERROR;
			goto err_rtn;
		}
		if (fseek(sf_fd, image_offset + actual, IO_SEEK_SET)) {
			printf("Error seek offset in spi flash device!\n");
			return_code = UPDATE_EXIT_ERROR;
			goto err_rtn;
		}
		if (chunk_len != fwrite(buff, 1, chunk_len, sf_fd)) {
			printf("Error write firmware to flash!\n");
			return_code = UPDATE_EXIT_ERROR;
			goto err_rtn;
		}
	}
      
	err_rtn:
    fclose(sf_fd);
    fclose(fp);
    _io_dev_uninstall(DEMOCFG_SPI_FLASH_DEV_NAME);
    if (buff)
        _mem_free((pointer)buff);
	if (return_code) {
        printf("Failed to update firmware %s to spi flash @0x%x, size %d\n",
               image_name, image_offset, size);
	}
	else {
        printf("Succeed to update firmware %s to spi flash @0x%x, size %d\n",
               image_name, image_offset, size);
		reset_cpu();
	}
    return return_code;
}
