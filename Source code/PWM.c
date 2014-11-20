/*
 * PWM.c
 *
 * Created: 02-11-2014 15:55:26
 * Author: Rune Salberg-Bak
 */

#include <stdint.h>
#include <avr/io.h>

#include "PWM.h"

void PWM_setDuty(uint16_t duty)
{
	if (duty==0)
	{
		// Need to disconnected OC1A because then TCCR1A is set to 0 the duty cycle is not 0.
		TCCR1A = 0;	// OC1A disconnected
	}
	else
	// Clear OC1A on compare match
	// Set OC1A at BOTTOM, (non-inverting mode)
	// Set Counter to Compare Output Mode, Fast PWM.
	TCCR1A = (1<<COM1A1) | (1<<WGM11);

	/* Set duty-cycle */
	OCR1A = duty;
}

static void PWM_setFreq(uint16_t counter_top)
{
	DDRB |= (1<<PORTB3); //PB3 pin set to output.
	// Set Timer/Counter top
	ICR1 = counter_top;
}

void PWM_init()
{
	/* Set Timer/Counter Mode of Operation to Fast PWM. TOP=ICR1 */

	PWM_setFreq(PWM_COUNTER_TOP);
	
	PWM_setDuty(0);
	
	//Fast PWM
	//TOP: ICR1
	//No prescaling
	TCCR1B = (1<<WGM13) | (1<<WGM12) | (1<< CS10);
}