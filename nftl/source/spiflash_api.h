#ifndef __SPIFLASH_API_H__
#define __SPIFLASH_API_H__
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
#include "nftl.h"

extern uint32_t spiflash_init(NFTL_DEV_INFO_PTR flash_ptr);
extern void spiflash_deinit(NFTL_DEV_INFO_PTR flash_ptr);
extern uint32_t spiflash_readid(NFTL_DEV_INFO_PTR flash_ptr, uint32_t* idtable);
extern uint32_t spiflash_erasechip(NFTL_DEV_INFO_PTR flash_ptr);
extern uint32_t spiflash_eraseblock(NFTL_DEV_INFO_PTR flash_ptr,uint32_t block_number);
extern uint32_t spiflash_erasesector(NFTL_DEV_INFO_PTR flash_ptr,uint32_t sector_number);
extern uint32_t spiflash_byteread(NFTL_DEV_INFO_PTR,uint32_t address, char_ptr buffer_to, uint32_t count);
extern uint32_t spiflash_bytewrite(NFTL_DEV_INFO_PTR,uint32_t address, char_ptr buffer_from, uint32_t count);
extern uint32_t spiflash_writeprotect(NFTL_DEV_INFO_PTR flash_ptr, bool protect);
extern _mqx_int spiflash_ioctl(NFTL_DEV_INFO_PTR flash_ptr,_mqx_uint cmd, pointer param_ptr);

#endif
