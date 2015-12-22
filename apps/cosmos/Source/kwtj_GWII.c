#include "global.h"
#include "kwtj_GWII.h"


LWGPIO_STRUCT       		gw_monitor_led, gw_rf_led, gw_run_led, gw_plug_led, gw_ser_led, gw_red_led, gw_green_led;
static LWGPIO_STRUCT		gw_rf_power, gw_rf_reset, gw_eth_reset, gw_wdg_feed;
static LWGPIO_STRUCT		gw_monitor_sw;
LWGPIO_STRUCT				wifi_power, wifi_reset, wifi_cfgset, wifi_cfgget, wifi_ap, wifi_preftcp, wifi_secdtcp;

MQX_FILE_PTR               	fd_ittye_485_ptr;
MQX_FILE_PTR               	fd_ittya_wifi_ptr;
 
void external_wdg_reset(void)
{
	lwgpio_toggle_value(&gw_wdg_feed);
	_time_delay(100);
	lwgpio_toggle_value(&gw_wdg_feed);
	_time_delay(100);
	lwgpio_toggle_value(&gw_wdg_feed);
	_time_delay(100);
	lwgpio_toggle_value(&gw_wdg_feed);
}

static void new_tamper_alarm_on_isr(void * pin)
{
    //lwgpio_int_enable(&gw_monitor_sw, FALSE);
    lwgpio_int_clear_flag((LWGPIO_STRUCT_PTR) pin);
//    lwgpio_set_value(&gw_rf_power, LWGPIO_VALUE_LOW);
	lwgpio_toggle_value(&gw_red_led);
}

