/**HEADER********************************************************************
* 
* Copyright (c) 2009 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2009 Embedded Access Inc.;
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
#ifndef __DM9000X_PRV_H__
#define __DM9000X_PRV_H__

#include "dm9000.h"
#include "dm9000_def.h"

#define CHECKSUM_NONE           0
#define CHECKSUM_UNNECESSARY    1
#define CHECKSUM_COMPLETE       2
#define CHECKSUM_PARTIAL        3

//#define DM9000_MII_TIMEOUT         (0x10000)
#define DM9000_RESET_TIMEOUT       (0x10000)

#define DM9000_RX_ALIGN(n) ENET_ALIGN(n, DM9000_RX_BUFFER_ALIGNMENT)
#define DM9000_TX_ALIGN(n) ENET_ALIGN(n, DM9000_TX_BUFFER_ALIGNMENT)
#define DM9000_BD_ALIGN(n) ENET_ALIGN(n, DM9000_BD_ALIGNMENT)

#define DM9000_NUM_ISRS      1

#define DM9000_MAX_FRAGS     6     // Assumes RX_BUFFER_SIZE >= 256, so that 256*6=1536, set higher for smaller RX_BUFFER_SIZE

#define dm9000_int_disable()       _int_disable()
#define dm9000_int_enable()        _int_enable()

// Set ENETCFG_SUPPORT_FRAGMENTED_PCBS to 1 if the upper layer can support receive PCBs with multiple
// fragments.  NOTE: RTCS currently does not support this.
#ifndef ENETCFG_SUPPORT_FRAGMENTED_PCBS
   #define ENETCFG_SUPPORT_FRAGMENTED_PCBS 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define DM9000_INT_RX_TX    0
#define DM9000_NUM_INTS     1
    
//#define CONFIG_DM9000_DEBUG
//#define CONFIG_DM9000_DEBUG_TX
//#define CONFIG_DM9000_DEBUG_IRQ
//#define CONFIG_DM9000_DEBUG_RX

#ifdef CONFIG_DM9000_DEBUG
#define DM9000_DBG(fmt,args...) printf(fmt, ##args)
#define DM9000_DUMP_PACKET(func, packet, length)  \
    do { \
        int i; 							\
        fprintf(stdout, "%s: length: %d\n", func, length);		\
        for (i = 0; i < length; i++) {				\
            if (i % 8 == 0)					\
                fprintf(stdout, "\n%02x: ", i);	\
                fprintf(stdout, "%02x ", ((uint8_t *) packet)[i]);	\
        } fprintf(stdout, "\n");						\
    } while(0)
#else
#define DM9000_DBG(fmt,args...)
#define DM9000_DMP_PACKET(func,packet,length)
#endif

#define dm9000_outb(d,r) ( *(volatile uint8_t *)r = d )
#define dm9000_outw(d,r) ( *(volatile uint16_t *)r = d )
#define dm9000_outl(d,r) ( *(volatile uint32_t *)r = d )
#define dm9000_inb(r) (*(volatile uint8_t *)r)
#define dm9000_inw(r) (*(volatile uint16_t *)r)
#define dm9000_inl(r) (*(volatile uint32_t *)r)

/* The Ethernet initialization structure */
typedef struct {
   uint32_t            BUS_CLOCK;     
   _int_level         ERX_LEVEL;      // Receive interrupt level
   _int_priority      ERX_SUBLEVEL;   // Receive interrupt sublevel
   _int_level         ETX_LEVEL;      // Transmit interrupt level
   _int_priority      ETX_SUBLEVEL;   // Transmit interrupt sublevel
} DM9000_INIT_STRUCT, _PTR_ DM9000_INIT_STRUCT_PTR;


