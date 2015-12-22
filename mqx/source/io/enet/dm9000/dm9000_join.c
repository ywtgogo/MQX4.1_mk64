/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include "enet.h"
#include "enetprv.h"
#include "dm9000_prv.h"          

#if BSPCFG_ENABLE_ENET_MULTICAST

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : dm9000_join
*  Returned Value : void
*  Comments       :
*        Joins a multicast group on an Ethernet channel.
*
*END*-----------------------------------------------------------------*/

uint32_t dm9000_join
   (
     ENET_CONTEXT_STRUCT_PTR  enet_ptr,
         /* [IN] the Ethernet state structure */
     ENET_MCB_STRUCT_PTR     mcb_ptr
         /* [IN] the multicast control block */
   )
{ 
    DM9000_CONTEXT_STRUCT_PTR  context = (DM9000_CONTEXT_STRUCT_PTR)enet_ptr->MAC_CONTEXT_PTR;
    uint32_t crc, i, oft;
    uint16_t hash_table[4] = { 0 };

    DM9000_DBG("entering %s\n", __func__);

    /* broadcast address */
    //hash_table[3] = 0x8000;
 
    /* Compute the CRC-32 polynomial on the multicast group */
    /* Set the appropriate bit in the hash table */
    crc = dm9000_ether_crc_le(6, mcb_ptr->GROUP, 1) & 0x3f;
    crc >>= 26; /* I don't konw why? */
    crc &= 0x3f;

    mcb_ptr->HASH = crc;

    hash_table[crc / 16] |= (uint16_t)1 << (crc % 16);

    /* Write the hash table to MAC MD table */
    for (i = 0, oft = DM9000_MAR; i < 4; i++) {
        dm9000_iow(context, oft++, hash_table[i]);
        dm9000_iow(context, oft++, hash_table[i] >> 8);
    }

    return ENET_OK;
} 

#endif

/* EOF */
