#ifndef SES_PWM_H
#define SES_PWM_H

/*INCLUDES *******************************************************************/

#include "ses_common.h"

/* FUNCTION PROTOTYPES *******************************************************/

/**
 * Initializes the fast PWM mode
 */
void pwm_init(void);

/**
 * set the value of the register OCR0B for dutycycle
 */
void pwm_setDutyCycle(uint8_t dutyCycle);

#endif /* SES_PWM_H */