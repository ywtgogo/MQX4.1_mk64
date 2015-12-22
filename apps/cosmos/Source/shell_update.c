#include "global.h"
#include "kwtj_GWII.h"
#include "spi_flash.h"
#include "nftl.h"
#include "update.h"
#include <string.h>
#include <shell.h>

const SPI_FLASH_INIT_STRUCT _bsp_spi_flash_init = {
    DEMOCFG_SPI_FLASH_CHANNEL,
    {                                   /* Default parameters: */
        DEMOCFG_SPI_FLASH_CLK_FREQ,     /* Baudrate */
        DEMOCFG_SPI_FLASH_CLK_MODE,     /* Mode */
        8,                              /* Frame size */
        1,                              /* Chip select */
        DSPI_ATTR_USE_ISR,              /* Attributes */
        0xFFFFFFFF                      /* Dummy pattern */
    }
};

static int32_t print_update_usage(void)
{
    printf("Usage: update <new> <filename>\n");
    //printf("   recovery = Update firmware as recovery firmware\n");
    printf("   new = Update firmware as new firmware\n");
    printf("   filename = Name of firmware image on FTP server\n");
    
    return 0;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    :   	Shell_update
* Returned Value   :  	int32_t error code
* Comments         :	Reads from a file .
*
*END*---------------------------------------------------------------------*/

int32_t  Shell_update(int32_t argc, char *argv[] )
{
   bool              print_usage, shorthelp = FALSE;
   int32_t            return_code = SHELL_EXIT_SUCCESS;
   char              *image_name = NULL;
   uint32_t          image_offset = 0, area_size = 0;
   char              *buff = NULL;
   MQX_FILE_PTR      fp, sf_fd;
   size_t            size = 0, actual = 0, chunk_len = 0;
   uint32_t          sf_erase_param[2] = { 0 };
#ifdef UPDATER_PERF_TEST
   TIME_STRUCT time_start, time_end, diff_time;
   uint32_t time_write_sf = 0, time_delete = 0, time_readfs = 0;
#endif

   print_usage = Shell_check_help_request(argc, argv, &shorthelp );

   if (!print_usage)  {
      if (argc != 3) {
         printf("Error, invalid number of parameters\n");
         return_code = SHELL_EXIT_ERROR;
         print_usage = TRUE;
      } else {
         if (!strcmp(argv[1], "recovery")) {

         } else if (!strcmp(argv[1], "new")) {
            image_offset = DEMOCFG_NEW_IMAGE_OFFSET;
            area_size = DEMOCFG_NEW_IMAGE_AREA_LEN;
         } else {
             printf("Invalid firmware type!\r\n");
             return SHELL_EXIT_ERROR;
         }
		 
         image_name = argv[2];
         
         fp = fopen(image_name, "rb");
         if (NULL == fp) {
            printf("Can't open file %s\n", image_name);
            return SHELL_EXIT_ERROR; 
         }

         _io_spi_flash_install(DEMOCFG_SPI_FLASH_DEV_NAME, &_bsp_spi_flash_init);
         sf_fd = fopen(DEMOCFG_SPI_FLASH_DEV_NAME, NULL);
         if (!sf_fd) {
            printf("Error open spi flash device!\n");
            fclose(fp);
            return SHELL_EXIT_ERROR; 
         }

         /* Get file size */
         fseek(fp, 0, IO_SEEK_END);
         size = ftell(fp);
         fseek(fp, 0, IO_SEEK_SET);

#ifdef UPDATER_PERF_TEST
         _time_get(&time_start);
#endif
         /* Erase image area */
         sf_erase_param[0] = image_offset;
         sf_erase_param[1] = area_size;
         if (ioctl(sf_fd, IO_IOCTL_SPIFLASH_ERASE, sf_erase_param)) {
            printf("Erase image area failed!\r\n");
            return_code = SHELL_EXIT_ERROR;
            goto err_rtn;
         }
#ifdef UPDATER_PERF_TEST
         _time_get(&time_end);
         _time_diff(&time_start, &time_end, &diff_time);
         time_delete = diff_time.MILLISECONDS + diff_time.SECONDS * 1000;
#endif

         buff = _mem_alloc_zero(DEMOCFG_UPDATE_BUFFER_SIZE);
         if (NULL == buff) {
            printf("Can't malloc buffer for update!\r\n");
            return_code = SHELL_EXIT_ERROR;
            goto err_rtn;
         }

         for (actual = 0; actual < size; actual += chunk_len) {
            chunk_len = min(size - actual, DEMOCFG_UPDATE_BUFFER_SIZE);

            if (fseek(fp, actual, IO_SEEK_SET)) {
                printf("Error seek offset in image file!\n");
                return_code = SHELL_EXIT_ERROR;
                goto err_rtn;
            }

#ifdef UPDATER_PERF_TEST
            _time_get(&time_start);
#endif
            if (chunk_len != fread(buff, 1, chunk_len, fp)) {
                printf("Error read firmware image data!\n");
                return_code = SHELL_EXIT_ERROR;
                goto err_rtn;
            }
#ifdef UPDATER_PERF_TEST
            _time_get(&time_end);
            _time_diff(&time_start, &time_end, &diff_time);
            time_readfs += diff_time.MILLISECONDS + diff_time.SECONDS * 1000;
#endif
#ifdef UPDATER_PERF_TEST
            _time_get(&time_start);
#endif
            if (fseek(sf_fd, image_offset + actual, IO_SEEK_SET)) {
                printf("Error seek offset in spi flash device!\n");
                return_code = SHELL_EXIT_ERROR;
                goto err_rtn;
            }
            if (chunk_len != fwrite(buff, 1, chunk_len, sf_fd)) {
                printf("Error write firmware to flash!\n");
                return_code = SHELL_EXIT_ERROR;
                goto err_rtn;
            }
#ifdef UPDATER_PERF_TEST
            _time_get(&time_end);
            _time_diff(&time_start, &time_end, &diff_time);
            time_write_sf += diff_time.MILLISECONDS + diff_time.SECONDS * 1000;
#endif
        }
      }
   }

   if (print_usage) {
       if (shorthelp)
		   printf("update <new> <filename>\n");
       else
            print_update_usage();
       return 0;
   }

#ifdef UPDATER_PERF_TEST
   printf("Erase time: %d ms, Read FS time: %d ms, Write Spi Flash time: %d ms\r\n",
          time_delete, time_readfs, time_write_sf);
#endif

err_rtn:
    fclose(sf_fd);
    fclose(fp);
    _io_dev_uninstall(DEMOCFG_SPI_FLASH_DEV_NAME);
    if (buff)
        _mem_free((pointer)buff);
	if (return_code) {
        printf("Failed to update firmware %s to spi flash @0x%x, size %d\n",
               image_name, image_offset, size);
	}else {
        printf("Succeed to update firmware %s to spi flash @0x%x, size %d\n",
               image_name, image_offset, size);
		reset_cpu();
	}
    return return_code;
}

/* EOF*/
