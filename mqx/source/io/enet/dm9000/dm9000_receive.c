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
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"
#include "enet.h"
#include "enetprv.h"
#include "bsp_prv.h"
#include "dm9000_prv.h"        

extern void *ENET_Dequeue_Buffer( void **q);
static void dm9000_rx_free
   (
         /* [IN] the PCB to enqueue */
      PCB_PTR  pcb_ptr
   )
{ 
    ENET_CONTEXT_STRUCT_PTR   enet_ptr = (ENET_CONTEXT_STRUCT_PTR)pcb_ptr->PRIVATE;
    DM9000_CONTEXT_STRUCT_PTR   context = (DM9000_CONTEXT_STRUCT_PTR)(enet_ptr->MAC_CONTEXT_PTR);

#ifdef CONFIG_DM9000_DEBUG_RX
    DM9000_DBG("entering %s\n", __func__);
#endif

    /*
     * This function can be called from any context, and it needs mutual
     * exclusion with itself.
     */
    dm9000_int_disable();

    ENET_Enqueue_Buffer((pointer *)&context->RX_BUFFERS, pcb_ptr->FRAG[0].FRAGMENT);
    pcb_ptr->FRAG[0].FRAGMENT = NULL;
   
    QADD(context->RxPCBHead, context->RxPCBTail, pcb_ptr);

    dm9000_int_enable();
}

#pragma pack(1)
struct dm9000_rxhdr {
    uint8_t   RxPktReady;
    uint8_t   RxStatus;
    uint16_t  RxLen;
};

static int32_t dm9000_get_rxhdr(DM9000_CONTEXT_STRUCT_PTR context,
                                    uint8_t *rx_status, uint16_t *rx_len)
{
    struct dm9000_rxhdr rxhdr;

    memset(&rxhdr, 0, sizeof(struct dm9000_rxhdr));

    dm9000_outb(DM9000_MRCMD, context->io_addr);

	(context->inblk)(context->io_data, &rxhdr, sizeof(struct dm9000_rxhdr));

    *rx_status = rxhdr.RxStatus;
    *rx_len = rxhdr.RxLen;

    return 0;
}

static int32_t dm9000_check_rxhdr(pointer enet, uint16_t *rx_len)
{
    ENET_CONTEXT_STRUCT_PTR         enet_ptr = (ENET_CONTEXT_STRUCT_PTR)enet;
    DM9000_CONTEXT_STRUCT_PTR       context = (DM9000_CONTEXT_STRUCT_PTR)enet_ptr->MAC_CONTEXT_PTR;
    uint8_t GoodPacket, RxStatus;
	uint16_t RxLen;

   	/* Check packet ready or not */
    /* A packet ready now  & Get status/length */
    GoodPacket = 1;
    dm9000_get_rxhdr(context, &RxStatus, &RxLen);

#ifdef CONFIG_DM9000_DEBUG_RX
    DM9000_DBG("rx status: 0x%04x rx len: %d\n", RxStatus, RxLen);
#endif

    /* Packet Status check */
	if (RxLen < 0x40) {
        GoodPacket = 0;
        fprintf(stderr, "RX: Bad Packet (runt)\n");
	}

	if (RxLen > DM9000_PKT_MAX)
	    DM9000_DBG("RST: RX Len:%x\n", RxLen);

	/* rxhdr.RxStatus is identical to RSR register. */
	if (RxStatus & (RSR_FOE | RSR_CE | RSR_AE |
			      RSR_PLE | RSR_RWTO |
			      RSR_LCS | RSR_RF)) {
	   	GoodPacket = 0;
	   	if (RxStatus & RSR_FOE) 
            ENET_INC_STATS(ST_RX_OVERRUN);
	   		fprintf(stderr, "rx fifo error\n");
	   	if (RxStatus & RSR_CE)
            ENET_INC_STATS(ST_RX_FCS);
	   		fprintf(stderr, "rx crc error\n");
	   	if (RxStatus & RSR_RF)
            ENET_INC_STATS(ST_RX_ALIGN);
	   		fprintf(stderr, "rx length error\n");
        if (RxLen > DM9000_PKT_MAX) {
            ENET_INC_STATS(ST_RX_GIANT);
            fprintf(stderr, "rx length too big\n"); 
            dm9000_reset(context, context->enet_addr);
        }
    }
 
    if (GoodPacket) {
        *rx_len = RxLen;
        return 0;
    } else {
        /* need to dump the packet's data */
        (context->dumpblk)(context->io_data, RxLen);
        return 1;
    }
}

