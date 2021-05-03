#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

void shortDelay (uint32_t millis)
{
    uint32_t i; // 16 bit unsigned integer
    int clock_cycle = 16000; //16million clock cycle per second
    for (i = (millis * clock_cycle); i > 0 ; i--)
    { //prevent code optimization by using inline assembler
        asm volatile ( "nop" ); // one cycle with no operation
    }
}
int main(void)
{
    int led_pin_no = 1; //declaring led pin position
    DDRG |= (1<<led_pin_no); //initialization of led pin register as output
    while (1) //infinite loop for toggling the led
   {
       PORTG ^= (1<<led_pin_no); //toggling the red led
       shortDelay(1000); //delay of 1 second
   }
return 0;
}
