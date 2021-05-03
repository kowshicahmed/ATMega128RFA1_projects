/*INCLUDES ************************************************************/
#include "ses_motorFrequency.h"
#include "util/atomic.h"
#include "ses_led.h"

/* DEFINES & MACROS **********************************************************/
#define TMR5_TCNT5_VAL_TOCOMPARE 0x0000      // start TCNT5 from 0
#define TIMER5_CYC_FOR_100MILLISEC 25000 - 1 // cycle required for 100ms delay
#define SPIKE_PER_REVOLUTION 6               // one revolution is completed after 6 spikes
#define N0_OF_SAMPLES 10                     // every 100ms one sample is stored in the array total 10 samples are taken
#define MEDIAN_INDEX 5                       // with 9 sample values in median array 5th value is in the middle
#define MS100_TO_SEC 10                      // converting factor for 100ms to 1s
#define SAMPLE_1 0
#define SAMPLE_2 1
#define SAMPLE_3 2

/* GLOBAL VARIABLES **********************************************************/
volatile static uint16_t spike_counter = 0;                       // gets increamented in the external interrupt isr
volatile static uint16_t revolution_completed = 0;                // no. of completed revolution in 100ms
volatile static uint8_t MOTOR_STATE = 0;                          // stores different states of the motor defined in enum
volatile static uint16_t spike_values_array[N0_OF_SAMPLES] = {0}; // saves spike_counter value each 100ms
volatile static uint16_t median_samples[N0_OF_SAMPLES - 1] = {0}; // revolution_completed values each 100ms
volatile static uint8_t iterator = 0;                             //iterates through the arrays

/* MOTOR STATES **********************************************************/
enum
{
    MOTOR_OFF,
    MOTOR_START,
    CALCULATE
};

/*FUNCTION DEFINITION *************************************************/
void motorFrequency_init()
{
    /*Initializing external interrupt*/
    cli();                                //clear global interrupt
    EIFR |= (1 << INTF0);                 //clear external interrupt flag register
    EICRA |= (1 << ISC01) | (1 << ISC00); //the rising edge of INTO gerenates interrupt request
    EIMSK |= (1 << INT0);                 //enable external interrupt musk register

    /*Initializing timer5*/
    TCCR5B &= ~(1 << WGM53); //timer 5 ctc mode
    TCCR5B |= (1 << WGM52);
    TCCR5A &= ~(1 << WGM51);
    TCCR5A &= ~(1 << WGM50);

    TCCR5B |= 1 << CS51; //set prescaler to 64
    TCCR5B &= ~(1 << CS52);
    TCCR5B |= 1 << CS50;

    TCNT5 = TMR5_TCNT5_VAL_TOCOMPARE;
    OCR5A = TIMER5_CYC_FOR_100MILLISEC;

    TIMSK5 |= 1 << OCIE5A; //enable compare match interrupt

    sei(); //enable global interrupt
    led_yellowInit();
    led_redInit();
}

uint16_t motorFrequency_getRecent()
{
    return MS100_TO_SEC * revolution_completed; // returns no. of revolution completed in 1sec i.e hertz
}

uint16_t motorFrequency_getMedian()
{
    for (int i = 0; i < N0_OF_SAMPLES - 1; i++) // sorts the samples of median array in ascending order
    {
        for (int j = 0; j < N0_OF_SAMPLES - 1; j++)
        {
            if (median_samples[j] > median_samples[i])
            {
                int temp = median_samples[i];
                median_samples[i] = median_samples[j];
                median_samples[j] = temp;
            }
        }
    }
    return MS100_TO_SEC * median_samples[MEDIAN_INDEX]; // returns no. of revolution completed in 1sec i.e median hertz
}

ISR(INT0_vect)
{
    led_yellowToggle();
    spike_counter++;             // increament with every current drom

    if (spike_counter == 0xFFFF) // prevent from overflow
    {
        spike_counter = 0;
    }
}

ISR(TIMER5_COMPA_vect)
{
    switch (MOTOR_STATE)
    {
    case MOTOR_OFF:

        spike_values_array[iterator] = spike_counter; // store spike_counter values every 100ms
        iterator++;

        if (iterator == N0_OF_SAMPLES) // restart the iterator if all samples are checked
        {
            iterator = 0;
        }

        // to determine if the motor has started rotating, a few samples are checked to see if the spike_counter is changing
        if (spike_values_array[SAMPLE_1] != spike_values_array[SAMPLE_2] && spike_values_array[SAMPLE_2] != spike_values_array[SAMPLE_3])
        {
            led_greenOff();
            MOTOR_STATE = MOTOR_START; // if motor is rotating change the state
            iterator = 0;              // restart iterator
            spike_counter = 0;
        }
        else
        {
            led_greenInit();
            led_greenOn();
            MOTOR_STATE = MOTOR_OFF; // if the motor is not rotating stay in the same state
            revolution_completed = 0x0000;
        }

        break;

    case MOTOR_START:

        spike_values_array[iterator] = spike_counter; // if the morot is rotating, collect spike counter value every 100ms
        iterator++;

        if (iterator == N0_OF_SAMPLES)
        {
            iterator = 0;
            MOTOR_STATE = CALCULATE; // move to next state when all the samples are collected
        }
        else
        {
            MOTOR_STATE = MOTOR_START; // stay in the same state until all the samples are collected
        }

        break;

    case CALCULATE:

        //the difference between two samples will give us the no. of rotation completed in 100ms
        revolution_completed = (spike_values_array[iterator + 1] - spike_values_array[iterator]) / SPIKE_PER_REVOLUTION;
        median_samples[iterator] = revolution_completed;
        iterator++;

        //since revolution completed is difference between two samples there are N0_OF_SAMPLES - 1 indexes for median array
        if (iterator == (N0_OF_SAMPLES - 1))
        {
            iterator = 0;
            MOTOR_STATE = MOTOR_OFF;
        }
        else
        {
            MOTOR_STATE = CALCULATE;
        }

        break;
    }
}