#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

//Definitions

#define LED_RED_PIN 1
#define CYCLE_PER_MILLISECOND 1600
#define WASTED_CYCLE 8

//Function Prototype
void wait(uint32_t millis);

int main(void)
{
    DDRG |= (1 << LED_RED_PIN); //initialising Red LED pin as output

    while (1)
    {
        PORTG ^= (1 << LED_RED_PIN); //toggles the Red LED
        wait(1000);                  //delay for 1 second
    }
    return 0;
}

void wait(uint32_t millis)
{
    uint32_t i;
    for (i = ((millis * CYCLE_PER_MILLISECOND) / WASTED_CYCLE); i > 0; --i)
    { //elf file showed 8 extra cycles for the for loop
        asm volatile( "nop" );
    }
}