#ifndef SES_MOTORFREQUENCY_H
#define SES_MOTORFREQUENCY_H

/*INCLUDES *******************************************************************/

#include "ses_common.h"

/* FUNCTION PROTOTYPES *******************************************************/

/**
 * Initializes external interrupt at PORTD0 pin to sense the rising edge
 * caused by motor current drops. INT0 is used
 */
void motorFrequency_init();

/**
 * Returns the most recent frequency measurement in hertz
 */
uint16_t motorFrequency_getRecent();

/**
 * Filters the erroneous measurements occured in motorFrequency_getRecent
 */
uint16_t motorFrequency_getMedian();

#endif /* SES_MOTORFREQUENCY_H */