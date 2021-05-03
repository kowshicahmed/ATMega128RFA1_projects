#include <avr/io.h>
#include <util/delay.h>
void toggled()
{
   PORTF ^= 0x80;
}
/**Toggles the yellow LED of the SES-board*/
int main(void) {
	DDRF |= 0x80;
	while (1) 
	{
		toggled();
		_delay_ms(1000);
	}
	return 0;
}
