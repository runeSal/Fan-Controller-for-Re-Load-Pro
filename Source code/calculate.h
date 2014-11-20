/*
 * calculate.h
 *
 * Created: 02-11-2014 16:11:49
 * Author: Rune Salberg-Bak
 */


#ifndef CALCULATE_H_
#define CALCULATE_H_

#include "PWM.h"
#include "commen_def.h"

// Precision multiplication factor.
#define PM_FACTOR 128 

uint16_t cal_PWM_duty(uint16_t temperature, uint8_t *fan_state);

#endif /* CALCULATE_H_ */