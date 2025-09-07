#include "drv_led.h"
#include "drv_beep.h"
#include "drv_systick.h"
#include "drv_key.h"
#include "drv_exti.h"

uint8_t keyvalue=0;

int main(void)
{

	drv_led_init();
	drv_led_close();
	drv_beep_init();
	drv_systick_init(168);
	drv_key_init();
	drv_exit9_init();
	while(1)
	{   
		if(keyvalue)
		{
			drv_beep_open();
			
		}
		else
		{
			drv_beep_close();
		}
	}

}

