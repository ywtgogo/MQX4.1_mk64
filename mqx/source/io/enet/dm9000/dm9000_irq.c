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

static int32_t dm9000_clear_port_int_sts(void)
{
    PORT_MemMapPtr pctl;
    
    pctl = (PORT_MemMapPtr)PORTD_BASE_PTR;
    
    pctl->PCR[8] |= PORT_PCR_ISF_MASK;
    
    return 0;
}

void dm9000_isr
   (
         /* [IN] the Ethernet state structure */
      pointer  enet
   )
{ /* Body */

    ENET_CONTEXT_STRUCT_PTR          enet_ptr = (ENET_CONTEXT_STRUCT_PTR)enet;
    DM9000_CONTEXT_STRUCT_PTR        context = (DM9000_CONTEXT_STRUCT_PTR)enet_ptr->MAC_CONTEXT_PTR;
    uint8_t int_status;
    uint8_t  reg_save;

#ifdef CONFIG_DM9000_DEBUG_IRQ
	DM9000_DBG("entering %s\n", __func__);
#endif

    /* A real interrupt coming */
    /* Clear port interrupt */
    dm9000_clear_port_int_sts();

    /* Save previous register address */
    reg_save = dm9000_inb(context->io_addr);

	/* Disable all interrupts */
	dm9000_iow(context, DM9000_IMR, IMR_PAR);

	/* Got DM9000 interrupt status */
	int_status = dm9000_ior(context, DM9000_ISR);	/* Got ISR */
#ifdef CONFIG_DM9000_DEBUG_IRQ
    DM9000_DBG("interrupt status %x\n", int_status);
#endif
	dm9000_iow(context, DM9000_ISR, int_status);	/* Clear ISR status */

	/* Received the coming packet */
	if (int_status & ISR_PRS)
		dm9000_receive(enet_ptr);

	/* Transmit Interrupt check */
	if (int_status & ISR_PTS)
		dm9000_tx_done(enet_ptr);

	/* Re-enable interrupt mask */
	dm9000_iow(context, DM9000_IMR, context->imr_all);

#ifdef CONFIG_DM9000_DEBUG_IRQ
    dm9000_dump_regs(context);
#endif

    /* Restore previous register address */
    dm9000_outb(reg_save, context->io_addr);
} 


/* EOF */
