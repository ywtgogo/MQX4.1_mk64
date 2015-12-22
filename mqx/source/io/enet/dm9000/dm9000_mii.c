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
*  Function Name  : MACNET_read_mii
*  Returned Value : TRUE and MII Data register value, FALSE if timeout
*  Comments       :
*    Return entire MII data register value
*
*END*-----------------------------------------------------------------*/

bool dm9000_read_mii
    (
        ENET_CONTEXT_STRUCT_PTR enet_ptr,
        uint32_t reg_index,
        uint32_t *data,
        uint32_t timeout
    )
{
    DM9000_CONTEXT_STRUCT_PTR context = (DM9000_CONTEXT_STRUCT_PTR)enet_ptr->MAC_CONTEXT_PTR;

    DM9000_DBG("entering %s\n", __func__);

    *data = dm9000_phy_read(context, reg_index);
    
    return TRUE;
}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : MACNET_write_mii
*  Returned Value : TRUE if success, FALSE if timeout
*  Comments       :
*    Write MII data register value
*
*END*-----------------------------------------------------------------*/

bool dm9000_write_mii
    (
        ENET_CONTEXT_STRUCT_PTR enet_ptr,
        uint32_t reg_index,
        uint32_t data,
        uint32_t timeout
    )
{
    DM9000_CONTEXT_STRUCT_PTR context = (DM9000_CONTEXT_STRUCT_PTR)enet_ptr->MAC_CONTEXT_PTR;

    DM9000_DBG("entering %s\n", __func__);

    dm9000_phy_write(context, reg_index, data);

    return TRUE;
}
