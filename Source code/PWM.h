/*
 * PWM.h
 *
 * Created: 02-11-2014 15:55:41
 * Author: Rune Salberg-Bak
 */

#ifndef PWM_H_
#define PWM_H_

#define PWM_COUNTER_TOP 175// give a PWM frequency on 21kHz with a 3.6864Mhz clock.
#define PWM_MIN 0

void PWM_setDuty(unsigned int duty);
void PWM_init();

#endif /* PWM_H_ */