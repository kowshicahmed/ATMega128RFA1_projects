#include <avr/io.h>
#include <util/delay.h>
#include "ses_button.h"
#include "ses_led.h"
#include "ses_lcd.h"
#include "ses_adc.h"

int main()
{
    adc_init();
    lcd_init();
    uint16_t temp_value = adc_read(ADC_TEMP_CH);
	uint16_t read_temp_value = adc_getTemperature(temp_value);
    uint16_t light_value = adc_read(ADC_LIGHT_CH);
    while(1)
    {
        if(read_temp_value)
        {
            char val[4];        // Store in char Array
		    sprintf(val, "%hu", read_temp_value); //convert the number to string
		    lcd_setCursor(0,0);
		    fprintf(lcdout, "Temperature %s degree celsius",val);
        }
        else if(adc_light(light_value))
        {
            lcd_setCursor(1,0);
            fprintf(lcdout, "Light Value %hu", adc_light(light_value));
            led_redOn();
            _delay_ms(1000);
            led_redOff();
        }


    }
return 0;

}