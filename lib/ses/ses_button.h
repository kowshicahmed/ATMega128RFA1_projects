#ifndef SES_BUTTON_H_
#define SES_BUTTON_H_

/* INCLUDES ******************************************************************/

#include "ses_common.h"

/* FUNCTION PROTOTYPES *******************************************************/

/**
 * Initializes rotary encoder and joystick button
 */
void button_init(bool debouncing);

/** 
 * Get the state of the joystick button.
 */
bool button_isJoystickPressed(void);

/** 
 * Get the state of the rotary button.
 */
bool button_isRotaryPressed(void);

/** 
 * Function pointer for button callbacks.
 */
typedef void (*pButtonCallback)();

/** 
 * A function can be passed using this callback function 
 * which will be executed upon the corresponding button press.
 */
void button_setRotaryButtonCallback(pButtonCallback callback);

/** 
 * A function can be passed using this callback function 
 * which will be executed upon the corresponding button press.
 */
void button_setJoystickButtonCallback(pButtonCallback callback);

/** 
 * instead of using polling in a main loop or using direct interrupts 
 * which may lead to multiple button presses due to bouncing, 
 * this function will steer middle ground by polling the button state within a timer interrupt.
 */
void button_checkState();

#endif /* SES_BUTTON_H_ */
