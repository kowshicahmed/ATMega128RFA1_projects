/* INCLUDES ************************************************************/
#include "ses_fsm.h"
#include "ses_led.h"
#include "ses_lcd.h"

/* DEFINES & MACROS ****************************************************/
#define HR_PER_DAY 24
#define MIN_PER_HR 60
#define SEC_PER_MIN 60
#define mills_per_sec 1000
#define TRANSITION(newState) (fsm->state = newState, RET_TRANSITION)

/* Global variables ****************************************************/
static Fsm alarm_time;

/* FUNCTION DEFINITION *************************************************/
uint8_t setTime_wrapper_hr(Fsm *user_time, uint8_t hour)
{
    user_time->timeSet.hour = hour % HR_PER_DAY; //< wraps hr value in 0 to 23

    if (user_time->timeSet.minute == (MIN_PER_HR))
    {
        user_time->timeSet.hour += 1;  //< increment hr if min = 60
        user_time->timeSet.minute = 0; //< reset min
    }

    if (user_time->timeSet.hour > (HR_PER_DAY - 1)) //< reset hr
    {
        user_time->timeSet.hour = 0;
    }

    return user_time->timeSet.hour;
}

uint8_t setTime_wrapper_min(Fsm *user_time, uint8_t minute)
{
    user_time->timeSet.minute = minute % (SEC_PER_MIN + 1);                                //< wraps min value in 0 to 60
    user_time->timeSet.second = (scheduler_getTime() / mills_per_sec) % (SEC_PER_MIN + 1); //< wraps sec value in 0 to 60
    if (user_time->timeSet.second == (SEC_PER_MIN))
    {
        user_time->timeSet.minute += 1; //< increment min if sec = 60
        user_time->timeSet.second = 0;  //< reset sec
    }

    return user_time->timeSet.minute;
}

/*********** State functions to set up the alarm time and system time ***********************/
fsmReturnStatus idle(Fsm *fsm, const Event *event)
{
    switch (event->signal)
    {
    case ENTRY:
        lcd_setCursor(0, 1);
        fprintf(lcdout, "Rotary=Set_hr");
        return RET_HANDLED;
        break;
    case ROTARY_PRESSED:
        return TRANSITION(hour_set);
        break;
    default:
        return RET_IGNORED;
        break;
    }
}

fsmReturnStatus hour_set(Fsm *fsm, const Event *event)
{
    switch (event->signal)
    {
    case ENTRY:
        lcd_setCursor(0, 1);
        fprintf(lcdout, "Rotary=hr++");
        lcd_setCursor(0, 2);
        fprintf(lcdout, "Joystick=set_min");
        return RET_HANDLED;
        break;
    case ROTARY_PRESSED:
        fsm->timeSet.hour++;
        lcd_setCursor(0, 1);
        fprintf(lcdout, "Rotary=hr++");
        lcd_setCursor(0, 2);
        fprintf(lcdout, "Joystick=set_min");
        return RET_HANDLED;
        break;
    case JOYSTICK_PRESSED:
        return TRANSITION(minute_set);
        break;
    default:
        return RET_IGNORED;
        break;
    }
}

fsmReturnStatus minute_set(Fsm *fsm, const Event *event)
{
    switch (event->signal)
    {
    case ENTRY:
        lcd_setCursor(0, 1);
        fprintf(lcdout, "Rotary=min++");
        lcd_setCursor(0, 2);
        fprintf(lcdout, "Joystk=normalmode");
        return RET_HANDLED;
        break;
    case ROTARY_PRESSED:
        fsm->timeSet.minute++;
        lcd_setCursor(0, 1);
        fprintf(lcdout, "Rotary=min++");
        lcd_setCursor(0, 2);
        fprintf(lcdout, "Joystk=normalmode");
        return RET_HANDLED;
        break;
    case JOYSTICK_PRESSED:
        return TRANSITION(normal_mode);
        break;
    case EXIT:
        scheduler_setTime(0); //< start the system time from 0 upon exit
        return RET_HANDLED;
        break;
    default:
        return RET_IGNORED;
        break;
    }
}

fsmReturnStatus normal_mode(Fsm *fsm, const Event *event)
{
    switch (event->signal)
    {
    case ENTRY:
        lcd_setCursor(0, 1);
        fprintf(lcdout, "Normalmode");
        lcd_setCursor(0, 2);
        fprintf(lcdout, "Joyst=show_alrm");
        lcd_setCursor(0, 3);
        fprintf(lcdout, "Rotary=togg_alrm");
        led_redOff();
        if (fsm->isAlarmEnabled == true)
        {
            led_yellowInit();
            led_yellowOn();
        }
        else if (fsm->isAlarmEnabled == false)
        {
            led_yellowOff();
        }
        return RET_HANDLED;
        break;
    case ROTARY_PRESSED: //< pressing the rotary button in normal mode we can toggle on or off the alarm
        fsm->isAlarmEnabled = !fsm->isAlarmEnabled;
        lcd_setCursor(0, 1);
        fprintf(lcdout, "Joystick=show_alrm");
        if (fsm->isAlarmEnabled == true)
        {
            lcd_setCursor(0, 2);
            fprintf(lcdout, "Alarm on");
            led_yellowInit();
            led_yellowOn();
        }
        else if (fsm->isAlarmEnabled == false)
        {
            lcd_setCursor(0, 2);
            fprintf(lcdout, "Alarm off");
            led_yellowOff();
        }
        return RET_HANDLED;
        break;
    case JOYSTICK_PRESSED:
        return TRANSITION(display_alarm_time);
        break;
    case TIME_MATCH:
        return TRANSITION(sound_alarm);
        break;
    default:
        return RET_IGNORED;
        break;
    }
}

