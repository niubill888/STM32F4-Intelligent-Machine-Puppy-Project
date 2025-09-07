#include "drv_led.h"
#include "drv_beep.h"
#include "drv_systick.h"

int main(void)
{
	drv_led_init();
	drv_beep_init();
	while(1)
	{   
		drv_led_open();
		//drv_beep_open();
		drv_systick_ms(500);
		drv_led_close();
		//drv_beep_close();
		drv_systick_ms(500);
	}

}

