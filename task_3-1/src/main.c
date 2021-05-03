#include <avr/io.h>
#include <util/delay.h>
#include "ses_button.h"
#include "ses_led.h"

//Declaring functions for passing as callbacks
void led_red_toggle();
void led_green_toggle();

int main()
{
    button_init(0);
    led_redInit();
    led_greenInit();

    button_setJoystickButtonCallback(led_red_toggle);
    button_setRotaryButtonCallback(led_green_toggle);

    while (1)
    {
    }
    return 0;
}

void led_red_toggle()
{
    led_redToggle();
}

void led_green_toggle()
{
    led_greenToggle();
}
