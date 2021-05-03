/*INCLUDES ************************************************************/
#include "ses_pwm.h"

/* DEFINES & MACROS **********************************************************/
#define MOTOR_PORT PORTG
#define MOTOR_PIN 5
#define TMR0_TCNT0_VAL_TOCOMPARE 0x00

/*FUNCTION DEFINITION *************************************************/
void pwm_init(void)
{
    DDR_REGISTER(MOTOR_PORT) |= (1 << MOTOR_PIN);
    PRR0 &= ~(1 << PRTIM0); // enable Timer0

    TCCR0A |= (1 << COM0B1); // setting the non inverting mode for OC0B
    TCCR0A &= ~(1 << COM0B0);

    TCCR0A |= (1 << WGM01) | (1 << WGM00); // setting Fast PWM mode TOP = 0xFF
    TCCR0B &= ~(1 << WGM02);

    TCCR0B &= ~(1 << CS02); // setting the prescaler value to no prescaler
    TCCR0B &= ~(1 << CS01);
    TCCR0B |= (1 << CS00);
    TCNT0 = TMR0_TCNT0_VAL_TOCOMPARE; // starting TCNTO from zero
    pwm_setDutyCycle(0);              // initially motor is stopped
}

void pwm_setDutyCycle(uint8_t dutyCycle)
{
    OCR0B = dutyCycle; // write the value passed from main
}
