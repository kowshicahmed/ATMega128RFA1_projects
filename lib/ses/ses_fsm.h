#ifndef SES_FSM_H
#define SES_FSM_H

/*INCLUDES *******************************************************************/

#include "ses_common.h"
#include "ses_scheduler.h"

typedef struct fsm_s Fsm;     //< typedef for alarm clock state machine
typedef struct event_s Event; //< event type for alarm clock fsm

/** return values */
enum
{
    RET_HANDLED,   //< event was handled
    RET_IGNORED,   //< event was ignored; not used in this implementation
    RET_TRANSITION //< event was handled and a state transition occurred
};

typedef uint8_t fsmReturnStatus; //< typedef to be used with above enum

/** possible events */
enum
{
    ENTRY,            //< action performed in the state upon entry
    JOYSTICK_PRESSED, //< action performed when joystick button is pressed
    ROTARY_PRESSED,   //< action performed when rotary button is pressed
    TIME_MATCH,
    TIMEOUT,
    EXIT //< action performed on exit
};

/** typedef for state event handler functions */
typedef fsmReturnStatus (*State)(Fsm *, const Event *);

struct fsm_s
{
    State state;           //< current state, pointer to event handler
    bool isAlarmEnabled;   //< flag for the alarm status
    struct time_t timeSet; //< multi-purpose var for system time and alarm time
};

struct event_s
{
    uint8_t signal; //< identifies the type of event
};

/* FUNCTION PROTOTYPES *******************************************************/
/* dispatches events to state machine, called in application*/
inline static void fsm_dispatch(Fsm *fsm, const Event *event)
{
    static Event entryEvent = {.signal = ENTRY};
    static Event exitEvent = {.signal = EXIT};
    State s = fsm->state;
    fsmReturnStatus r = fsm->state(fsm, event);
    if (r == RET_TRANSITION)
    {
        s(fsm, &exitEvent);           //< call exit action of last state
        fsm->state(fsm, &entryEvent); //< call entry action of new state
    }
}

/* sets and calls initial state of state machine */
inline static void fsm_init(Fsm *fsm, State init)
{
    //... other initialization
    Event entryEvent = {.signal = ENTRY};
    fsm->state = init;
    fsm->state(fsm, &entryEvent);
}

/**
 * System time hour wrapper function
 */
uint8_t setTime_wrapper_hr(Fsm *user_time, uint8_t hour);

/**
 * System time minute wrapper function
 */
uint8_t setTime_wrapper_min(Fsm *user_time, uint8_t minute);

/**
 * States of the alarm time FSM
 */
fsmReturnStatus idle(Fsm *fsm, const Event *event);
fsmReturnStatus hour_set(Fsm *fsm, const Event *event);
fsmReturnStatus minute_set(Fsm *fsm, const Event *event);
fsmReturnStatus normal_mode(Fsm *fsm, const Event *event);
fsmReturnStatus display_alarm_time(Fsm *fsm, const Event *event);
fsmReturnStatus set_alarm_hr(Fsm *fsm, const Event *event);
fsmReturnStatus set_alarm_min(Fsm *fsm, const Event *event);
fsmReturnStatus sound_alarm(Fsm *fsm, const Event *event);

/**
 * Return alarm time set by the user
 */
uint8_t get_alarm_time_hr();
uint8_t get_alarm_time_min();

#endif /* SES_FSM_H */