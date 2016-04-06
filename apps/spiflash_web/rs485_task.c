/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
**************************************************************************
*
* $FileName: rs485_task.c$
* $Version : 3.8.4.0$
* $Date    : May-22-2013$
*
* Comments:
*
*   This file contains the source for the rs485 example program.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h> 
#include <fio.h>
#include <string.h>

/*TASK*-----------------------------------------------------
* 
* Task Name    : rs485_write_task
* Comments     :
*
*END*-----------------------------------------------------*/
void RS485_Write_Task
   (
      uint32_t initial_data
   )
{
   MQX_FILE_PTR rs485_dev = NULL;
   char data_buffer[] = "\r\nabcdefghijklmnopqrstuvwxyz1234567890\r\n";
   bool disable_rx = TRUE;
   uint32_t result;

   if(initial_data == 1)
   {
       rs485_dev  = fopen( "ttye:", NULL );      
   }
   if(initial_data == 3)
   {
       rs485_dev  = fopen( "ttyf:", NULL );  
   }                    
   
   if( rs485_dev == NULL )
   {
      _task_block();
   }

   /* half duplex, two wire. disable receiver in transmit */
   /* bsp will disable receiver automatically when invoke write, just to show the flow control here */
   result = ioctl( rs485_dev, IO_IOCTL_SERIAL_DISABLE_RX, &disable_rx ); 
   if( result == IO_ERROR_INVALID_IOCTL_CMD )
   {
      _task_block();
   }

   write( rs485_dev, data_buffer, strlen(data_buffer) );
   
   /* empty queue - not needed for polled mode */
   //fflush( rs485_dev );
   
   /* wait for transfer complete flag */
   result = ioctl( rs485_dev, IO_IOCTL_SERIAL_WAIT_FOR_TC, NULL );
   if( result == IO_ERROR_INVALID_IOCTL_CMD )
   {
      _task_block();
   }
   
   /* half duplex, two wire. if receiver was disabled before, enable receiver again */  
   disable_rx = FALSE;
   ioctl( rs485_dev, IO_IOCTL_SERIAL_DISABLE_RX, &disable_rx ) ;

   fclose(rs485_dev);

   _task_block();
}
/*TASK*-----------------------------------------------------
* 
* Task Name    : rs485_read_task
* Comments     :
*
*END*-----------------------------------------------------*/
void RS485_Read_Task
   (
      uint32_t initial_data
   )
{
   MQX_FILE_PTR rs485_dev = NULL;
   char data_buffer[256];

   if(initial_data == 0)
   {
       rs485_dev  = fopen( "ttye:", NULL );      
   }
   if(initial_data == 2)
   {
       rs485_dev  = fopen( "ttyf:", NULL );  
   }                      
   
   if(rs485_dev == NULL)
   {
      _task_block();
   }

   fprintf(rs485_dev,"\r\nplease send data to board, press 'x' to exit\r\n");

   while(TRUE)
   {
      if(fstatus( rs485_dev ))
      {
         data_buffer[0] = fgetc( rs485_dev );
		 if(data_buffer[0]=='x')
            break;

         printf("get data: 0x%x \n", data_buffer[0]);
      }
   }
	  
   fclose(rs485_dev);

   _task_block();
}
/* EOF */
