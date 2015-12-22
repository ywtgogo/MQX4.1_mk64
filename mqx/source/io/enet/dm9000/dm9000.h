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
#ifndef __DM9000_h__
#define __DM9000_h__

#include <bsp.h>

#define DM9000_DEVICE_MAC_1     1
#define DM9000_DEVICE_PHY_0     0

#define DM9000_DEVICE_COUNT 1
#define DM9000_BASE_PTR     0x80000000
#define DM9000_DATA_OFFSET  (0x2)
#define DM9000_ADDR         (DM9000_BASE_PTR)
#define DM9000_DATA         (DM9000_BASE_PTR + DM9000_DATA_OFFSET)
   
#define DM9000_RX_BUFFER_ALIGNMENT   16  
#define DM9000_TX_BUFFER_ALIGNMENT   4   
#define DM9000_BD_ALIGNMENT          16 // may be 4.

enum dm9000_type {
    TYPE_DM9000E,   /* original DM9000 */
    TYPE_DM9000A,
    TYPE_DM9000B
};

#endif
/* EOF */