fsmReturnStatus display_alarm_time(Fsm *fsm, const Event *event) //< from this state, pressing rotary button we can set alarm time
{
    Fsm *pAlarm_time;
    pAlarm_time = &alarm_time;
    switch (event->signal)
    {
    case ENTRY:
        lcd_setCursor(0, 1);
        fprintf(lcdout, "Alarmtime");
        lcd_setCursor(0, 2);
        fprintf(lcdout, "%02d::%02d", pAlarm_time->timeSet.hour, pAlarm_time->timeSet.minute);
        lcd_setCursor(0, 3);
        fprintf(lcdout, "Rotary=set_alrm");
        return RET_HANDLED;
        break;
    case JOYSTICK_PRESSED:
        return TRANSITION(normal_mode);
        break;
    case ROTARY_PRESSED:
        return TRANSITION(set_alarm_hr);
        break;
    default:
        return RET_IGNORED;
        break;
    }
}

fsmReturnStatus set_alarm_hr(Fsm *fsm, const Event *event)
{
    Fsm *pAlarm_time;
    pAlarm_time = &alarm_time;
    switch (event->signal)
    {
    case ENTRY:
        lcd_setCursor(0, 1);
        fprintf(lcdout, "Alarm hr");
        lcd_setCursor(0, 2);
        fprintf(lcdout, "%02d::%02d", pAlarm_time->timeSet.hour, pAlarm_time->timeSet.minute);
        lcd_setCursor(0, 3);
        fprintf(lcdout, "Rotary=hr++");
        return RET_HANDLED;
        break;
    case JOYSTICK_PRESSED:
        return TRANSITION(set_alarm_min);
        break;
    case ROTARY_PRESSED:
        pAlarm_time->timeSet.hour++;
        pAlarm_time->timeSet.hour = (pAlarm_time->timeSet.hour) % HR_PER_DAY;
        lcd_setCursor(0, 1);
        fprintf(lcdout, "Alarm hr");
        lcd_setCursor(0, 2);
        fprintf(lcdout, "%02d::%02d", pAlarm_time->timeSet.hour, pAlarm_time->timeSet.minute);
        lcd_setCursor(0, 3);
        fprintf(lcdout, "Joystick=Alarm min");
        return RET_HANDLED;
        break;
    default:
        return RET_IGNORED;
        break;
    }
}

fsmReturnStatus set_alarm_min(Fsm *fsm, const Event *event)
{
    Fsm *pAlarm_time;
    pAlarm_time = &alarm_time;
    switch (event->signal)
    {
    case ENTRY:
        lcd_setCursor(0, 1);
        fprintf(lcdout, "Alarm min");
        lcd_setCursor(0, 2);
        fprintf(lcdout, "%02d::%02d", pAlarm_time->timeSet.hour, pAlarm_time->timeSet.minute);
        lcd_setCursor(0, 3);
        fprintf(lcdout, "Rotary=min++");
        return RET_HANDLED;
        break;
    case JOYSTICK_PRESSED:
        return TRANSITION(normal_mode);
        break;
    case ROTARY_PRESSED:
        pAlarm_time->timeSet.minute++;
        pAlarm_time->timeSet.minute = (pAlarm_time->timeSet.minute) % MIN_PER_HR;
        lcd_setCursor(0, 1);
        fprintf(lcdout, "Alarm min");
        lcd_setCursor(0, 2);
        fprintf(lcdout, "%02d::%02d", pAlarm_time->timeSet.hour, pAlarm_time->timeSet.minute);
        lcd_setCursor(0, 3);
        fprintf(lcdout, "Joystick=normal mode");
        return RET_HANDLED;
        break;

    default:
        return RET_IGNORED;
        break;
    }
}

fsmReturnStatus sound_alarm(Fsm *fsm, const Event *event)
{
    switch (event->signal)
    {
    case ENTRY:
        lcd_setCursor(0, 1);
        fprintf(lcdout, "Alarm running");
        lcd_setCursor(0, 2);
        fprintf(lcdout, "Presss button");
        return RET_HANDLED;
        break;
    case JOYSTICK_PRESSED:
        return TRANSITION(normal_mode);
        break;
    case ROTARY_PRESSED:
        return TRANSITION(normal_mode);
        break;
    case TIMEOUT:
        return TRANSITION(normal_mode);
        break;
    case EXIT:
        lcd_setCursor(0, 2);
        fprintf(lcdout, "Snoozed"); //< disable the alarm on exit
        fsm->isAlarmEnabled = false;
        return RET_HANDLED;
        break;
    default:
        return RET_IGNORED;
        break;
    }
}

/***************** Functions to return the alarm time set by the user ***************/
uint8_t get_alarm_time_hr()
{
    return alarm_time.timeSet.hour;
}

uint8_t get_alarm_time_min()
{
    return alarm_time.timeSet.minute;
}
