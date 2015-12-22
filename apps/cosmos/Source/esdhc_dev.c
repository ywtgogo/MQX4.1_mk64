#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include <mfs.h>
#include <shell.h>
#include <sdcard.h>
#include <spi.h>


#if ! SHELLCFG_USES_MFS
#error "This application requires SHELLCFG_USES_MFS defined non-zero in user_config.h. Please recompile libraries with this option."
#endif

#if defined BSP_SDCARD_ESDHC_CHANNEL
#if ! BSPCFG_ENABLE_ESDHC
#error "This application requires BSPCFG_ENABLE_ESDHC defined non-zero in user_config.h. Please recompile libraries with this option."
#else
#define SDCARD_COM_CHANNEL BSP_SDCARD_ESDHC_CHANNEL
#endif

#elif defined BSP_SDCARD_SDHC_CHANNEL
#if ! BSPCFG_ENABLE_SDHC
#error "This application requires BSPCFG_ENABLE_SDHC defined non-zero in user_config.h. Please recompile libraries with this option."
#else
#define SDCARD_COM_CHANNEL BSP_SDCARD_SDHC_CHANNEL
#endif

#elif defined (BSP_SDCARD_SPI_CHANNEL)
#define SDCARD_COM_CHANNEL BSP_SDCARD_SPI_CHANNEL
#else
#error "SDCARD low level communication device not defined!"
#endif

/*TASK*-----------------------------------------------------------------
*
* Function Name  : sdcard_task
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/
void sdcard_task(uint32_t temp)
{
    (void)          temp; /* suppress 'unused variable' warning */
    bool            inserted = TRUE, readonly = FALSE, last = FALSE;
    _mqx_int        error_code;
    _mqx_uint       param;
    MQX_FILE_PTR    com_handle, sdcard_handle, filesystem_handle;
    char            filesystem_name[] = "c:";

#if defined BSP_SDCARD_GPIO_DETECT
    LWGPIO_STRUCT   sd_detect;
#endif
    /* Open low level communication device */
    com_handle = fopen(SDCARD_COM_CHANNEL, (void *)(SPI_FLAG_FULL_DUPLEX));

    if (NULL == com_handle)
    {
        printf("Error opening communication handle %s, check your user_config.h.\n", SDCARD_COM_CHANNEL);
        _task_block();
    }
#if defined BSP_SDCARD_GPIO_DETECT
    /* Init GPIO pins for other SD card signals */
    error_code = lwgpio_init(&sd_detect, BSP_SDCARD_GPIO_DETECT, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE);
       if (!error_code)
       {
           printf("Initializing GPIO with sdcard detect pin failed.\n");
           _task_block();
       }
       /*Set detect and protect pins as GPIO Function */
       lwgpio_set_functionality(&sd_detect,BSP_SDCARD_DETECT_MUX_GPIO);
       lwgpio_set_attribute(&sd_detect, LWGPIO_ATTR_PULL_DOWN, LWGPIO_AVAL_ENABLE);//LWGPIO_ATTR_PULL_UP
#endif
    /* Install SD card device */
    error_code = _io_sdcard_install("sdcard:", (void *)&_bsp_sdcard0_init, com_handle);
    if ( error_code != MQX_OK )
    {
        printf("Error installing SD card device (0x%x)\n", error_code);
        _task_block();
    }

    for (;;)
    {
#if defined BSP_SDCARD_GPIO_DETECT
        inserted = !lwgpio_get_value(&sd_detect);
#endif
        if (last != inserted)
        {
            last = inserted;
        	        
            if (inserted)
            {
                _time_delay (200);
                /* Open the device which MFS will be installed on */
                sdcard_handle = fopen("sdcard:", 0);
                if (sdcard_handle == NULL)
                {
                    printf("\nUnable to open SD card device.");
                    continue;
                }

				printf("\nInstalling MFS over SD card driver...");
				
				/* Install MFS over SD card driver */
				error_code = _io_mfs_install(sdcard_handle, filesystem_name, (_file_size)0);
				if (error_code != MFS_NO_ERROR)
				{
					printf("Error initializing MFS: %s\n", MFS_Error_text((uint32_t)error_code));
				}

                /* Open file system */
                if (error_code == MFS_NO_ERROR) {
                    filesystem_handle = fopen(filesystem_name, NULL);
                    error_code = ferror(filesystem_handle);
                    if (error_code == MFS_NOT_A_DOS_DISK)
                    {
                        printf("NOT A DOS DISK! You must format to continue.\n");
                    }
                    else if (error_code != MFS_NO_ERROR)
                    {
                        printf("Error opening filesystem: %s\n", MFS_Error_text((uint32_t)error_code));
                        continue;
                    }

                    printf ("\nSD card installed to %s", filesystem_name);
					
					/* Init SUBDIR */ 
					if (ioctl(filesystem_handle, IO_IOCTL_CHECK_DIR_EXIST,(void *)"\\datalog" ))
					{
						error_code = ioctl(filesystem_handle, IO_IOCTL_CREATE_SUBDIR, (void *) "\\datalog");
						//printf ("SD card installed to %s\n", filesystem_name);
					}
					if (ioctl(filesystem_handle, IO_IOCTL_CHECK_DIR_EXIST,(void *)"\\cmdlog" ))
					{
						error_code = ioctl(filesystem_handle, IO_IOCTL_CREATE_SUBDIR, (void *) "\\cmdlog");
						//printf ("SD card installed to %s\n", filesystem_name);
					}
                }
            }
            else
            {
                /* Close the filesystem */
                if ((filesystem_handle != NULL) && (MQX_OK != fclose(filesystem_handle)))
                {
                    printf("Error closing filesystem.\n");
                }
                filesystem_handle = NULL;

                /* Uninstall MFS  */
                error_code = _io_dev_uninstall(filesystem_name);
                if (error_code != MFS_NO_ERROR)
                {
                    printf("Error uninstalling filesystem.\n");
                }

                /* Close the SD card device */
                if ((sdcard_handle != NULL) && (MQX_OK != fclose(sdcard_handle)))
                {
                    printf("Unable to close SD card device.\n");
                }
                sdcard_handle = NULL;

                printf ("SD card uninstalled.\n");
            }
        }
        
        _time_delay (1000);
    }
}

/* EOF */
