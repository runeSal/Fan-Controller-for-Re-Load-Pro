/*
 * DS18B20.h
 *
 * Created: 01-11-2014 14:19:41
 * Author: Rune Salberg-Bak
 */

#ifndef DS18B20_H_
#define DS18B20_H_

#include "commen_def.h"

/*------------- 1-WIRE BUS TIMEINGS -------------*/
//RESET
#define RESET_TIME_LOW 500 //us
#define WAIT_FOR_PRESENCE_PULSE 110 //us
#define WAIT_FOR_ROM_COMMAND 385 //us
//READ
#define READ_TIME_LOW 2 //us 
#define SAMPLE_TIME 7 //us  
#define WAIT_FOR_NEXT_TIME_SLOT 62 //us
//WIRTE
#define WIRTE_1_LOW_TIME 2 //us
#define WIRTE_0_LOW_TIME 70 //us
#define WAIT_TIME_FOR_NEXT_BIT_0 2 //us
#define WAIT_TIME_FOR_NEXT_BIT_1 70 //us
/*-----------------------------------------------*/

#define IO_ONE_WIRE_BUS			PORTB0
#define IO_ONE_WIRE_BUS_PORT	PORTB
#define IO_ONE_WIRE_BUS_PIN		PINB
#define IO_ONE_WIRE_BUS_DDR		DDRB

#define MSB 7
	
//PORT PD? must always be 0. To work as input(Hi-Z) when DDR is 0, and output(pull bus line low) when DDR is 1.
#define IO_PULL_LOW(bit) (bit ## _DDR |= (1<<bit))		 // Pull bus line low
#define IO_EXTERN_PULL_UP(bit) (bit ## _DDR &= ~(1<<bit))// High-Z
#define IO_PIN_READ(bit) (bit ## _PIN & (1 << bit))		 // Read bus line


typedef enum {STATE_INIT, STATE_READ_SCRATCHPAD, STATE_START_CONVERSION, STATE_READ_TEMP, STATE_WAIT}DS18B20_STATES;	

void DS18B20_StateMachine();

#endif /* DS18B20_H_ */