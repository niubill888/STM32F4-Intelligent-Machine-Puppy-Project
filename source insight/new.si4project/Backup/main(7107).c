#include "drv_led.h"
#include "drv_beep.h"

int main(void)
{
	drv_led_init();
	drv_beep_init();
	while(1)
	{   
		drv_led_open();
		drv_beep_open();
		delay_ms(100);
		drv_led_close();
		drv_beep_close();
		delay_ms(100);
	}

}

