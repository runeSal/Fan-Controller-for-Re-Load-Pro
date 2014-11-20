/*
 * calculate.c
 *
 * Created: 02-11-2014 16:11:33
 * Author: Rune Salberg-Bak
 */ 

#include <stdint.h>
#include <avr/io.h>

#include "PWM.h"
#include "calculate.h"

uint16_t FAN_T_MAX;
uint16_t FAN_T_MIN;
uint16_t FAN_T_START;
uint16_t FAN_T_OFF;
uint16_t FAN_PWM_MIN;

static uint16_t cal_PWM_slope()
{
	// the same as Round( (PWM_COUNTER_TOP-FAN_PWM_MIN)*P_FA/(FAN_T_MAX-FAN_T_MIN) )
	return( ((PWM_COUNTER_TOP-FAN_PWM_MIN)*PM_FACTOR+(FAN_T_MAX-FAN_T_MIN)/2)/(FAN_T_MAX-FAN_T_MIN) );
}

uint16_t cal_PWM_duty(uint16_t temperature, uint8_t *fan_state)
{
	if (*fan_state==ON)
	{
		if (temperature<FAN_T_OFF)
		{
			*fan_state = OFF;
			return 0;			
		}
		else if (temperature<=FAN_T_MIN)
		{
			return FAN_PWM_MIN;
		}
		else if (temperature>FAN_T_MAX)
		{
			return PWM_COUNTER_TOP;
		}
		else
		{
			uint32_t cal_32 = (temperature-FAN_T_MIN)*(uint32_t)cal_PWM_slope();
			uint16_t cal_16 = ((cal_32+PM_FACTOR/2)/PM_FACTOR);
			// Return PWM timer value.
			return (cal_16+FAN_PWM_MIN);
		}
	}
	else
	{
		if (temperature>=FAN_T_START)
		{
			*fan_state = ON;
			return FAN_PWM_MIN;
		}	
		else
			return 0;	
	}
}