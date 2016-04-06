/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
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
* $FileName: Shell_Task.c$
* $Version : 3.7.5.0$
* $Date    : Jan-17-2011$
*
* Comments:
*
*   
*
*END************************************************************************/



#include "enet_cfg.h"
#include <mfs.h>
#include "nftl.h"

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Shell_Task
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/

#if DEMOCFG_ENABLE_SERIAL_SHELL

extern NFTL_IO_INFO bsp_spiflash_info;
extern void enet_initialize_networking(void);

static int32_t mfs_spiflash_open(MQX_FILE_PTR *sf_fs_fp, MQX_FILE_PTR *fs_fp)
{
    MQX_FILE_PTR tmp_sf_fs_fp, tmp_fs_fp;
    _mqx_int error_code;

    /* Open spi-nor flash wl in normal mode */
    tmp_sf_fs_fp = fopen(DEMOCFG_SPIFLASH_FS_DEVICE, NULL); 
    if (tmp_sf_fs_fp == NULL) {
        printf("Unable to open spinor flash device.\n");
        printf("\nYou must run Repair command\n");
        return 0;
    }

    /* Install MFS over spinor_wl driver */
    error_code = _io_mfs_install(tmp_sf_fs_fp, DEMOCFG_SPIFLASH_FS_NAME, (_file_size)0);
    if (error_code != MFS_NO_ERROR) {
        printf("Error initializing MFS: %s\n", MFS_Error_text((uint32_t)error_code));
        fclose(tmp_sf_fs_fp);
        _task_block();
        return 1;
    }

    /* Open file system */
    tmp_fs_fp = fopen(DEMOCFG_SPIFLASH_FS_NAME, NULL); 
    error_code = ferror (tmp_fs_fp);
    if ((error_code != MFS_NO_ERROR) && (error_code != MFS_NOT_A_DOS_DISK)) {
        printf("Error opening filesystem: %s(0x%x)\n", MFS_Error_text((uint32_t)error_code),error_code);
        fclose(tmp_sf_fs_fp);
        _task_block();
        return 1;
    }
    
    *sf_fs_fp = tmp_sf_fs_fp;
    *fs_fp = tmp_fs_fp;

    if (error_code == MFS_NOT_A_DOS_DISK) {
        printf("NOT A DOS DISK! You must format a: to continue.\n");
        return 0;
    }   
    
    printf ("SPI-NOR flash File system installed to %s\n", DEMOCFG_SPIFLASH_FS_NAME);
    
    return 0;
}

int32_t mfs_spiflash_close(MQX_FILE_PTR sf_fs_fp, MQX_FILE_PTR fs_fp)
{
    _mqx_int error_code;

    if (fs_fp != NULL)
    {
        /* Close the filesystem */
        if (MQX_OK != fclose(fs_fp))
        {
            printf("Error closing filesystem.\n");
            return 1;
        }
    }
    
    if (sf_fs_fp != NULL)
    {
        /* Uninstall MFS  */
        error_code = _io_dev_uninstall(DEMOCFG_SPIFLASH_FS_NAME);
        if (error_code != MFS_NO_ERROR) 
        {
            printf("Error uninstalling filesystem.\n");
            _task_block();
        } 
        
        /* Close the spinor flash device */
        if (MQX_OK != fclose (sf_fs_fp)) 
        {
            printf("Unable to close spinor device.\n");
            return 1;
        }
        
        printf ("spinor uninstalled.\n");
    }
    
    return 0;
}

void Shell_Task(uint32_t param)
{
    MQX_FILE_PTR sf_fs_fp = NULL, fs_fp = NULL;

    enet_initialize_networking();

    if (nftl_install(&bsp_spiflash_info, DEMOCFG_SPIFLASH_FS_DEVICE) != MQX_OK)
    {
        printf("Can't install NFTL!\n");
        _task_block();
    }

   /* Run the shell on the serial port */
   for(;;)  {
       mfs_spiflash_open(&sf_fs_fp, &fs_fp);
       Shell(Shell_commands, NULL);
       printf("Shell exited, restarting...\n");
       mfs_spiflash_close(sf_fs_fp, fs_fp);
   }
}
#endif

/* EOF */
