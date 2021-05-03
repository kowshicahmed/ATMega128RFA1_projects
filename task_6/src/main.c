#include "ses_button.h"
#include "ses_scheduler.h"
#include "ses_lcd.h"
#include "ses_fsm.h"
#include "ses_led.h"

/* DEFINES & MACROS **********************************************************/
#define TIMEOUT_VALUE 5 //< if alarm is running, after timeout, turn alarm off

/* FUNCTION DEFINITION *******************************************************/
static void joystickPressedDispatch(void *param); //< joystick pressed signal
static void rotaryPressedDispatch(void *param);   //< rotary pressed signal

static void task_display_time(Fsm *fsm);  //< task to display the system time
static void task_check_alarm(Fsm *fsm);   //< task to check if alarm condition are met
static void task_alarm_timeout(Fsm *fsm); //< configures the timeout behaviour of the alarm

/* FSM super state ***********************************************************/
typedef struct
{
    Fsm super;
} alarm_clock_FSM;

/* Volatile static global variables *******************************************/
static alarm_clock_FSM alarm_clock;       //< an instance of the super state
volatile static uint8_t timeout_time = 0; //< keeps the timeout value when alarm is running

int main()
{
    /* Initializations */
    button_init(1);
    led_redInit();
    led_greenInit();
    lcd_init();
    fsm_init((Fsm *)&alarm_clock, idle);

    /* Button callback functions for generating the signals */
    button_setJoystickButtonCallback(joystickPressedDispatch);
    button_setRotaryButtonCallback(rotaryPressedDispatch);

    /* Tasks to display main clock, check alarm time and calculate timeout*/
    taskDescriptor *task_1 = NULL;
    task_1 = (taskDescriptor *)create_new_task((task_t)task_display_time, 1000, 1000, (Fsm *)&alarm_clock);
    scheduler_add(task_1);

    taskDescriptor *task_2 = NULL;
    task_2 = (taskDescriptor *)create_new_task((task_t)task_check_alarm, 250, 250, (Fsm *)&alarm_clock);
    scheduler_add(task_2);

    taskDescriptor *task_3 = NULL;
    task_3 = (taskDescriptor *)create_new_task((task_t)task_alarm_timeout, 1000, 1000, (Fsm *)&alarm_clock);
    scheduler_add(task_3);

    scheduler_init();
    scheduler_run();
}

static void joystickPressedDispatch(void *param)
{
    lcd_clear();
    Event e = {.signal = JOYSTICK_PRESSED};
    fsm_dispatch((Fsm *)&alarm_clock, &e);
}

static void rotaryPressedDispatch(void *param)
{
    lcd_clear();
    Event e = {.signal = ROTARY_PRESSED};
    fsm_dispatch((Fsm *)&alarm_clock, &e);
}

static void task_display_time(Fsm *fsm)
{
    if (fsm->state == hour_set || fsm->state == minute_set || fsm->state == idle) //< display HH::MM
    {
        lcd_setCursor(0, 0);
        fprintf(lcdout, "%02d::%02d", fsm->timeSet.hour, fsm->timeSet.minute);
    }

    else //< display the full clock in HH::MM::SS format
    {
        lcd_setCursor(0, 0);
        fprintf(lcdout, "%02d::%02d::%02d",
                setTime_wrapper_hr((Fsm *)&alarm_clock, fsm->timeSet.hour),
                setTime_wrapper_min((Fsm *)&alarm_clock, fsm->timeSet.minute), fsm->timeSet.second);
        led_greenToggle(); //< toggle green led with the second hand of the clock
    }
}

static void task_check_alarm(Fsm *fsm)
{
    if (timeout_time < TIMEOUT_VALUE && get_alarm_time_hr() == fsm->timeSet.hour &&
        get_alarm_time_min() == fsm->timeSet.minute && fsm->isAlarmEnabled == true &&
        fsm->state == normal_mode)
    {
        lcd_clear();
        Event e = {.signal = TIME_MATCH}; //< generate TIME_MATCH signal when alarm conditions are met
        fsm_dispatch((Fsm *)&alarm_clock, &e);
    }
    if (timeout_time < TIMEOUT_VALUE && fsm->state == sound_alarm && fsm->isAlarmEnabled == true)
    {
        led_redToggle(); //< FSM is in the sound alarm state and red led is toggling
        int temp = TIMEOUT_VALUE;
        lcd_setCursor(0, 3);
        fprintf(lcdout, "Wait %ds", temp - timeout_time);
    }
    else if (timeout_time > TIMEOUT_VALUE && fsm->state == sound_alarm) //< if 5s time has passed since alarm was on
    {
        lcd_clear();
        led_redOff();
        Event e = {.signal = TIMEOUT}; //< generate signal timeout
        fsm_dispatch((Fsm *)&alarm_clock, &e);
    }
}

static void task_alarm_timeout(Fsm *fsm)
{
    if (fsm->state == sound_alarm)
    {
        timeout_time++; //< start the timer when the FSM is in sound alarm state
    }

    else
    {
        timeout_time = 0;
    }
}