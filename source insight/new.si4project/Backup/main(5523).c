#include "drv_led.h"
#include "drv_beep.h"
#include "drv_systick.h"
#include "drv_key.h"

uint8_t keyvalue=0;

int main(void)
{
	drv_led_init();
	drv_led_close();
	drv_beep_init();
	drv_systick_init(168);
	drv_key_init();
	while(!drv_get_key());
	while(1)
	{   
		drv_get_key();
		if(keyvalue)//这里填keyvalue==1不行！！！
		{
			drv_led_close();
			drv_beep_open();
		}
		else
		{
			drv_beep_close();
			drv_led_flow();
		}
	}

}

