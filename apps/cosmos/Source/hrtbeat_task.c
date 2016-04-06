#include "kwtj_GWII.h"
#include "uplink_data_type.h"
#include "rf_type_cmd.h"
#include "internal_mem.h"
#include <timer.h>

extern bool WifiUpgradeNow;
extern uint32_t systime_get(void);
//extern _timer_id active_rebootwifi_timer_id;
//extern _timer_id active_rebootwifi_timer(void);

void heart_beat_task(uint32_t parameter)
{
	uint32_t 		delay_unit = 0;
	TIME_STRUCT		mqx_time;
	
	HEARTBEAT_TYPE_t	heartbeat_pkg;
	
	heartbeat_pkg.Uid    = htonl(CosmosInfoPtr->CosmosUid);
	heartbeat_pkg.UtcSec = htonl(0x02020000);

	_time_delay(500);
	uplk_send((char *)&heartbeat_pkg, sizeof(HEARTBEAT_TYPE_t));	
	while(1)
	{
		/* 所有的led指示需要放到独立的任务中去 */
		if (CosmosInfoPtr->FuncEn&FUNC_EN_ETH)
		{
			for (int i=0; i < CosmosTcpPtr->CellCount; i++) {
				if (socket_info[i].Status == SOCKET_OK) {
					lwgpio_set_value(&gw_ser_led, LWGPIO_VALUE_LOW);
				}else {
					lwgpio_set_value(&gw_ser_led, LWGPIO_VALUE_HIGH);
				}
				_time_delay(900);
				_time_get(&mqx_time);
				run_led_blink();
			}			
		}else {
			_time_delay(900);
			_time_get(&mqx_time);
			run_led_blink();
		}
 
		if (mqx_time.SECONDS%10 == 0){
			if (WifiUpgradeNow != TRUE)
				uplk_send((char *)&heartbeat_pkg, sizeof(HEARTBEAT_TYPE_t));
				//active_rebootwifi_timer_id = active_rebootwifi_timer();
		}
		if (mqx_time.SECONDS%3600 == 0){
			systime_get();
			delay_unit = 0;
		}
	}
}