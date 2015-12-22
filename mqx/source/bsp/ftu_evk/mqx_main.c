/*****************************************************************************
*
* Comments:
*
*   This file contains the main C language entrypoint, starting up the MQX
*
*
*END************************************************************************/

#include "mqx.h"
#include "bsp.h"

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : main
* Returned Value   : should return "status"
* Comments         :
*   Starts MQX running
*
*END*----------------------------------------------------------------------*/
int main
   (
      void
   )
{ /* Body */

   extern const MQX_INITIALIZATION_STRUCT MQX_init_struct;

   /* Start MQX */
   _mqx( (MQX_INITIALIZATION_STRUCT_PTR) &MQX_init_struct );
   return 0;

} /* Endbody */