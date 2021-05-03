#include <avr/io.h>
#include <util/delay.h>
#include "ses_button.h"
#include "ses_led.h"
#include "ses_lcd.h"

//this function will show the time in seconds since last reset
int show_time(int *ptime);

int main(void)
{ //Initialization
	button_init();
	led_redInit();
	led_greenInit();
	lcd_init();
	int time = 0; //Initializing a new variable to display seconds since last reset

	while (1)
	{   
		lcd_setCursor(0, 0);
		fprintf(lcdout, "Time since last reset %d s", show_time(&time)); //passing the address of "time" variable
		if (button_isJoystickPressed())
			led_redOn(); //while pressing the joystick button the red led will be on
		else if (button_isRotaryPressed())
			led_greenOn(); //while pressing the rotary button the yellow led will be on
		else			   //releasing the buttons will turn off the led
		{
			led_redOff();
			led_greenOff();
		}
	}
	return 0;
}

int show_time(int *ptime)
{
	_delay_ms(1000);
	*ptime = *ptime + 1; //after 1 sec increment the value of time by 1
	return *ptime;
}
