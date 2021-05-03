#include <avr/io.h>
#include <util/delay.h>
#include "ses_lcd.h"
#include "ses_uart.h"

int main(void)
{
	//Initialization of uart with 57600 baud rate and lcd
	uart_init(57600);
	lcd_init();

	fprintf(uartout, "START\n");
	fprintf(lcdout, "START");
	while (1)
	{
	}

	return 0;
}
