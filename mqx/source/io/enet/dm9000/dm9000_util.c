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
#include "enetprv.h"
#include "dm9000_prv.h"
#include "dm9000.h"

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : dm9000_install_isr
*  Returned Value : ENET_OK or error code
*  Comments       :
*         
*
*END*-----------------------------------------------------------------*/
static bool dm9000_install_isr( 
   ENET_CONTEXT_STRUCT_PTR enet_ptr, 
   uint32_t                 int_num, 
   uint32_t                 int_index, 
   INT_ISR_FPTR            isr, 
   uint32_t                 level, 
   uint32_t                 sublevel  ) 
{
   uint32_t  vector = dm9000_get_vector(enet_ptr->PARAM_PTR->ENET_IF->PHY_NUMBER, int_index);
 
#if BSPCFG_ENET_RESTORE
   DM9000_CONTEXT_STRUCT_PTR context = (DM9000_CONTEXT_STRUCT_PTR)enet_ptr->MAC_CONTEXT_PTR;


   // Save old ISR and data
   context->OLDISR_PTR[int_num]   = _int_get_isr(vector);
   context->OLDISR_DATA[int_num] = _int_get_isr_data(vector);
#endif

   if (_int_install_isr(vector, isr, (pointer)enet_ptr)==NULL) {
      return FALSE;
   }

   // Initialize interrupt priority and level
   _bsp_int_init((PSP_INTERRUPT_TABLE_INDEX)vector, level, sublevel, TRUE);

   return TRUE;
}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : dm9000_install_isrs
*  Returned Value :  
*  Comments       :
*        
*
*END*-----------------------------------------------------------------*/
bool dm9000_install_isrs(ENET_CONTEXT_STRUCT_PTR enet_ptr, DM9000_INIT_STRUCT const *enet_init_ptr) 
{
    bool bOK;
    bOK = dm9000_install_isr(enet_ptr, 0, DM9000_INT_RX_TX, dm9000_isr, enet_init_ptr->ETX_LEVEL, enet_init_ptr->ETX_SUBLEVEL) ;
   
   return bOK;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : dm9000_mask_interrupts
*  Returned Value :  
*  Comments       :
*        
*
*END*-----------------------------------------------------------------*/
void dm9000_mask_interrupts(ENET_CONTEXT_STRUCT_PTR enet_ptr) 
{   uint32_t i;

    for (i = 0; i < DM9000_NUM_INTS; ++i)
        _bsp_int_disable((PSP_INTERRUPT_TABLE_INDEX)(dm9000_get_vector(enet_ptr->PARAM_PTR->ENET_IF->PHY_NUMBER, i)) );
}

#if BSPCFG_ENET_RESTORE
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : dm9000_uninstall_isr
*  Returned Value :  
*  Comments       :
*        
*
*END*-----------------------------------------------------------------*/
static void dm9000_uninstall_isr(ENET_CONTEXT_STRUCT_PTR enet_ptr, uint32_t int_num, uint32_t int_index) 
{
    uint32_t  vector = dm9000_get_vector(enet_ptr->PARAM_PTR->ENET_IF->PHY_NUMBER, int_index);
    DM9000_CONTEXT_STRUCT_PTR    context = (DM9000_CONTEXT_STRUCT_PTR)enet_ptr->MAC_CONTEXT_PTR;
 
    _bsp_int_disable((PSP_INTERRUPT_TABLE_INDEX)vector);
    if (context->OLDISR_PTR[int_num]) {
        _int_install_isr(vector, context->OLDISR_PTR[int_num], context->OLDISR_DATA[int_num]);
        context->OLDISR_PTR[int_num] = (INT_ISR_FPTR)NULL;
    }
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : dm9000_uninstall_all_isrs
*  Returned Value :  
*  Comments       :
*        
*
*END*-----------------------------------------------------------------*/
void dm9000_uninstall_all_isrs(ENET_CONTEXT_STRUCT_PTR enet_ptr) 
{
   dm9000_uninstall_isr(enet_ptr, 0, DM9000_INT_RX_TX) ;
}
#endif


/*FUNCTION*-------------------------------------------------------------
* 
* Function Name    : dm9000_get_vector
* Returned Value   : MQX vector number for specified interrupt
* Comments         :
*    This function returns index into MQX interrupt vector table for
*    specified enet interrupt. If not known, returns 0.
*
*END*-----------------------------------------------------------------*/
uint32_t dm9000_get_vector 
(
    uint32_t device,
    uint32_t vector_index
)
{ 
    uint32_t index = 0;

    if ((device < DM9000_DEVICE_COUNT) && (vector_index < DM9000_NUM_INTS)) {
        index = dm9000_vectors[device][vector_index];
    }

    return index;
} 


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : dm9000_free_context
*  Returned Value :  
*  Comments       :
*        
*
*END*-----------------------------------------------------------------*/

void dm9000_free_context(DM9000_CONTEXT_STRUCT_PTR context) 
{
    if (context) {
        if (context->UNALIGNED_BUFFERS) {
            _mem_free((pointer)context->UNALIGNED_BUFFERS);
        }
        if (context->RX_PCB_BASE) {
         _mem_free((pointer)context->RX_PCB_BASE);
        }
        _mem_free((pointer)context);
    }
}

#ifdef CONFIG_DM9000_DEBUG
void dm9000_dump_regs(DM9000_CONTEXT_STRUCT_PTR context)
{
	DM9000_DBG("\n");
	DM9000_DBG("NCR     (0x00): %02x\n", dm9000_ior(context, DM9000_NCR));
	DM9000_DBG("NSR     (0x01): %02x\n", dm9000_ior(context, DM9000_NSR));
	DM9000_DBG("TCR     (0x02): %02x\n", dm9000_ior(context, DM9000_TCR));
	DM9000_DBG("TSRI    (0x03): %02x\n", dm9000_ior(context, DM9000_TSR1));
	DM9000_DBG("TSRII   (0x04): %02x\n", dm9000_ior(context, DM9000_TSR2));
	DM9000_DBG("RCR     (0x05): %02x\n", dm9000_ior(context, DM9000_RCR));
	DM9000_DBG("RSR     (0x06): %02x\n", dm9000_ior(context, DM9000_RSR));
    DM9000_DBG("ROCR    (0x07): %02x\n", dm9000_ior(context, DM9000_ROCR));
    DM9000_DBG("BPTR    (0x08): %02x\n", dm9000_ior(context, DM9000_BPTR));
    DM9000_DBG("FCTR    (0x09): %02x\n", dm9000_ior(context, DM9000_FCTR));
    DM9000_DBG("FCR     (0x0a): %02x\n", dm9000_ior(context, DM9000_FCR));
    DM9000_DBG("CHIPR   (0x2C): %02x\n", dm9000_ior(context, DM9000_CHIPR));
    DM9000_DBG("TCR2    (0x2D): %02x\n", dm9000_ior(context, DM9000_TCR2));
    DM9000_DBG("OCR     (0x2E): %02x\n", dm9000_ior(context, DM9000_OCR));
    DM9000_DBG("SMCR    (0x2F): %02x\n", dm9000_ior(context, DM9000_SMCR));
    DM9000_DBG("ETXCSR  (0x30): %02x\n", dm9000_ior(context, DM9000_ETXCSR));
    DM9000_DBG("TCSCR   (0x31): %02x\n", dm9000_ior(context, DM9000_TCSCR));
    DM9000_DBG("RCSCSR  (0x31): %02x\n", dm9000_ior(context, DM9000_RCSCSR));
    DM9000_DBG("RSR     (0x06): %02x\n", dm9000_ior(context, DM9000_RSR));
	DM9000_DBG("ISR     (0xFE): %02x\n", dm9000_ior(context, DM9000_ISR));
    DM9000_DBG("IMR     (0xFF): %02x\n", dm9000_ior(context, DM9000_IMR));
	DM9000_DBG("\n");
}
#endif

static void dm9000_outblk_8bit(void *reg, volatile void *data_ptr, int32_t count)
{
	int32_t i;
	for (i = 0; i < count; i++)
		dm9000_outb((((uint8_t *)data_ptr)[i] & 0xff), reg);
}

static void dm9000_outblk_16bit(void *reg, volatile void *data_ptr, int32_t count)
{
	int32_t i;
	uint32_t tmplen = (count + 1) / 2;

	for (i = 0; i < tmplen; i++)
		dm9000_outw(((uint16_t *)data_ptr)[i], reg);
}

static void dm9000_outblk_32bit(void *reg, volatile void *data_ptr, int32_t count)
{
	int32_t i;
	uint32_t tmplen = (count + 3) / 4;

	for (i = 0; i < tmplen; i++)
		dm9000_outl(((uint32_t *)data_ptr)[i], reg);
}

static void dm9000_inblk_8bit(void *reg, volatile void *data_ptr, int32_t count)
{
	int32_t i;
	for (i = 0; i < count; i++)
		((uint8_t *)data_ptr)[i] = dm9000_inb(reg);
}

static void dm9000_inblk_16bit(void *reg, volatile void *data_ptr, int32_t count)
{
	int32_t i;
	uint32_t tmplen = (count + 1) / 2;

	for (i = 0; i < tmplen; i++)
		((uint16_t *)data_ptr)[i] = dm9000_inw(reg);
}

static void dm9000_inblk_32bit(void *reg, volatile void *data_ptr, int32_t count)
{
	int i;
	uint32_t tmplen = (count + 3) / 4;

	for (i = 0; i < tmplen; i++)
		((uint32_t *)data_ptr)[i] = dm9000_inl(reg);
}

/* dump block from chip to null */
static void dm9000_dumpblk_8bit(void *reg, int32_t count)
{
	int32_t i;
	int32_t tmp;

	for (i = 0; i < count; i++)
		tmp = dm9000_inb(reg);
}

static void dm9000_dumpblk_16bit(void *reg, int32_t count)
{
	int32_t i;
	int32_t tmp;

	count = (count + 1) >> 1;

	for (i = 0; i < count; i++)
		tmp = dm9000_inw(reg);
}

static void dm9000_dumpblk_32bit(void *reg, int32_t count)
{
	int32_t i;
	int32_t tmp;

	count = (count + 3) >> 2;

	for (i = 0; i < count; i++)
		tmp = dm9000_inl(reg);
}

/*
 * Read a byte from I/O port
 */
uint8_t dm9000_ior(DM9000_CONTEXT_STRUCT_PTR context, int32_t reg)
{
    dm9000_outb(reg, context->io_addr);
    return dm9000_inb(context->io_data);
}

/*
 * Write a byte to I/O port
 */
void dm9000_iow(DM9000_CONTEXT_STRUCT_PTR context, int32_t reg, int32_t value)
{
      dm9000_outb(reg, context->io_addr);
      dm9000_outb(value, context->io_data);
}

int32_t dm9000_phy_read(DM9000_CONTEXT_STRUCT_PTR context, int32_t reg)
{
	uint32_t reg_save;
	int32_t ret;

    dm9000_int_disable();
    
    /* Save previous register address */
    reg_save = dm9000_inb(context->io_addr);

	/* Fill the phyxcer register into REG_0C */
	dm9000_iow(context, DM9000_EPAR, DM9000_PHY | reg);

	/* Issue phyxcer read command */
	dm9000_iow(context, DM9000_EPCR, EPCR_ERPRR | EPCR_EPOS);
    
    /* restore the previous address */
    dm9000_outb(reg_save, context->io_addr);

    dm9000_int_enable();

	_time_delay(1);		/* Wait read complete */

    dm9000_int_disable();
    
    /* Save previous register address */
    reg_save = dm9000_inb(context->io_addr);

	dm9000_iow(context, DM9000_EPCR, 0x0);	/* Clear phyxcer read command */

	/* The read data keeps on REG_0D & REG_0E */
	ret = (dm9000_ior(context, DM9000_EPDRH) << 8) | dm9000_ior(context, DM9000_EPDRL);

    /* restore the previous address */
    dm9000_outb(reg_save, context->io_addr);

    dm9000_int_enable();

#ifdef CONFIG_DM9000_DEBUG_PHY
	DM9000_DBG("phy_read[%02x] -> %04x\n", reg, ret);
#endif

	return ret;
}

void dm9000_phy_write(DM9000_CONTEXT_STRUCT_PTR context, int32_t reg, int32_t value)
{
	unsigned long reg_save;

	DM9000_DBG("phy_write[%02x] = %04x\n", reg, value);
    dm9000_int_disable();

    /* Save previous register address */
    reg_save = dm9000_inb(context->io_addr);

	/* Fill the phyxcer register into REG_0C */
    dm9000_iow(context, DM9000_EPAR, DM9000_PHY | reg);

	/* Fill the written data into REG_0D & REG_0E */
	dm9000_iow(context, DM9000_EPDRL, value & 0xff);
	dm9000_iow(context, DM9000_EPDRH, (value >> 8) & 0xff);

	/* Issue phyxcer write command */
	dm9000_iow(context, DM9000_EPCR, EPCR_EPOS | EPCR_ERPRW);

    /* restore the previous address */
    dm9000_outb(reg_save, context->io_addr);

    dm9000_int_enable();
	_time_delay(1);		/* Wait write complete */

    dm9000_int_disable();
    
    /* Save previous register address */
    reg_save = dm9000_inb(context->io_addr);

	dm9000_iow(context, DM9000_EPCR, 0x0);	/* Clear phyxcer write command */
    
    /* restore the previous address */
    dm9000_outb(reg_save, context->io_addr);
    dm9000_int_enable();
}

void dm9000_set_io(DM9000_CONTEXT_STRUCT_PTR context, int32_t byte_width)
{
	/* use the size of the data resource to work out what IO
	 * routines we want to use
	 */

	switch (byte_width) {
	case 1:
		context->dumpblk = dm9000_dumpblk_8bit;
		context->outblk  = dm9000_outblk_8bit;
		context->inblk   = dm9000_inblk_8bit;
		break;
	case 3:
		DM9000_DBG("3 byte IO, falling back to 16bit\n");
	case 2:
		context->dumpblk = dm9000_dumpblk_16bit;
		context->outblk  = dm9000_outblk_16bit;
		context->inblk   = dm9000_inblk_16bit;
		break;
	case 4:
	default:
		context->dumpblk = dm9000_dumpblk_32bit;
		context->outblk  = dm9000_outblk_32bit;
		context->inblk   = dm9000_inblk_32bit;
		break;
	}
}

static uint32_t dm9000_read_locked(DM9000_CONTEXT_STRUCT_PTR context, int reg)
{
	unsigned int ret;

    dm9000_int_disable();
	ret = dm9000_ior(context, reg);
    dm9000_int_enable();

	return ret;
}

static int dm9000_wait_eeprom(DM9000_CONTEXT_STRUCT_PTR context)
{
	unsigned int status;
	int timeout = 8;	/* wait max 8msec */

	/* The DM9000 data sheets say we should be able to
	 * poll the ERRE bit in EPCR to wait for the EEPROM
	 * operation. From testing several chips, this bit
	 * does not seem to work.
	 *
	 * We attempt to use the bit, but fall back to the
	 * timeout (which is why we do not return an error
	 * on expiry) to say that the EEPROM operation has
	 * completed.
	 */

	while (1) {
		status = dm9000_read_locked(context, DM9000_EPCR);

		if ((status & EPCR_ERRE) == 0)
			break;

		_time_delay(1);

		if (timeout-- < 0) {
			DM9000_DBG("timeout waiting EEPROM\n");
			break;
		}
	}

	return 0;
}

/*
 *  Read a word data from EEPROM
 */
void dm9000_read_eeprom(DM9000_CONTEXT_STRUCT_PTR context, int offset, uint8_t *to)
{
    dm9000_int_disable();

	dm9000_iow(context, DM9000_EPAR, offset);
	dm9000_iow(context, DM9000_EPCR, EPCR_ERPRR);
    
    dm9000_int_enable();

	dm9000_wait_eeprom(context);

	/* delay for at-least 150uS */
	_time_delay(1);

    dm9000_int_disable();
	dm9000_iow(context, DM9000_EPCR, 0x0);

	to[0] = dm9000_ior(context, DM9000_EPDRL);
	to[1] = dm9000_ior(context, DM9000_EPDRH);

    dm9000_int_enable();
}

/*
 * Write a word data to SROM
 */
void dm9000_write_eeprom(DM9000_CONTEXT_STRUCT_PTR context, int offset, uint8_t *data)
{
    dm9000_int_disable();

	dm9000_iow(context, DM9000_EPAR, offset);
	dm9000_iow(context, DM9000_EPDRH, data[1]);
	dm9000_iow(context, DM9000_EPDRL, data[0]);
	dm9000_iow(context, DM9000_EPCR, EPCR_WEP | EPCR_ERPRW);
    
    dm9000_int_enable();

	dm9000_wait_eeprom(context);

	_time_delay(1);	/* wait at least 150uS to clear */

    dm9000_int_disable();
	dm9000_iow(context, DM9000_EPCR, 0);
    dm9000_int_enable();
}

uint8_t dm9000_type_to_char(enum dm9000_type type)
{
	switch (type) {
	case TYPE_DM9000E: return 'e';
	case TYPE_DM9000A: return 'a';
	case TYPE_DM9000B: return 'b';
	}

	return '?';
}

const uint32_t ethernet_polynomial_le = 0xedb88320U;

//bit-oriented implementation: processes a byte array.
uint32_t dm9000_ether_crc_le(int32_t length, uint8_t *data, int32_t foxes)
{
    uint32_t crc = (foxes) ? 0xffffffff: 0;  /* Initial value. */

    while (--length >= 0) 
    {
        unsigned char current_octet = *data++;
        int bit;

        DM9000_DBG("%02X, %08X,  inv %08X\n", current_octet, crc, ~crc);

        for (bit = 8; --bit >= 0; current_octet >>= 1) {
            if ((crc ^ current_octet) & 1) {
                crc >>= 1;
                crc ^= ethernet_polynomial_le;
            } else
                crc >>= 1;
        }
    }
    DM9000_DBG("crc final %x\n", crc);
    return crc;
}

void dm9000_hash_table_unlocked(DM9000_CONTEXT_STRUCT_PTR context)
{
    int i, oft;
    uint16_t hash_table[4];
    uint8_t rcr = RCR_DIS_LONG | RCR_DIS_CRC | RCR_RXEN;

    DM9000_DBG("entering %s\n", __func__);

    /* Clear Hash Table */
    for (i = 0; i < 4; i++)
        hash_table[i] = 0x0;

    /* broadcast address */
    hash_table[3] = 0x8000;

    /* Write the hash table to MAC MD table */
    for (i = 0, oft = DM9000_MAR; i < 4; i++) {
        dm9000_iow(context, oft++, hash_table[i]);
        dm9000_iow(context, oft++, hash_table[i] >> 8);
    }

    dm9000_iow(context, DM9000_RCR, rcr);
}

extern _enet_address dm9000_mac_addr;

void dm9000_get_mac_address(_enet_address address)
{
    _mem_copy(dm9000_mac_addr, address, sizeof(_enet_address));
}

/* EOF */