typedef struct dm9000_context_struct {
    void    *io_addr;   /* Register I/O base address */
    void    *io_data;   /* Data I/O address */

    uint16_t     tx_pkt_cnt;
    uint16_t     queue_pkt_len;
    uint16_t     queue_start_addr;
    uint16_t     queue_ip_summed;
    uint16_t     dbug_cnt;
    uint8_t      io_mode;        /* 0:word, 2:byte */
    uint8_t      phy_addr;
    uint8_t      imr_all;

    uint32_t     flags;

    enum dm9000_type type;

    void        (  *inblk)(void *port, volatile void *data, int32_t length);
    void        ( *outblk)(void *port, volatile void *data, int32_t length);
    void        (*dumpblk)(void *port, int32_t length);

    uint32_t     msg_enable;
    int32_t     ip_summed;

    _enet_address enet_addr;

    uint16_t     AlignedRxBufferSize;
    uint16_t     AlignedTxBufferSize;
   
    /*
    **    The Receive-Side State
    **
    ** RxPCBHead and RxPCBTail are the head and tail of a linked list
    ** of available PCBs for receiving packets.  RxEntries is
    ** the length of this list.
    **
    ** NextRxBD points to the 'first' queued receive descriptor, i.e., usually
    ** the one that RBPTR is pointing to.  When an RXF interrupt occurs,
    ** NextRxBD is pointing to the first descriptor with a received packet.
    ** The packets is forwarded to the upper layers, and NextRxBD is incremented.
    **
    ** LastRxBD points to the 'last' queued receive descriptor, i.e., the last
    ** one with E=1.  The LastRxBD descriptor usually precedes the LastRxBD descriptor.
    ** When the upper layers free a received packet, it gets placed at LastRxBD
    ** (with E=1), and LastRxBD is incremented.
    */
  
    PCB_PTR                       RxPCBHead;
    PCB_PTR                       RxPCBTail;
   
    ENET_ECB_STRUCT_PTR           CurrentRxECB;
    
    /*
    **    The Transmit-Side State
    **
    ** TxPCBS_PTR is an array of pointers to the PCBs queued on the transmit ring.
    ** AvailableTxBDs is the number of free entries on the ring.
    **
    ** NextTxBD points to the first available transmit descriptor (when AvailableTxBDs>0),
    ** or to LastTxBD (when AvailableTxBDs==NumTxBDs).  It is the descriptor that will next
    ** be queued onto the transmit ring, as soon as there's a packet to send
    ** and room on the transmit ring.
    **
    ** LastTxBD points to the 'last' queued transmit descriptor, i.e., the one
    ** that TBPTR is pointing to.  When a TXB interrupt occurs, LastTxBD is
   ** pointing to the transmitted buffer.  The buffer is freed, and LastTxBD is
    ** incremented.
    **
    ** TxErrors keeps track of all the errors that occur during transmission
    ** of a frame comprised of multiple buffers.
    */
    uint32_t                       TxErrors;

    PCB2_PTR                      RX_PCB_BASE;       
    pointer                       UNALIGNED_BUFFERS;  
    uchar_ptr                     RX_BUFFERS;  
    uchar_ptr                     TX_BUFFERS;  
    
#if BSPCFG_ENET_RESTORE
    /* These fields are kept only for ENET_shutdown() */
    INT_ISR_FPTR                  OLDISR_PTR[DM9000_NUM_ISRS];
    pointer                       OLDISR_DATA[DM9000_NUM_ISRS];
#endif
} DM9000_CONTEXT_STRUCT, * DM9000_CONTEXT_STRUCT_PTR;
    
    
extern const   DM9000_INIT_STRUCT DM9000_device[];
extern const   ENET_MAC_IF_STRUCT DM9000_IF;
extern const   uint32_t dm9000_vectors[DM9000_DEVICE_COUNT][DM9000_NUM_INTS];

extern bool dm9000_install_isrs( ENET_CONTEXT_STRUCT_PTR enet_ptr, DM9000_INIT_STRUCT const * enet_init_ptr  ); 
extern void    dm9000_add_buffers_to_rx_ring(DM9000_CONTEXT_STRUCT_PTR context);
extern uint32_t dm9000_initialize(ENET_CONTEXT_STRUCT_PTR);
extern uint32_t dm9000_shutdown( ENET_CONTEXT_STRUCT_PTR );
extern uint32_t dm9000_send( ENET_CONTEXT_STRUCT_PTR, PCB_PTR packet, uint32_t size, uint32_t frags, uint32_t flags );
extern uint32_t dm9000_join( ENET_CONTEXT_STRUCT_PTR, ENET_MCB_STRUCT_PTR mcb_ptr );
extern uint32_t dm9000_rejoin( ENET_CONTEXT_STRUCT_PTR );
extern void    dm9000_mask_interrupts( ENET_CONTEXT_STRUCT_PTR enet_ptr ); 
extern void    dm9000_uninstall_all_isrs( ENET_CONTEXT_STRUCT_PTR enet_ptr );
extern void    dm9000_tx_isr(pointer);
extern void    dm9000_rx_isr(pointer);
extern pointer dm9000_get_base_address (uint32_t);
extern uint32_t dm9000_get_vector(uint32_t,uint32_t);
extern void    dm9000_io_init(uint32_t);
extern void    dm9000_free_context( DM9000_CONTEXT_STRUCT_PTR context);
extern uint32_t dm9000_ether_crc_le(int32_t length, uint8_t *data, int32_t foxes);
extern uint8_t  dm9000_ior(DM9000_CONTEXT_STRUCT_PTR context, int32_t reg);
extern void    dm9000_iow(DM9000_CONTEXT_STRUCT_PTR context, int32_t reg, int32_t value);
extern int32_t  dm9000_phy_read(DM9000_CONTEXT_STRUCT_PTR context, int32_t reg);
extern void    dm9000_phy_write(DM9000_CONTEXT_STRUCT_PTR context, int32_t reg, int32_t value);
extern void    dm9000_set_io(DM9000_CONTEXT_STRUCT_PTR context, int32_t byte_width);
extern void    dm9000_isr(pointer  enet);
extern uint8_t dm9000_type_to_char(enum dm9000_type type);
extern int32_t dm9000_receive(ENET_CONTEXT_STRUCT_PTR enet_ptr);
extern void    dm9000_tx_done(ENET_CONTEXT_STRUCT_PTR enet_ptr);
extern int32_t  dm9000_reset(DM9000_CONTEXT_STRUCT_PTR context, _enet_address mac_addr);
extern void    dm9000_hash_table_unlocked(DM9000_CONTEXT_STRUCT_PTR context);
extern void    dm9000_read_eeprom(DM9000_CONTEXT_STRUCT_PTR context, int offset, uint8_t *to);
extern void    dm9000_write_eeprom(DM9000_CONTEXT_STRUCT_PTR context, int offset, uint8_t *data);
extern void    dm9000_get_mac_address(_enet_address address);
#ifdef CONFIG_DM9000_DEBUG
extern void    dm9000_dump_regs(DM9000_CONTEXT_STRUCT_PTR context);
#endif
#ifdef __cplusplus
}
#endif

#endif
/* EOF */

