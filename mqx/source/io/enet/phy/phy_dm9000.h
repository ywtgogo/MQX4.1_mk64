/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
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
#ifndef _phy_dm9000_h_
#define _phy_dm9000_h_ 1

#ifdef __cplusplus
extern "C" {
#endif

/*
** Register definitions for the PHY.
*/

enum dm9000_phy_reg {
   PHY_DM9000_MII_REG_BMCR       = 0x0,  // Control Register
   PHY_DM9000_MII_REG_BMSR       = 0x1,  // Status Register
   PHY_DM9000_MII_REG_PHYID1     = 0x2,  // PHY Identification Register 1
   PHY_DM9000_MII_REG_PHYID2     = 0x3,  // PHY Identification Register 2
   PHY_DM9000_MII_REG_ANAR       = 0x4,  // A-N Advertisement Register
   PHY_DM9000_MII_REG_ANLPAR     = 0x5,  // A-N Link Partner Ability Register
   PHY_DM9000_MII_REG_ANER       = 0x6,  // A-N Expansion Register

   PHY_DM9000_MII_REG_DSCR       = 0x10,  // Specified Configuration
   PHY_DM9000_MII_REG_DSCSR      = 0x11,  // Specified Configuration and Status
   PHY_DM9000_MII_REG_10BTCSR    = 0x12,  // 10BASE-T Configuration/Status
   PHY_DM9000_MII_REG_PWDOR      = 0x13,  // Power Down Control
   PHY_DM9000_MII_REG_SC         = 0x14,  // Specified config
};


// values for PHY_MII_REG_CR Status Register

#define PHY_DM9000_MII_REG_BMCR_AN_ENABLE   0x1000
#define PHY_DM9000_MII_REG_BMCR_LOOP        0x4000
#define PHY_DM9000_MII_REG_BMCR_RESET       0x8000


// values for PHY_MII_REG_SR Status Register

#define PHY_DM9000_MII_REG_BMSR_EXTCAP        0x0001
#define PHY_DM9000_MII_REG_BMSR_JABBER        0x0002
#define PHY_DM9000_MII_REG_BMSR_LINK_STATUS   0x0004
#define PHY_DM9000_MII_REG_BMSR_AN_ABLE       0x0008 // Auto-negotiate ability
#define PHY_DM9000_MII_REG_BMSR_REMOTE_FAULT  0x0010
#define PHY_DM9000_MII_REG_BMSR_AN_COMPLETE   0x0020 // Auto-negotiate completed


// values for PHY_MII_REG_DSCSR Status Register
#define PHY_DM9000_MII_REG_DSCSR_OP_MODE_MASK    0xf000
#define PHY_DM9000_MII_REG_DSCSR_OP_MODE_10_HD   0x1000 
#define PHY_DM9000_MII_REG_DSCSR_OP_MODE_10_FD   0x2000 
#define PHY_DM9000_MII_REG_DSCSR_OP_MODE_100_HD  0x4000 
#define PHY_DM9000_MII_REG_DSCSR_OP_MODE_100_FD  0x8000 


extern const ENET_PHY_IF_STRUCT phy_dm9000_IF;
 
#define DM9000_MII_TIMEOUT                     (0x10000)

#ifdef __cplusplus
}
#endif

#endif /* _phy_ksz8041_h_ */
/* EOF */