/*
 *  Received a packet and pass to upper layer
 */
static int32_t dm9000_rx(DM9000_CONTEXT_STRUCT_PTR context, uint8_t *buf, uint32_t len)
{
	/* Move data from DM9000 */
	/* Read received packet from RX SRAM */
    (context->inblk)(context->io_data, buf, len);

    return 0;
}

/* ISR context */
int32_t dm9000_receive(ENET_CONTEXT_STRUCT_PTR enet_ptr)
{
    DM9000_CONTEXT_STRUCT_PTR       context = (DM9000_CONTEXT_STRUCT_PTR)enet_ptr->MAC_CONTEXT_PTR;
    PCB_PTR                          pcb_ptr = NULL;
    uint8_t *rx_buf, rxbyte;
	uint16_t rx_len;

	/* Check packet ready or not */
	do {
		dm9000_ior(context, DM9000_MRCMDX);	/* Dummy read */

		/* Get most updated data */
		rxbyte = dm9000_inb(context->io_data);

		/* Status check: this byte must be 0 or 1 */
		if (rxbyte & DM9000_PKT_ERR) {
			fprintf(stderr, "status check fail: %d\n", rxbyte);
			dm9000_iow(context, DM9000_RCR, 0x00);	/* Stop Device */
			dm9000_iow(context, DM9000_ISR, IMR_PAR);	/* Stop INT request */
			return 1;
		}

		if (!(rxbyte & DM9000_PKT_RDY))
			return 0;

#ifdef CONFIG_DM9000_DEBUG_RX
        DM9000_DBG("receiving packet\n");
#endif

        /* A packet ready now  & Get status/length */
        if (dm9000_check_rxhdr(enet_ptr, &rx_len))
            continue;

        rx_buf = (uchar_ptr)ENET_Dequeue_Buffer((pointer *)&context->RX_BUFFERS);
        if (rx_buf == NULL) {
            dm9000_int_enable();
            DM9000_DBG("RX buffer queue full!\r\n");
            continue;
        }
        QGET(context->RxPCBHead, context->RxPCBTail, pcb_ptr);
        if (pcb_ptr) {
            // allocation successful, initialize new receive PCB
            pcb_ptr->FREE = dm9000_rx_free;
            pcb_ptr->PRIVATE = (pointer)enet_ptr;
        } else {
            ENET_INC_STATS(RX_PCBS_EXHAUSTED);
            continue;
        }
        dm9000_rx(context, rx_buf, rx_len);

#ifdef CONFIG_DM9000_DEBUG_RX
        DM9000_DUMP_PACKET(__func__, rx_buf, rx_len);
#endif

        ENET_INC_STATS(COMMON.ST_RX_TOTAL);

        // Add the packet to the list of fragments in the PCB               
        pcb_ptr->FRAG[0].FRAGMENT = rx_buf;
        // remove CRC
        pcb_ptr->FRAG[0].LENGTH = rx_len - ENET_FRAMESIZE_TAIL;
        context->CurrentRxECB = ENET_find_receiver(enet_ptr, (ENET_HEADER_PTR)rx_buf, (uint32_t *)&rx_len); 
        if (!(context->CurrentRxECB)) {
            DM9000_DBG("No RxECB founded!\r\n");
            dm9000_rx_free(pcb_ptr);
            continue;
        }

        context->CurrentRxECB->SERVICE(pcb_ptr, context->CurrentRxECB->PRIVATE);
	} while (rxbyte & DM9000_PKT_RDY);
    
    return 0;
}
/* EOF */
