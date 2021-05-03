#include <avr/io.h>
#include <util/delay.h>
#include "ses_button.h"
#include "ses_led.h"

int main(void)
{ 	//Initialization
	button_init();
	led_redInit();
	led_yellowInit();

	//Testing the button functions
	while (1)
	{
		if (button_isJoystickPressed())
			led_redToggle(); 	//pressing the joystick button will toggle the red led
		else if (button_isRotaryPressed())
			led_yellowToggle(); //pressing the rotary button will toggle the yellow led
	}
	return 0;
}
