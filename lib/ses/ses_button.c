#include <avr/io.h>
#include <util/delay.h>
#include "ses_common.h"
#include "ses_button.h"
#include "ses_timer.h"

/*Button wiring definitions******************/
#define ROTARY_ENCODER_PORT PORTB
#define ROTARY_ENCODER_PIN 6
#define JOYSTICK_PORT PORTB
#define JOYSTICK_PIN 7

#define BUTTON_NUM_DEBOUNCE_CHECKS 5
#define BUTTON_DEBOUNCE_PERIOD 30
#define BUTTON_DEBOUNCE_POS_JOYSTICK 0x01
#define BUTTON_DEBOUNCE_POS_ROTARY 0x02

/*Function pointers should not be accessible outside module (static) and must not be touched by GCC
optimization (volatile)*/
volatile static pButtonCallback rotary_button_callback;
volatile static pButtonCallback joystick_button_callback;

void button_init(bool debouncing)
{
    DDR_REGISTER(ROTARY_ENCODER_PORT) &= ~(1 << ROTARY_ENCODER_PIN); // setting the pin as output
    ROTARY_ENCODER_PORT |= (1 << ROTARY_ENCODER_PIN);                // activating pull up resister
    DDR_REGISTER(JOYSTICK_PORT) &= ~(1 << JOYSTICK_PIN);             // setting the pin as output
    JOYSTICK_PORT |= (1 << JOYSTICK_PIN);                            // activating pull up resister

    if (debouncing)                              //start the button check state function in timer1
    {
        timer1_start ();
        timer1_setCallback (button_checkState);
    }

    else
    {
        sei();                               // global interrupt enable
        PCICR |= (1 << PCIE0);               // pin change interrupt for PCINT7:0 enable
        PCMSK0 |= (1 << JOYSTICK_PIN);       // pin change mask register for PORTB7 enable
        PCMSK0 |= (1 << ROTARY_ENCODER_PIN); // pin change mask register for PORTB6 enable
    }    
}

bool button_isJoystickPressed(void)
{
    bool is_button_pressed = false;
    if (PIN_REGISTER(JOYSTICK_PORT) & (1 << JOYSTICK_PIN)) // the button pin will be grounded when it is pressed
        is_button_pressed = false;
    else
        is_button_pressed = true;
    return is_button_pressed;
}

bool button_isRotaryPressed(void)
{
    bool is_button_pressed = false;
    if (PIN_REGISTER(ROTARY_ENCODER_PORT) & (1 << ROTARY_ENCODER_PIN)) // the button pin will be grounded when it is pressed
        is_button_pressed = false;
    else
        is_button_pressed = true;
    return is_button_pressed;
}

void button_setRotaryButtonCallback(pButtonCallback callback)
{
    rotary_button_callback = callback; // save the function passed in 'callback' to the function pointer
}
void button_setJoystickButtonCallback(pButtonCallback callback)
{
    joystick_button_callback = callback; // save the function passed in 'callback' to the function pointer
}
ISR(PCINT0_vect)
{
    /***if the callback points to a valid function and the button is pressed,
    function passed inside the function pointers will be executed*****/
    if (rotary_button_callback != NULL && button_isRotaryPressed() == true)
    {
        rotary_button_callback();
    }
    else if (joystick_button_callback != NULL && button_isJoystickPressed() == true)
    {
        joystick_button_callback();
    }
}

void button_checkState()
{
    static uint8_t state[BUTTON_NUM_DEBOUNCE_CHECKS] = {};
    static uint8_t index = 0;
    static uint8_t debouncedState = 0;
    uint8_t lastDebouncedState = debouncedState;

    // each bit in every state byte represents one button
    state[index] = 0;

    if (button_isJoystickPressed())
    {
        state[index] |= BUTTON_DEBOUNCE_POS_JOYSTICK;
    }

    if (button_isRotaryPressed())
    {
        state[index] |= BUTTON_DEBOUNCE_POS_ROTARY;
    }

    index++;

    if (index == BUTTON_NUM_DEBOUNCE_CHECKS)
    {
        index = 0;
    }
    // init compare value and compare with ALL reads, only if
    // we read BUTTON_NUM_DEBOUNCE_CHECKS consistent "1's" in the state
    // array, the button at this position is considered pressed
    
    uint8_t j = 0xFF;
    for (uint8_t i = 0; i < BUTTON_NUM_DEBOUNCE_CHECKS; i++)
    {
        j = j & state[i];
    }
    debouncedState = j;

    if (debouncedState == BUTTON_DEBOUNCE_POS_JOYSTICK && joystick_button_callback != NULL && debouncedState != lastDebouncedState)
    {
        joystick_button_callback();
    }

    if (debouncedState == BUTTON_DEBOUNCE_POS_ROTARY && rotary_button_callback != NULL && debouncedState != lastDebouncedState)
    {
        rotary_button_callback();
    }
}