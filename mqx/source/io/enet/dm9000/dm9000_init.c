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
#include "DM9000_prv.h"
#include "phy_dm9000.h"
#include "mqx_inc.h"


extern _mem_pool_id _BSP_sram_pool;

_enet_address dm9000_mac_addr;

const ENET_MAC_IF_STRUCT DM9000_IF = {
    dm9000_initialize,
    dm9000_shutdown,
    dm9000_send,
    NULL,
    NULL
#if BSPCFG_ENABLE_ENET_MULTICAST
    ,
    dm9000_join,
    dm9000_rejoin
#endif
};

/*
 * Initialize dm9000 board
 */
static void dm9000_init_dm9000(ENET_CONTEXT_STRUCT_PTR enet_ptr,
                               DM9000_CONTEXT_STRUCT_PTR context)
{
    uint8_t tmp = 0;
    uint32_t i = 0;

    DM9000_DBG("entering %s\n", __func__);

    /* Checksum mode */
#if defined(BSPCFG_ENET_HW_TX_IP_CHECKSUM) || defined(BSPCFG_ENET_HW_TX_PROTOCOL_CHECKSUM)
    dm9000_iow(context, DM9000_TCSCR, TCCR_IP | TCCR_TCP | TCCR_UDP);
    context->ip_summed = CHECKSUM_COMPLETE;
#endif

#if defined(BSPCFG_ENET_HW_RX_IP_CHECKSUM) || defined(BSPCFG_ENET_HW_RX_PROTOCOL_CHECKSUM)
    dm9000_iow(context, DM9000_RCSCSR, RCSR_CSUM);
#endif
    dm9000_iow(context, DM9000_GPCR, GPCR_GEP_CNTL);	/* Let GPIO0 output */

    if (enet_ptr->PARAM_PTR->OPTIONS & ENET_OPTION_MAC_LOOPBACK)
        tmp |= NCR_LBK_INT_MAC;

    //  Set Full/Half duplex based on mode.
    if (enet_ptr->PARAM_PTR->MODE & ENET_HALF_DUPLEX)
        tmp &= ~NCR_FDX; // half duplex
    else
        tmp |= NCR_FDX; // full duplex
    
    //dm9000_phy_write(context, PHY_DM9000_MII_REG_BMCR, PHY_DM9000_MII_REG_BMCR_RESET); /* PHY RESET */
    
    dm9000_iow(context, DM9000_NCR, tmp);

    /* Program operating register */
    dm9000_iow(context, DM9000_TCR, 0);	        /* TX Polling clear */
    dm9000_iow(context, DM9000_BPTR, BPTR_BPHW(3) | BPTR_JPT_600US);	/* Less 3Kb, 200us */
    dm9000_iow(context, DM9000_FCTR, FCTR_HWOT(3) | FCTR_LWOT(8)); /* Flow Control : High/Low Water */
    dm9000_iow(context, DM9000_FCR, 0xff);	/* Flow Control */
    //dm9000_iow(context, DM9000_FCR, 0x0);	/* Flow Control */
    dm9000_iow(context, DM9000_SMCR, 0);        /* Special Mode */
    /* clear TX status */
    dm9000_iow(context, DM9000_NSR, NSR_WAKEST | NSR_TX2END | NSR_TX1END);
    dm9000_iow(context, DM9000_ISR, ISR_CLR_STATUS); /* Clear interrupt status */

    /* Set address filter table */
    dm9000_hash_table_unlocked(context);

    tmp = IMR_PAR | IMR_PTM | IMR_PRM;

    context->imr_all = tmp;

    /* Enable TX/RX interrupt mask */
    dm9000_iow(context, DM9000_IMR, tmp);

    i = 0;
    while (!(dm9000_phy_read(context, PHY_DM9000_MII_REG_BMSR) &
             PHY_DM9000_MII_REG_BMSR_AN_COMPLETE)) {
        /* autonegation complete bit */
        _time_delay_ticks(1);
        ++i;
        if (1000 == i) {
            fprintf(stderr, "Could not establish link!\r\n");
            return;
        }
    }

    /* see what we've got */
    tmp = dm9000_phy_read(context, PHY_DM9000_MII_REG_DSCSR) >> 12;
    fprintf(stdout, "Operation at ");
    switch (tmp) {
    case 1:
        fprintf(stdout, "10M half duplex ");
        break;
    case 2:
        fprintf(stdout, "10M full duplex ");
        break;
    case 4:
        fprintf(stdout, "100M half duplex ");
        break;
    case 8:
        fprintf(stdout, "100M full duplex ");
        break;
    default:
        fprintf(stdout, "unknown: %d ", tmp);
        break;
    }
    fprintf(stdout, "mode\r\n");

    /* Init Driver variable */
    context->tx_pkt_cnt = 0;
    context->queue_pkt_len = 0;
}

