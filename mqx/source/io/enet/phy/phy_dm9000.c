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
#include "bsp_prv.h"
#include "enet.h"
#include "enetprv.h"
#include "phy_dm9000.h"

static bool phy_dm9000_discover_addr(ENET_CONTEXT_STRUCT_PTR enet_ptr);
static bool phy_dm9000_init(ENET_CONTEXT_STRUCT_PTR enet_ptr);
static uint32_t phy_dm9000_get_speed(ENET_CONTEXT_STRUCT_PTR enet_ptr);
static bool phy_dm9000_get_link_status(ENET_CONTEXT_STRUCT_PTR enet_ptr);

const ENET_PHY_IF_STRUCT phy_dm9000_IF = {
  phy_dm9000_discover_addr,
  phy_dm9000_init,
  phy_dm9000_get_speed,
  phy_dm9000_get_link_status
};
  
  

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : phy_ksz8041_discover_addr
*  Returned Value : none
*  Comments       :
*    Scan possible PHY addresses looking for a valid device
*
*END*-----------------------------------------------------------------*/

static bool phy_dm9000_discover_addr
   (
       ENET_CONTEXT_STRUCT_PTR     enet_ptr
   )
{ 
   uint32_t              i;
   uint32_t              id;

   for (i = 0; i < 32; i++) {
      id = 0;
      enet_ptr->PHY_ADDRESS = i;
      if ((*enet_ptr->PARAM_PTR->ENET_IF->MAC_IF->PHY_READ)(enet_ptr, PHY_DM9000_MII_REG_PHYID1, &id, DM9000_MII_TIMEOUT)) 
      {
          if ((id != 0) && (id != 0xffff)) 
          {
                return TRUE;
          }
      } 
   } 
   
   return FALSE; 
} 


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : phy_dm9000_init
*  Returned Value : bool
*  Comments       :
*    Wait for PHY to automatically negotiate its configuration
*
*END*-----------------------------------------------------------------*/

static bool phy_dm9000_init
   (
       ENET_CONTEXT_STRUCT_PTR     enet_ptr
   )
{ 
   uint32_t              phy_status=0;

   if (enet_ptr->PARAM_PTR->OPTIONS & ENET_OPTION_PHY_LOOPBACK) {
      if ((*enet_ptr->PARAM_PTR->ENET_IF->MAC_IF->PHY_READ)(enet_ptr, PHY_DM9000_MII_REG_BMCR, &phy_status, DM9000_MII_TIMEOUT)) {
         phy_status |= PHY_DM9000_MII_REG_BMCR_LOOP;
         (*enet_ptr->PARAM_PTR->ENET_IF->MAC_IF->PHY_WRITE)(enet_ptr, PHY_DM9000_MII_REG_BMCR, phy_status, DM9000_MII_TIMEOUT);
         return TRUE;
      }
   } else {
      if ((*enet_ptr->PARAM_PTR->ENET_IF->MAC_IF->PHY_READ)(enet_ptr, PHY_DM9000_MII_REG_BMSR, &phy_status, DM9000_MII_TIMEOUT)) {
         if (phy_status & PHY_DM9000_MII_REG_BMSR_AN_ABLE) { 
            // Has auto-negotiate ability
            int i;
            for (i = 0; i < 3 * BSP_ALARM_FREQUENCY; i++) {
               if ((*enet_ptr->PARAM_PTR->ENET_IF->MAC_IF->PHY_READ)(enet_ptr, PHY_DM9000_MII_REG_BMSR, &phy_status, DM9000_MII_TIMEOUT)) 
                  if ((phy_status & PHY_DM9000_MII_REG_BMSR_AN_COMPLETE) != 0) 
                     return TRUE;
               _time_delay(BSP_ALARM_RESOLUTION);
            }  
         }
         return TRUE;
      }  
   }  
   return FALSE;
}  


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : phy_dm9000_get_speed
*  Returned Value : uint32_t - connection speed
*  Comments       :
*    Determine if connection speed is 10 or 100 Mbit
*
*END*-----------------------------------------------------------------*/

static uint32_t phy_dm9000_get_speed
   (
       ENET_CONTEXT_STRUCT_PTR     enet_ptr
   )
{ 
   uint32_t speed_status;

   if ((*enet_ptr->PARAM_PTR->ENET_IF->MAC_IF->PHY_READ)(enet_ptr, PHY_DM9000_MII_REG_DSCSR, &speed_status, DM9000_MII_TIMEOUT)) {
      speed_status &= PHY_DM9000_MII_REG_DSCSR_OP_MODE_MASK;
  
      if ((PHY_DM9000_MII_REG_DSCSR_OP_MODE_10_HD == speed_status) ||
          (PHY_DM9000_MII_REG_DSCSR_OP_MODE_10_FD == speed_status)) {
         return 10;
      } else if ((PHY_DM9000_MII_REG_DSCSR_OP_MODE_100_HD == speed_status) ||
                 (PHY_DM9000_MII_REG_DSCSR_OP_MODE_100_FD == speed_status)) {
         return 100;
      }
   }
   return 0;
} 

  
/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : phy_dm9000_get_link_status
*  Returned Value : TRUE if link active, FALSE otherwise
*  Comments       : 
*    Get actual link status.
*
*END*-----------------------------------------------------------------*/

static bool phy_dm9000_get_link_status
   (
       ENET_CONTEXT_STRUCT_PTR     enet_ptr
   )
{ 
   uint32_t data;
   bool res = FALSE;
   
   if ((*enet_ptr->PARAM_PTR->ENET_IF->MAC_IF->PHY_READ)(enet_ptr, PHY_DM9000_MII_REG_BMCR, &data, DM9000_MII_TIMEOUT))
   {
       /* After power up, when PHY does not fully initial PHY_DM9000_MII_REG_SR_LINK_STATUS bit 
        * is '1'. So if we get link status in this case, the return result is always "Connected" 
        * even when we haven't plug the cable in yet. 
        * Check if PHY in normal operation -> ready to get link status */
       if ((data & PHY_DM9000_MII_REG_BMCR_RESET) == 0 )
       {
           /* Some PHY (e.g.DP8340) returns "unconnected" and than "connected" state
           *  just to show that was transition event from one state to another.
           *  As we need only curent state,  read 2 times and return 
           *  the current/latest state. */
           if ((*enet_ptr->PARAM_PTR->ENET_IF->MAC_IF->PHY_READ)(enet_ptr, PHY_DM9000_MII_REG_BMSR, &data, DM9000_MII_TIMEOUT)) {
              if ((*enet_ptr->PARAM_PTR->ENET_IF->MAC_IF->PHY_READ)(enet_ptr, PHY_DM9000_MII_REG_BMSR, &data, DM9000_MII_TIMEOUT))
              {
                 res = ((data & PHY_DM9000_MII_REG_BMSR_LINK_STATUS) != 0) ? TRUE : FALSE;
              }
           }
       }
   }

  return res;
}