void gw_gpio_init()
{
	lwgpio_init(&gw_rf_power, BSP_POWER_RF, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_LOW);
	lwgpio_set_functionality(&gw_rf_power, LWGPIO_MUX_E12_GPIO);
	lwgpio_set_value(&gw_rf_power, LWGPIO_VALUE_HIGH); // LWGPIO_VALUE_HIGH
	
	lwgpio_init(&gw_rf_reset, BSP_RESET_RF, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_HIGH);
	lwgpio_set_functionality(&gw_rf_reset, LWGPIO_MUX_E11_GPIO);
	lwgpio_set_value(&gw_rf_reset, LWGPIO_VALUE_HIGH);
	
	lwgpio_init(&gw_eth_reset, BSP_RESET_ETH, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_HIGH);
	lwgpio_set_functionality(&gw_eth_reset, LWGPIO_MUX_B8_GPIO);
	lwgpio_set_value(&gw_eth_reset, LWGPIO_VALUE_HIGH);
#if 1
	lwgpio_init(&wifi_power, BSP_POWER_WIFI, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE);
	lwgpio_set_functionality(&wifi_power, BSP_WPOWER_MUX_GPIO);
	lwgpio_set_value(&wifi_power, LWGPIO_VALUE_HIGH);

	lwgpio_init(&wifi_reset, BSP_RESET_WIFI, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE);
	lwgpio_set_functionality(&wifi_reset, BSP_WRESET_MUX_GPIO);
	lwgpio_set_value(&wifi_reset, LWGPIO_VALUE_HIGH);	

	lwgpio_init(&wifi_cfgset, BSP_CFGSET_WIFI, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE);
	lwgpio_set_functionality(&wifi_cfgset, BSP_CFGSET_MUX_GPIO);
	lwgpio_set_value(&wifi_cfgset, LWGPIO_VALUE_HIGH);	

	lwgpio_init(&wifi_cfgget, BSP_CFGGET_WIFI, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE);
	lwgpio_set_functionality(&wifi_cfgget, BSP_CFGGET_MUX_GPIO);
	lwgpio_set_attribute(&wifi_cfgget, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
	
	lwgpio_init(&wifi_ap, BSP_AP_WIFI, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE);
	lwgpio_set_functionality(&wifi_ap, BSP_AP_MUX_GPIO);
	lwgpio_set_attribute(&wifi_ap, LWGPIO_ATTR_PULL_DOWN, LWGPIO_AVAL_ENABLE);

	lwgpio_init(&wifi_preftcp, BSP_PREF_WTCP, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE);
	lwgpio_set_functionality(&wifi_preftcp, BSP_PREF_MUX_GPIO);
	lwgpio_set_attribute(&wifi_preftcp, LWGPIO_ATTR_PULL_DOWN, LWGPIO_AVAL_ENABLE);

	lwgpio_init(&wifi_secdtcp, BSP_SECD_WTCP, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE);
	lwgpio_set_functionality(&wifi_secdtcp, BSP_SECD_MUX_GPIO);
	lwgpio_set_attribute(&wifi_secdtcp, LWGPIO_ATTR_PULL_DOWN, LWGPIO_AVAL_ENABLE);
#endif //wifi control gpio
	
	lwgpio_init(&gw_wdg_feed, BSP_FEED_WDG, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_HIGH);
	lwgpio_set_functionality(&gw_wdg_feed, LWGPIO_MUX_A19_GPIO);
	lwgpio_set_value(&gw_wdg_feed, LWGPIO_VALUE_HIGH);
	
    lwgpio_init(&gw_run_led, BSP_BEAT_LED, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_LOW);
    lwgpio_set_functionality(&gw_run_led, BSP_BEATLED_MUX_GPIO);
    lwgpio_set_value(&gw_run_led, LWGPIO_VALUE_LOW);   
	_time_delay(50);
	lwgpio_init(&gw_plug_led, BSP_PLUG_LED, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_LOW);
    lwgpio_set_functionality(&gw_plug_led, BSP_PLUGLED_MUX_GPIO);
    lwgpio_set_value(&gw_plug_led, LWGPIO_VALUE_LOW);  
	_time_delay(50);
    lwgpio_init(&gw_ser_led, BSP_SER_LED, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_LOW);
    lwgpio_set_functionality(&gw_ser_led, BSP_SERLED_MUX_GPIO);
    lwgpio_set_value(&gw_ser_led, LWGPIO_VALUE_LOW);  
    _time_delay(50);
	lwgpio_init(&gw_rf_led, BSP_RF_LED, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_LOW);
    lwgpio_set_functionality(&gw_rf_led, BSP_RFLED_MUX_GPIO);
    lwgpio_set_value(&gw_rf_led, LWGPIO_VALUE_LOW);
    _time_delay(50);
	lwgpio_init(&gw_red_led, BSP_RED_LED, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_LOW);
    lwgpio_set_functionality(&gw_red_led, BSP_REDLED_MUX_GPIO);
    lwgpio_set_value(&gw_red_led, LWGPIO_VALUE_LOW);
    _time_delay(50);
	lwgpio_init(&gw_green_led, BSP_GREEN_LED, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_HIGH);
    lwgpio_set_functionality(&gw_green_led, BSP_GREENLED_MUX_GPIO);
    lwgpio_set_value(&gw_green_led, LWGPIO_VALUE_HIGH);	
	_time_delay(300);
	lwgpio_set_value(&gw_run_led, LWGPIO_VALUE_HIGH);
	lwgpio_set_value(&gw_plug_led, LWGPIO_VALUE_HIGH);
	lwgpio_set_value(&gw_ser_led, LWGPIO_VALUE_HIGH);
	lwgpio_set_value(&gw_rf_led, LWGPIO_VALUE_HIGH);
	
	if (!lwgpio_init(&gw_monitor_sw, BSP_SW_MONITOR, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
	{
		printf("Initializing button GPIO as input failed.\n");
        //_task_block();
	}
	lwgpio_set_functionality(&gw_monitor_sw, BSP_MONITOR_MUX_IRQ);
	lwgpio_set_attribute(&gw_monitor_sw, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);		
	if (!lwgpio_int_init(&gw_monitor_sw, LWGPIO_INT_MODE_LOW))
	{
        printf("Initializing button GPIO for interrupt failed.\n");
        //_task_block();
    }
    _int_install_isr(lwgpio_int_get_vector(&gw_monitor_sw), new_tamper_alarm_on_isr, (void *) &gw_monitor_sw);
    _bsp_int_init(lwgpio_int_get_vector(&gw_monitor_sw), 3, 0, TRUE);
    lwgpio_int_enable(&gw_monitor_sw, TRUE);
}

void gw_serial_init()
{
//	uint32_t     baud = 115200;
//	fd_ittye_rf_ptr = fopen("ittye:",(void *)(IO_SERIAL_RAW_IO)); 
//	if(NULL == fd_ittye_rf_ptr)
//	{
//		printf("\nittye_rf init error");
//	}
//	serial_ioctl(fd_ittye_rf_ptr,IO_IOCTL_SERIAL_SET_BAUD,&baud);
//	serial_ioctl(fd_ittye_rf_ptr,IO_IOCTL_SERIAL_CLEAR_BUF,NULL);
//	printf("\nittye_rf init ");
	
    fd_ittya_wifi_ptr = fopen("ittya:",(void *)(IO_SERIAL_RAW_IO|IO_SERIAL_NON_BLOCKING)); 
    if(NULL == fd_ittya_wifi_ptr) {
        printf("\nittya_wifi init error");
    }
    serial_ioctl(fd_ittya_wifi_ptr,IO_IOCTL_SERIAL_CLEAR_BUF,NULL);
	
	fd_ittye_485_ptr = fopen("ittye:",(void *)(IO_SERIAL_RAW_IO|IO_SERIAL_NON_BLOCKING)); 
	if(NULL == fd_ittye_485_ptr) {
		printf("\nittye_485 init error");
	}
    serial_ioctl(fd_ittye_485_ptr,IO_IOCTL_SERIAL_CLEAR_BUF,NULL);
}

void gw_rf_power_off()
{
    lwgpio_set_value(&gw_rf_power, LWGPIO_VALUE_LOW);
}

void gw_rf_reset_run()
{
    lwgpio_set_value(&gw_rf_reset, LWGPIO_VALUE_LOW);
	_time_delay(1000);
	lwgpio_set_value(&gw_rf_reset, LWGPIO_VALUE_HIGH);
}
//网口灯开
void eth_plug_led_on()
{
    lwgpio_set_value(&gw_plug_led, LWGPIO_VALUE_LOW);
}
//网口灯关
void eth_plug_led_off()
{
    lwgpio_set_value(&gw_plug_led, LWGPIO_VALUE_HIGH);
}
//网口灯闪烁
void eth_plug_led_blink()
{
	lwgpio_toggle_value(&gw_plug_led);
}
//上行灯开
void eth_updata_led_on()
{
    lwgpio_set_value(&gw_ser_led, LWGPIO_VALUE_LOW);
}
//上行灯关
void eth_updata_led_off()
{
    lwgpio_set_value(&gw_ser_led, LWGPIO_VALUE_HIGH);
}
void eth_ser_led_on()
{
    lwgpio_set_value(&gw_ser_led, LWGPIO_VALUE_LOW);
}
//上行灯关
void eth_ser_led_off()
{
    lwgpio_set_value(&gw_ser_led, LWGPIO_VALUE_HIGH);
}
//下行灯开
void rf_led_on()
{
    lwgpio_set_value(&gw_rf_led, LWGPIO_VALUE_LOW);
}
//下行灯关
void rf_led_off()
{
    lwgpio_set_value(&gw_rf_led, LWGPIO_VALUE_HIGH);
}
void rf_led_blink()
{
    lwgpio_toggle_value(&gw_rf_led);
}
//alarm
void monitor_led_on()
{
    lwgpio_set_value(&gw_monitor_led, LWGPIO_VALUE_LOW);
}
void monitor_led_off()
{
    lwgpio_set_value(&gw_monitor_led, LWGPIO_VALUE_HIGH);
}
void run_led_blink()
{
	//printf("\ngouggfsahfasdhf\n");
	lwgpio_toggle_value(&gw_run_led);
}

void reset_cpu() 
{
	VCORTEX_SCB_STRUCT_PTR scb = (VCORTEX_SCB_STRUCT_PTR)&(((CORTEX_SCS_STRUCT_PTR)CORTEX_PRI_PERIPH_IN_BASE)->SCB);
	uint32_t temp_AIRCR=0;

	temp_AIRCR = (uint32_t)scb->AIRCR;   		//Read AIRCR register
	temp_AIRCR &= 0x0000ffff;   				//DES mask of the top 16-bits
	temp_AIRCR |= 0x05Fa0000;   				//DES When writing to AIRCR the update 16-bit must be "0x05FA" per ARMv7-M Architecture Reference Manual (must register on ARM.com to get)
	temp_AIRCR |= SCB_AIRCR_SYSRESETREQ_MASK;   //DES set the SYSRESETREQ bit to generate software reset

   // Reboot...
	_time_delay (50);
	scb->AIRCR = temp_AIRCR;

	for(;;)
		
	return;
}

void reset_rf()
{
	lwgpio_set_value(&gw_rf_reset, LWGPIO_VALUE_LOW);
	_time_delay(1000);
	lwgpio_set_value(&gw_rf_reset, LWGPIO_VALUE_HIGH);
}

void reset_wifi()
{
	lwgpio_set_value(&wifi_reset, LWGPIO_VALUE_LOW);
	_time_delay(1000);
	lwgpio_set_value(&wifi_reset, LWGPIO_VALUE_HIGH);
}

void gw_sync_init()
{
    MUTEX_ATTR_STRUCT mutexattr;												// Pointer to the mutex attributes structure to initialize
    if (_mutatr_init(&mutexattr) != MQX_OK)
    {
          printf("\nmutex failed.");
    }

    if (_mutex_init(&mutex_serial_rf, &mutexattr) != MQX_OK)
    {
          printf("\nserial_rf mutex failed.");
    }
}

//初始化时间event