/* General Purpose dm9000 reset routine */
int32_t dm9000_reset(DM9000_CONTEXT_STRUCT_PTR context, _enet_address mac_addr)
{
    uint32_t id_val, i = 0;

    DM9000_DBG("resetting DM9000\n");

    /* Reset DM9000,
       see DM9000 Application Notes V1.22 Jun 11, 2004 page 29 */

    /* DEBUG: Make all GPIO0 outputs, all others inputs */
    //dm9000_iow(context, DM9000_GPCR, GPCR_GPIO0_OUT);
    /* Step 1: Power internal PHY by writing 0 to GPIO0 pin */
    dm9000_iow(context, DM9000_GPR, 0);
    /* Step 2: Software reset */
    /* Fixing bug on dm9000_probe, takeover dm9000_reset(db),
     * Need 'NCR_MAC_LBK' bit to indeed stable our DM9000 fifo
     * while probe stage.
     */
    dm9000_iow(context, DM9000_NCR, (NCR_LBK_INT_MAC | NCR_RST));

    do {
        DM9000_DBG("resetting the DM9000, 1st reset\n");
        _time_delay(1); /* Wait at least 20 us */
    } while (dm9000_ior(context, DM9000_NCR) & 1);

    dm9000_iow(context, DM9000_NCR, 0);
    /* Fixing bug on dm9000_probe, takeover dm9000_reset(db),
     * Need 'NCR_MAC_LBK' bit to indeed stable our DM9000 fifo
     * while probe stage.
     */
    dm9000_iow(context, DM9000_NCR, (NCR_LBK_INT_MAC | NCR_RST)); /* Issue a second reset */

    do {
        DM9000_DBG("resetting the DM9000, 2nd reset\n");
        _time_delay(1); /* Wait at least 20 us */
    } while (dm9000_ior(context, DM9000_NCR) & 1);

    dm9000_iow(context, DM9000_NCR, 0);

    for (i = 0; i < 8; i++) {
        id_val  = dm9000_ior(context, DM9000_VIDL);
        id_val |= (uint32_t)dm9000_ior(context, DM9000_VIDH) << 8;
        id_val |= (uint32_t)dm9000_ior(context, DM9000_PIDL) << 16;
        id_val |= (uint32_t)dm9000_ior(context, DM9000_PIDH) << 24;

        if (id_val == DM9000_ID)
            break;
        fprintf(stderr, "read wrong id 0x%08x\n", id_val);
    }

    if (id_val != DM9000_ID) {
        fprintf(stderr, "wrong id: 0x%08x\n", id_val);
        return 1;
    }

    /* Identify what type of DM9000 we are working on */
    id_val = dm9000_ior(context, DM9000_CHIPR);
    DM9000_DBG("dm9000 revision 0x%02x\n", id_val);

    switch (id_val) {
    case CHIPR_DM9000A:
        context->type = TYPE_DM9000A;
        break;
    case CHIPR_DM9000B:
        context->type = TYPE_DM9000B;
        break;
    default:
        DM9000_DBG("ID %02x => defaulting to DM9000E\n", id_val);
        context->type = TYPE_DM9000E;
    }

    /* Write node address to PAR and eeprom first. */
    if (is_valid_ether_addr(mac_addr)) {
        /* Write to PAR */
        for (i = 0; i < 6; i++)
            dm9000_iow(context, DM9000_PAR + i, mac_addr[i]);
        for (i = 0; i < 6; i += 2)
            dm9000_write_eeprom(context, i / 2, &mac_addr[i]);
        _mem_copy(mac_addr, context->enet_addr, sizeof(_enet_address));
    } else {
        for (i = 0; i < 6; i += 2)
            dm9000_read_eeprom(context, i / 2, context->enet_addr + i);
        if (!is_valid_ether_addr(context->enet_addr)) {
            /* try reading from mac */
            for (i = 0; i < 6; i++)
                context->enet_addr[i] = dm9000_ior(context, DM9000_PAR + i);
        }
    }
    
    return 0;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : DM9000_initialize
*  Returned Value : ENET_OK or error code
*  Comments       :
*        Initializes the chip (extended version).
*
*END*-----------------------------------------------------------------*/

#define IF_ERROR_EXIT(c,x) {if (c) { error = x; goto EXIT; }}

uint32_t dm9000_initialize
    (
        ENET_CONTEXT_STRUCT_PTR enet_ptr
    )
{ 
    DM9000_CONTEXT_STRUCT_PTR    context = NULL;
    uchar_ptr                    buf_ptr;
    PCB2_PTR                     pcb_ptr;
    uint32_t                      i, rxsize, txsize, pcbsize;
    uint32_t                     error = ENET_OK;
    bool                      bOK;
 
    /* currently limit number of TX BDs to 32, as a bitmask is used in the free function. */
    if (enet_ptr->PARAM_PTR->NUM_TX_ENTRIES > 32) 
        return ENETERR_INVALID_INIT_PARAM;
   
    /*
     * This function can be called from any context, and it needs mutual
     * exclusion with itself.
     */
    dm9000_int_disable();

    /* Initialize the I/O Pins */
    dm9000_io_init(enet_ptr->PARAM_PTR->ENET_IF->PHY_NUMBER); 

    context = _mem_alloc_system_zero(sizeof(DM9000_CONTEXT_STRUCT));
    IF_ERROR_EXIT((NULL == context), ENETERR_ALLOC_MAC_CONTEXT);
    _mem_set_type((pointer)context, MEM_TYPE_IO_ENET_MAC_CONTEXT_STRUCT);
   
    enet_ptr->MAC_CONTEXT_PTR = (pointer)context;  

    context->io_addr = dm9000_get_base_address(enet_ptr->PARAM_PTR->ENET_IF->PHY_NUMBER);
    context->io_data = (uint8_t *)context->io_addr + DM9000_DATA_OFFSET;

    /* RESET device */
    if (dm9000_reset(context, enet_ptr->ADDRESS) < 0)
        return ENETERR_INIT_FAILED;

    /* Auto-detect 8/16/32 bit mode, ISR Bit 6+7 indicate bus width */
    context->io_mode = dm9000_ior(context, DM9000_ISR) >> 6;
    switch (context->io_mode) {
    case 0x0:  /* 16-bit mode */
        fprintf(stdout, "DM9000: running in 16 bit mode\n");
        dm9000_set_io(context, 2);
        break;
    case 0x01:  /* 32-bit mode */
        fprintf(stdout, "DM9000: running in 32 bit mode\n");
        dm9000_set_io(context, 4);
        break;
    case 0x02: /* 8 bit mode */
        fprintf(stdout, "DM9000: running in 8 bit mode\n");
        dm9000_set_io(context, 1);
        break;
    default:
        /* Assume 8 bit mode, will probably not work anyway */
        fprintf(stdout, "DM9000: Undefined IO-mode:0x%x\n", context->io_mode);
        dm9000_set_io(context, 1);
        break;
    }
    dm9000_init_dm9000(enet_ptr, context);
     
    /* Compute aligned buffer sizes */
    if (enet_ptr->PARAM_PTR->TX_BUFFER_SIZE) {
        context->AlignedTxBufferSize = DM9000_TX_ALIGN(enet_ptr->PARAM_PTR->TX_BUFFER_SIZE);
    } else {
        context->AlignedTxBufferSize = DM9000_TX_ALIGN(enet_ptr->MaxTxFrameSize);
    }

    if (enet_ptr->PARAM_PTR->RX_BUFFER_SIZE) {
        context->AlignedRxBufferSize = DM9000_RX_ALIGN(enet_ptr->PARAM_PTR->RX_BUFFER_SIZE);
    } else {
        context->AlignedRxBufferSize = DM9000_RX_ALIGN(enet_ptr->MaxRxFrameSize);
    }

    /* Allocate the Receive PCBs */
    pcbsize = enet_ptr->PARAM_PTR->NUM_RX_PCBS * sizeof(PCB2);
    context->RX_PCB_BASE = (PCB2_PTR)_mem_alloc_system_zero(pcbsize);
    IF_ERROR_EXIT((NULL == context->RX_PCB_BASE), ENETERR_ALLOC_PCB);
    _mem_set_type((pointer)context->RX_PCB_BASE, MEM_TYPE_IO_PCB_STRUCT);
       
    /* Allocate the Transmit and Receive buffers */
    txsize = (enet_ptr->PARAM_PTR->NUM_TX_BUFFERS * context->AlignedTxBufferSize) + DM9000_TX_BUFFER_ALIGNMENT;
    rxsize = (enet_ptr->PARAM_PTR->NUM_RX_BUFFERS  * context->AlignedRxBufferSize) +DM9000_RX_BUFFER_ALIGNMENT;

#if BSPCFG_HAS_SRAM_POOL && BSPCFG_ENET_SRAM_BUF
    buf_ptr = _mem_alloc_system_from(_BSP_sram_pool, rxsize + txsize);
#else
    buf_ptr = _mem_alloc_system_uncached(rxsize + txsize);  /* temporary fix for cache problems... was befor _mem_alloc_system */
#endif
   
    IF_ERROR_EXIT((NULL == buf_ptr), ENETERR_ALLOC_BUFFERS);
    _mem_set_type(buf_ptr, MEM_TYPE_IO_ENET_BUFFERS);

    context->UNALIGNED_BUFFERS = buf_ptr;

    /* Align to TX buffer boundary */
    buf_ptr = (uchar_ptr)DM9000_TX_ALIGN((uint32_t)buf_ptr);
   
    /* Queue packets on TX Buffer Q. */
    context->TX_BUFFERS = NULL;
    for (i = 0; i < enet_ptr->PARAM_PTR->NUM_TX_BUFFERS; i++) {
        ENET_Enqueue_Buffer((pointer *)&context->TX_BUFFERS, buf_ptr);
        buf_ptr += context->AlignedTxBufferSize;
    }
   
    /* Align to RX buffer boundary */
    buf_ptr = (uchar_ptr)DM9000_RX_ALIGN((uint32_t)buf_ptr);
    /* Queue packets on RX Buffer Q. */
    context->RX_BUFFERS = NULL;
    for (i = 0; i < enet_ptr->PARAM_PTR->NUM_RX_BUFFERS; i++) {
        ENET_Enqueue_Buffer((pointer *)&context->RX_BUFFERS, buf_ptr);
        buf_ptr += context->AlignedRxBufferSize;
   }

    /* Enqueue the RX PCBs onto the receive PCB queue */
    pcb_ptr = context->RX_PCB_BASE;
    for (i = 0; i < enet_ptr->PARAM_PTR->NUM_RX_PCBS; i++) {
        QADD(context->RxPCBHead, context->RxPCBTail, (PCB_PTR)pcb_ptr);
        pcb_ptr++;
    }

    /* Install the ISRs */
    bOK = dm9000_install_isrs(enet_ptr, &DM9000_device[enet_ptr->PARAM_PTR->ENET_IF->PHY_NUMBER]);
    IF_ERROR_EXIT(!bOK, ENETERR_INSTALL_ISR);
          
    /* Control transfers to this point on any error, with error set to error code. */
EXIT:
    if (ENET_OK!=error) {
#if BSPCFG_ENET_RESTORE
        dm9000_uninstall_all_isrs(enet_ptr);
#endif
        dm9000_free_context(context);
    }
    
#ifdef CONFIG_DM9000_DEBUG
    dm9000_dump_regs(context);
#endif

    dm9000_int_enable();
 
    return error;
} 

/* EOF */
