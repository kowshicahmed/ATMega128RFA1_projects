#include <avr/io.h>
#include <util/delay.h>
#include "ses_timer.h"
#include "ses_led.h"
#include "ses_uart.h"

//Function definitions
void software_timer();
void software_timer_2();

static uint16_t software_counter = 0; //software counter to decrease the led toggle freqeuncy
static uint16_t software_counter_2 = 0; //software counter to decrease the led toggle freqeuncy

int main()
{
    led_yellowInit();
    led_greenInit();
    //uart_init(57600);
    timer2_start();
    timer1_start();
    timer2_setCallback(software_timer);
    timer1_setCallback(software_timer_2);

    while (1)
    {
		fprintf(uartout, "\nValue: %d", TCNT1);
    }
    return 0;
}

void software_timer()
{
    software_counter++;
    if (software_counter % 1000 == 0) // toggle the led every 1s
        led_yellowToggle();
    if (software_counter == 0xFFFF) // reset the software counter to avoid overflow
        software_counter = 0;
}

void software_timer_2()
{
    software_counter_2++;
    if (software_counter_2 % 200 == 0) // toggle the led every 1s
        led_greenToggle();
    if (software_counter_2 == 0xFFFF) // reset the software counter to avoid overflow
        software_counter_2 = 0;
}
