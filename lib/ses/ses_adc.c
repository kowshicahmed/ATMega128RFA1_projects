#include <avr/io.h>
#include <util/delay.h>
#include "ses_common.h"
#include "ses_led.h"
#include "ses_adc.h"
#include "ses_button.h"

#define ADC_SENSORS_PORT PORTF
#define TEMPERATURE_SENSOR_PIN 2
#define LIGHT_SENSOR_PIN 4
#define JOYSTICK_PIN 7
#define ADC_VREF_SRC 0xC0   //the 7th(REFS1) and 6th(REFS0) bit of ADMUX has to be set to 1 which is equivalent to 0xC0
#define ADC_PRESCALE 0x03   //set prescalar 8
#define NO_OF_CHANNELS 0x07 //there are total 7 ADC channels
#define CLEAR_CHANNEL 0xF8  //clears the 3 LSBs of ADMUX

void adc_init(void)
{
    //configuaring the data direction registers of the sensors and de-activating pull up registers
    DDR_REGISTER(ADC_SENSORS_PORT) &= ~(1 << TEMPERATURE_SENSOR_PIN);
    ADC_SENSORS_PORT &= ~(1 << TEMPERATURE_SENSOR_PIN);
    DDR_REGISTER(ADC_SENSORS_PORT) &= ~(1 << LIGHT_SENSOR_PIN);
    ADC_SENSORS_PORT &= ~(1 << LIGHT_SENSOR_PIN);
    DDR_REGISTER(ADC_SENSORS_PORT) &= ~(1 << JOYSTICK_PIN);
    ADC_SENSORS_PORT &= ~(1 << JOYSTICK_PIN);

    //configuaring ADC
    PRR0 &= ~(1 << PRADC);   //disable power reduction mode for the ADC module
    ADMUX |= ADC_VREF_SRC;   //select 1.6V voltage reference
    ADMUX &= ~(1 << ADLAR);  //aet the ADC result right adjusted
    ADCSRA |= ADC_PRESCALE;  //set prescalar 8
    ADCSRA &= ~(1 << ADATE); //auto triggering off
    ADCSRA |= (1 << ADEN);   //enable ADC
}

uint16_t adc_read(uint8_t adc_channel)
{
    adc_channel = adc_channel & NO_OF_CHANNELS;    //selecting the adc channel number between 0 to 7
    ADMUX = (ADMUX & CLEAR_CHANNEL) | adc_channel; //clearing the channel and storing the channel number to the ADMUX register
    ADCSRA |= (1 << ADSC);                         //enable conversion mode of ADC
    while (ADCSRA & (1 << ADSC))
        ; //keep checking the status of ADC conversion mode either 1 or 0
    return ADC;
}

uint16_t adc_getTemperature(uint16_t value)
{
    uint16_t read_value = value * 0.4882; // Read ADC2 Channel, convert the value to degree celsius
    return read_value;
}

uint16_t adc_light(uint16_t val)
{
    uint16_t read_light_value = val; //Read ADC4 channel and get the raw value of light sensor
    return read_light_value;
}
