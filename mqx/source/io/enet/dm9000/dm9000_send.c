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
**************************************************************************
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "enet.h"
#include "enetprv.h"
#include "dm9000_prv.h"          

static void dm9000_send_packet(DM9000_CONTEXT_STRUCT_PTR context, uint16_t pkt_len)
{
    /* The DM9000 is not smart enough to leave fragmented packets alone. */
    /* Can be moved to init function. */
    if (context->ip_summed == CHECKSUM_NONE)
        dm9000_iow(context, DM9000_TCSCR, 0);
    else
        dm9000_iow(context, DM9000_TCSCR, TCCR_IP | TCCR_UDP | TCCR_TCP);

    /* Set TX length to DM9000 */
    dm9000_iow(context, DM9000_TXPLL, pkt_len & 0xff);
    dm9000_iow(context, DM9000_TXPLH, (pkt_len >> 8) & 0xff);

    /* Issue TX polling command */
    dm9000_iow(context, DM9000_TCR, TCR_TXREQ); /* Cleared after TX complete */

#ifdef CONFIG_DM9000_DEBUG_TX
#if 0
    while (1) {
        uint8_t nsr = dm9000_ior(context, DM9000_NSR);
        uint8_t isr = dm9000_ior(context, DM9000_ISR);
        
        if (nsr & (NSR_TX1END | NSR_TX2END))
            break;
        if (isr & ISR_PTS)
            break;
    }
    dm9000_iow(context, DM9000_ISR, ISR_PTS); /* Clear Tx bit in ISR */
#endif
#endif
}

/*
  Hardware start transmission.
  Send a packet to media from the upper layer.
*/
static int dm9000_tx(DM9000_CONTEXT_STRUCT_PTR context, PCB_FRAGMENT_PTR frag_ptr)
{
    char *data_ptr;
    uint32_t totlen, len;

    /* Move data to DM9000 TX RAM */
    dm9000_outb(DM9000_MWCMD, context->io_addr);

    totlen = 0;
    for (len = frag_ptr->LENGTH; len != 0; len = frag_ptr->LENGTH) {
        data_ptr = (char *)frag_ptr->FRAGMENT;
#ifdef CONFIG_DM9000_DEBUG_TX
        DM9000_DUMP_PACKET(__func__, data_ptr, len);
#endif
        context->outblk(context->io_data, data_ptr, len);
        totlen += len;
        frag_ptr++;
    }

    context->tx_pkt_cnt++;
     /* TX control: First packet immediately send, second packet queue */
    if (context->tx_pkt_cnt == 1)
        dm9000_send_packet(context, totlen);
    else {
        /* Second packet */
        context->queue_pkt_len = totlen;
    }
    return 0;
}

void dm9000_tx_done(ENET_CONTEXT_STRUCT_PTR enet_ptr)
{
    DM9000_CONTEXT_STRUCT_PTR        context = (DM9000_CONTEXT_STRUCT_PTR)enet_ptr->MAC_CONTEXT_PTR;
    int32_t tx_status = dm9000_ior(context, DM9000_NSR);    /* Got TX status */

#ifdef CONFIG_DM9000_DEBUG_IRQ
    DM9000_DBG("entering %s\n", __func__);
#endif
    if (tx_status & (NSR_TX2END | NSR_TX1END)) {
        ENET_INC_STATS(COMMON.ST_TX_TOTAL);
        /* One packet sent complete */
        context->tx_pkt_cnt--;

        /* Queue packet check & send */
        if (context->tx_pkt_cnt > 0)
            dm9000_send_packet(context, context->queue_pkt_len);
    }
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : dm9000x_send
*  Returned Value : ENET_OK or error code
*  Comments       :
*        Sends a packet.
*
*END*-----------------------------------------------------------------*/

uint32_t dm9000_send
    (
        ENET_CONTEXT_STRUCT_PTR  enet_ptr,
        /* [IN] the Ethernet state structure */
        PCB_PTR              packet,
        /* [IN] the packet to send */
        uint32_t              size,
        /* [IN] total size of the packet */
        uint32_t              frags,
        /* [IN] total fragments in the packet */
        uint32_t              flags
        /* [IN] optional flags, zero = default */
    )
{ 
    DM9000_CONTEXT_STRUCT_PTR    context = (DM9000_CONTEXT_STRUCT_PTR)enet_ptr->MAC_CONTEXT_PTR;
    PCB_FRAGMENT_PTR              frag_ptr;
    uint32_t                       err = ENET_OK;

#ifdef CONFIG_DM9000_DEBUG_TX
    DM9000_DBG("entering %s\n", __func__);
#endif
    dm9000_int_disable();
   
#if BSPCFG_ENABLE_ENET_HISTOGRAM
    {
        uint32_t index = size>> ENET_HISTOGRAM_SHIFT;
      
        if (index < ENET_HISTOGRAM_ENTRIES)
            ENET_INC_STATS(TX_HISTOGRAM[index]);
    }
#endif
   
    frag_ptr = packet->FRAG;

    err = dm9000_tx(context, frag_ptr);
    PCB_free(packet);

    dm9000_int_enable();
      
   return err;
}

/* EOF */
