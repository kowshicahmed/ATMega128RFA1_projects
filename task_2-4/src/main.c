#include <util/delay.h>
#include "ses_led.h"

int main()
{
    //Initialization
	led_redInit(); 
    led_yellowInit(); 
    led_greenInit(); 
    
    while (1) 
	{	//testing the functions

		//This code will toggle all the LEDs after 1 sec
		/*
		led_redToggle();
		led_greenToggle();
		led_yellowToggle();
		_delay_ms(1000);
		*/

		//This will also toggle the LEDs but using the on/off functions
		led_redOn();
		led_greenOn();
		led_yellowOn();
		_delay_ms(1000);
		led_redOff();
		led_greenOff();
		led_yellowOff();
		_delay_ms(1000);
	}
	
	return 0;
}

