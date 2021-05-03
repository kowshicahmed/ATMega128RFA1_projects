/* INCLUDES ******************************************************************/
#include "ses_timer.h"

/* DEFINES & MACROS **********************************************************/
			

#define TIMER2_CYC_FOR_1MILLISEC 249 		// with 64 prescaler, 250 cycles are required for 1ms time
#define TMR2_TCCRB_VAL_PRESCALER_0 0x00 	// TCCRB register value for prescaler 0 or stop the timer
#define TMR2_TCNT2_VAL_TOCOMPARE 0x00 		// this value is compared with the value of OCR2A register

#define TIMER1_CYC_FOR_5MILLISEC 1249       // with 64 prescaler, 1250 cycles are required for 5ms time  
#define TMR1_TCCRB_VAL_PRESCALER_0 0x00 	// TCCRB register value for prescaler 0 or stop the timer
#define TMR1_TCNT1_VAL_TOCOMPARE 0x0000 	// this value is compared with the value of OCR1A register

/*Function pointers should not be accessible outside module (static) and must not be touched by GCC
optimization (volatile)*/
volatile static pTimerCallback timer1_callback = NULL;
volatile static pTimerCallback timer2_callback = NULL;

/*FUNCTION DEFINITION ********************************************************/

//timer 2 starts

void timer2_setCallback(pTimerCallback cb)
{
	timer2_callback = cb; 					// save the function passed in 'cb' to the function pointer
}

void timer2_start()
{
	cli(); 									// clear global interrupt
	//TCCR2A |= (1 << COM2A1); 				// set operation mode to clear on match
	//TCCR2A &= ~(1 << COM2A0);
    
    TCCR2B &= ~(1<<WGM22);                 //CTC mode
	TCCR2A |= (1<<WGM21);
	TCCR2A &= ~(1<<WGM20);

	TCCR2B |= (1<<CS22); 					// setting the pre-scaler to 64
	TCCR2B &= ~(1<<CS21); 
	TCCR2B &= ~(1<<CS20);

	OCR2A = TIMER2_CYC_FOR_1MILLISEC;
	TCNT2 = TMR2_TCNT2_VAL_TOCOMPARE;
	TIMSK2 |= (1 << OCIE2A); 				// enable compare match interrupt for timer 2A
	sei();					 				// enable global interrupt
}

void timer2_stop()
{
	TIMSK2 &= ~(1 << OCIE2A); 				// disable compare match interrupt for timer 2A
	TCCR2B = TMR2_TCCRB_VAL_PRESCALER_0;	// timer 2 stopped
}

//Timer 1 starts

void timer1_setCallback(pTimerCallback cb)
{
	timer1_callback = cb;                  // save the function passed in 'cb' to the function pointer
}

void timer1_start()
{
	cli(); 									// clear global interrupt
	TCCR1A |= (1 << COM1A1); 				// set operation mode to clear on match
	TCCR1A &= ~(1 << COM1A0);

	TCCR1B &= ~(1 << WGM13);				//CTC mode
	TCCR1B |= (1 << WGM12);
	TCCR1A &= ~(1 << WGM11); 
	TCCR1A &= ~(1 << WGM10);				
	
	TCCR1B &= ~(1<<CS12);					//prescaler 64
	TCCR1B |= (1<<CS11); 
	TCCR1B |= (1<<CS10);

	TCNT1 = TMR1_TCNT1_VAL_TOCOMPARE;     	// TCNT1 high and low value combined to TCNT1
	OCR1A = TIMER1_CYC_FOR_5MILLISEC;   
	TIMSK1 |= (1 << OCIE1A); 				// enable compare match interrupt for timer 1A
	sei();					 				// enable global interrupt
}

void timer1_stop()
{
	TIMSK1 &= ~(1 << OCIE1A); 				// disable compare match interrupt for timer 1A
	TCCR1B = TMR1_TCCRB_VAL_PRESCALER_0;	// timer 1 stopped
}

ISR(TIMER1_COMPA_vect)
{
	if(timer1_callback != NULL)     // if the callback points to a valid function
	    timer1_callback();
}

ISR(TIMER2_COMPA_vect)
{	
	if (timer2_callback != NULL) 	// if the callback points to a valid function
		timer2_callback();
}
