/*
*	定义全局使用的硬件资源 和函数接口
*/

#ifndef  _KWTJ_GATEWAY_H
#define  _KWTJ_GATEWAY_H

#include <mqx.h>
#include <bsp.h>
#include "psptypes.h"

#define			VERNO			0x00000033
#define			VERTIME			__DATE__

extern LWGPIO_STRUCT	wifi_power, wifi_reset, wifi_cfgset, wifi_cfgget, wifi_ap, wifi_preftcp, wifi_secdtcp;
extern LWGPIO_STRUCT    gw_monitor_led, gw_rf_led, gw_run_led, gw_plug_led, gw_ser_led, gw_red_led, gw_green_led;

extern _mqx_int  _io_serial_int_open(MQX_FILE_PTR, char *, char *);
extern _mqx_int  _io_serial_int_close(MQX_FILE_PTR);
extern _mqx_int  _io_serial_int_read(MQX_FILE_PTR, char *, _mqx_int);
extern _mqx_int  _io_serial_int_write(MQX_FILE_PTR, char *, _mqx_int);
extern _mqx_int  _io_serial_int_ioctl(MQX_FILE_PTR, _mqx_uint, void *);
//extern _mqx_int _io_serial_int_write_u8(MQX_FILE_PTR, uint8_t_ptr,uint8_t);

#define serial_open                             _io_serial_int_open
#define serial_close                            _io_serial_int_close
#define serial_write                            _io_serial_int_write
#define serial_ioctl                            _io_serial_int_ioctl
#define serial_read                             _io_serial_int_read




void gw_gpio_init();
void gw_serial_init();

void gw_sync_init();

void eth_plug_led_on();
void eth_plug_led_off();
void eth_plug_led_blink();
void eth_ser_led_on();
void eth_ser_led_off();
void eth_updata_led_on();
void eth_updata_led_off();
void rf_led_on();
void rf_led_off();
void monitor_led_on();
void monitor_led_off();
void run_led_blink();
void plug_led_bink();
void rf_led_blink();

void reset_cpu();
void reset_rf();
void reset_wifi();



#endif