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
#include "dm9000_prv.h"          

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : dm9000x_shutdown
 *  Returned Value : ENET_OK or error code
 *  Comments       :
 *        Stops the chip.
 *
 *-----------------------------------------------------------------*/

uint32_t dm9000_shutdown
    (
        ENET_CONTEXT_STRUCT_PTR  enet_ptr
        /* [IN] the Ethernet state structure */
    )
{
    /* Body */
    DM9000_CONTEXT_STRUCT_PTR   context = (DM9000_CONTEXT_STRUCT_PTR)enet_ptr->MAC_CONTEXT_PTR;

    /* Reset Device */
    dm9000_phy_write(context, 0, 0x8000);
    dm9000_iow(context, DM9000_GPR, GPR_PHY_PWROFF); /* Power-Down PHY */
    dm9000_iow(context, DM9000_IMR, IMR_PAR);        /* Disable all interrupt */
    dm9000_iow(context, DM9000_RCR, 0x00);   /* Disable RX */

    /* Stop the FEC from interrupting the core */
    dm9000_mask_interrupts(enet_ptr);

#if BSPCFG_ENET_RESTORE
    /* Restore old ISRs */
    dm9000_uninstall_all_isrs(enet_ptr);
#endif

   dm9000_free_context(context);
   return ENET_OK;

} 


/* EOF */
