#include "ses_button.h"
#include "ses_pwm.h"
#include "ses_motorFrequency.h"
#include "ses_scheduler.h"
#include "ses_lcd.h"

/* DEFINES & MACROS **********************************************************/
#define MINUTE_TO_SECOND 60
#define DUTY_CYCLE 170

/*Function Definitions*/
void joystick_event();	 // turns on and off the motor with every press
void task_display_rpm(); // displays recent RPM and median RPM

/*Volatile static global variables*/
volatile static int joystick_button_flag = 0; // detects number of button presses

int main()
{
	/*Initializations*/
	pwm_init();
	button_init(0);
	scheduler_init();
	motorFrequency_init();
	lcd_init();

	/*upon pressing button, function is called passing the joystick_event as parameter*/
	button_setJoystickButtonCallback(joystick_event);

	/*adding a new task in the scheduler to display RPM values*/
	taskDescriptor *task_1 = NULL;
	task_1 = (taskDescriptor *)create_new_task((task_t)task_display_rpm, 2000, 2000, NULL);
	scheduler_add(task_1);
	scheduler_run();
}

void joystick_event()
{
	if (joystick_button_flag == 0)
	{
		pwm_setDutyCycle(DUTY_CYCLE); // set the motor duty cycle to 170
		joystick_button_flag = 1;
	}

	else if (joystick_button_flag == 1)
	{
		pwm_setDutyCycle(0); // motor is stopped
		joystick_button_flag = 0;
	}
}

void task_display_rpm() // converts Hertz to RPM and displays
{
	lcd_setCursor(0, 0);
	fprintf(lcdout, "Recent RPM = %d", MINUTE_TO_SECOND * motorFrequency_getRecent());
	lcd_setCursor(0, 1);
	fprintf(lcdout, "Median RPM = %d", MINUTE_TO_SECOND * motorFrequency_getMedian());
}