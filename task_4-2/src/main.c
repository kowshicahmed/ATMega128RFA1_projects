#include "ses_button.h"
#include "ses_led.h"
#include "ses_scheduler.h"
#include "ses_lcd.h"


/*Enumuration of LED colors for LED toggle function*/
enum color
{
	RED,
	GREEN,
	YELLOW
};

/*Function Definitions*/
void task_led_toggle(int color);
void task_yellow_led();
void task_stopwatch();
void joystick_event();
void rotary_event();

/*Volatile static global variables*/
volatile static int yellow_button_time = 0;
volatile static int second = 0;
volatile static int tenth_second = 0;
volatile static int rotary_button_flag = 0;

int main()
{
	/*Initializations*/
	led_yellowInit();
	led_greenInit();
	led_redInit();
	lcd_init();
	button_init(0);

	/*Creating new task variables*/
	taskDescriptor *task_1 = NULL;
	taskDescriptor *task_2 = NULL;
	taskDescriptor *task_3 = NULL;
	taskDescriptor *task_4 = NULL;

	/*upon pressing button, function is called passing the joystick_event and rotary_event as parameter*/
	button_setJoystickButtonCallback(joystick_event);
	button_setRotaryButtonCallback(rotary_event);

	/*adding tasks to variables and passing the parameters*/
	task_1 = (taskDescriptor *)create_new_task((task_t)task_led_toggle, 2000, 2000, (void *)GREEN);
	scheduler_add(task_1);

	task_2 = (taskDescriptor *)create_new_task((task_t)button_checkState, 5, 5, NULL);
	scheduler_add(task_2);

	task_3 = (taskDescriptor *)create_new_task((task_t)task_yellow_led, 5000, 5000, NULL);
	scheduler_add(task_3);

	task_4 = (taskDescriptor *)create_new_task((task_t)task_stopwatch, 100, 100, NULL);
	scheduler_add(task_4);

	scheduler_init();
	scheduler_run();

	return 0;
}

/*toggles an LED using enum to select the color*/
void task_led_toggle(int color)
{
	switch (color)
	{
	case YELLOW:
		led_yellowToggle();
		break;
	case GREEN:
		led_greenToggle();
		break;
	case RED:
		led_redToggle();
		break;
	default:
		led_redToggle();
	}
}

/*yellow LED is on when the joystick button is pressed. 
it is turned off when pressing the joystickbutton again 
or after 5 seconds, whatever comes first*/
void task_yellow_led()
{
	yellow_button_time++; //incremented to 1 every five seconds

	if (yellow_button_time == 1)
	{
		led_yellowOff();
		yellow_button_time = 0;
	}
}

void joystick_event()
{
	led_yellowToggle();
	yellow_button_time = 0;
}

/*stop watch starts and stops when pressing the rotary button
LCD is used to show the current stop watch time in seconds and tenths of seconds*/
void task_stopwatch()
{
	if (rotary_button_flag == 1) //depending on the rotary_button_flag,  tenth_second and second are incremented or stopped
	{
		tenth_second++;
		if(tenth_second > 9)
		{
			second++;
			tenth_second = 0;
		}
	}
	else if (rotary_button_flag == 0)
	{
		tenth_second = tenth_second;
		second = second;
	}
	lcd_setCursor(0, 0);
	fprintf(lcdout, "Current Time %d:%d", second, tenth_second);
}

void rotary_event()
{
	rotary_button_flag = !rotary_button_flag; //with every rotary button press, flag is inverted
}