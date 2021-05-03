#include <avr/io.h>
#include <util/delay.h>
#include "ses_common.h"
#include "ses_led.h"

/* DEFINES & MACROS **********************************************************/

// LED wiring on SES board
#define LED_RED_PORT PORTG
#define LED_RED_PIN 1

#define LED_YELLOW_PORT PORTF
#define LED_YELLOW_PIN 7

#define LED_GREEN_PORT PORTF
#define LED_GREEN_PIN 6

/* FUNCTION DEFINITION *******************************************************/

void led_redInit(void)
{
	DDR_REGISTER(LED_RED_PORT) |= (1 << LED_RED_PIN); //initialize the pin 1 of PORTG as output for RED led
	led_redOff();
}

void led_redToggle(void)
{
	LED_RED_PORT ^= (1 << LED_RED_PIN); //bitwise XOR operation for toggling the RED led
}

void led_redOn(void)
{
	LED_RED_PORT &= ~(1 << LED_RED_PIN); //bitwise AND operation for clearing the RED led as logical HIGH will turn them OFF
}

void led_redOff(void)
{
	LED_RED_PORT |= (1 << LED_RED_PIN); //bitwise OR operation for setting the RED led as logical HIGH will turn them OFF
}

void led_yellowInit(void)
{
	DDR_REGISTER(LED_YELLOW_PORT) |= (1 << LED_YELLOW_PIN); //initialize the pin 7 of PORTF as output for YELLOW led
	led_yellowOff();
}

void led_yellowToggle(void)
{
	LED_YELLOW_PORT ^= (1 << LED_YELLOW_PIN); //bitwise XOR operation for toggling the YELLOW led
}

void led_yellowOn(void)
{
	LED_YELLOW_PORT &= ~(1 << LED_YELLOW_PIN); //bitwise AND operation for clearing the YELLOW led as logical HIGH will turn them OFF
}

void led_yellowOff(void)
{
	LED_YELLOW_PORT |= (1 << LED_YELLOW_PIN); //bitwise OR operation for setting the YELLOW led as logical HIGH will turn them OFF
}

void led_greenInit(void)
{
	DDR_REGISTER(LED_GREEN_PORT) |= (1 << LED_GREEN_PIN); //initialize the pin 6 of PORTF as output for GREEN led
	led_greenOff();
}

void led_greenToggle(void)
{
	LED_GREEN_PORT ^= (1 << LED_GREEN_PIN); //bitwise XOR operation for toggling the GREEN led
}

void led_greenOn(void)
{
	LED_GREEN_PORT &= ~(1 << LED_GREEN_PIN); //bitwise AND operation for clearing the GREEN led as logical HIGH will turn them OFF
}

void led_greenOff(void)
{
	LED_GREEN_PORT |= (1 << LED_GREEN_PIN); //bitwise OR operation for setting the GREEN led as logical HIGH will turn them OFF
}
