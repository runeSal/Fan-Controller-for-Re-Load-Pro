/*
 * DS18B20.c
 *
 * Created: 01-11-2014 14:18:15
 * Author: Rune Salberg-Bak
 */

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/wdt.h>

#include "DS18B20.h"
#include "calculate.h"
#include "PWM.h"
#include "DC_DC_Con.h"
#include "settings.h"

DS18B20_STATES sensor_state = STATE_INIT; // Store the state of the DS18B20 sensor state machine.

uint8_t wait_counter; // Used when waiting for the next temperature measurement.

uint8_t fan_state = OFF; // Store the current Fan state(States:ON or OFF).

// Write a 0 bit to the 1-wire Bus
static void W_0()
{
	IO_PULL_LOW(IO_ONE_WIRE_BUS);
	_delay_us(WIRTE_0_LOW_TIME);
	IO_EXTERN_PULL_UP(IO_ONE_WIRE_BUS);
	_delay_us(WAIT_TIME_FOR_NEXT_BIT_0);
}

// Write a 1 bit to the 1-wire Bus
static void W_1()
{
	IO_PULL_LOW(IO_ONE_WIRE_BUS);
	_delay_us(WIRTE_1_LOW_TIME);
	IO_EXTERN_PULL_UP(IO_ONE_WIRE_BUS);
	_delay_us(WAIT_TIME_FOR_NEXT_BIT_1);
}

// Read the 1-wire Bus
static void Read_Bus()
{
	IO_PULL_LOW(IO_ONE_WIRE_BUS);
	_delay_us(READ_TIME_LOW);
	IO_EXTERN_PULL_UP(IO_ONE_WIRE_BUS);
	_delay_us(SAMPLE_TIME);
}

// Do a 1-wire Bus reset
static uint8_t oneWire_reset()
{
	//reset
	IO_PULL_LOW(IO_ONE_WIRE_BUS);
	_delay_us(RESET_TIME_LOW);
	IO_EXTERN_PULL_UP(IO_ONE_WIRE_BUS);
	_delay_us(WAIT_FOR_PRESENCE_PULSE);
	if (IO_PIN_READ(IO_ONE_WIRE_BUS)!=0) // read present pulse
	return 0xFF; //error: Bus line is high no present pulse.
	//reset end	
	return 0;
}

// Write 8bit data to the 1-wire bus
static void oneWire_w_data(uint8_t data)
{
	uint8_t count = 8;
	do 
	{
		//Send LSB in data out on the 1-wire bus.
		if (data&1)
			W_1();
		else
			W_0();
		data >>= 1;		
		count--;
	} while (count);
}

static uint8_t oneWire_do_command(uint8_t command)
{
	//reset
	if(oneWire_reset())
		return 0xFF;
	_delay_us(WAIT_FOR_ROM_COMMAND);
	//reset end
	
	//Send skip ROM command
	oneWire_w_data(0xCC);
	
	oneWire_w_data(command);
	return 0; // no errors.
}

//Read data from the 1-wire bus
static uint8_t oneWire_r()
{
	uint8_t count = 8;
	uint8_t return_value = 0;
	do
	{
		return_value >>= 1;
		Read_Bus();
		#if IO_ONE_WIRE_BUS==MSB
			return_value |= IO_PIN_READ(IO_ONE_WIRE_BUS);
		#else
		if (IO_PIN_READ(IO_ONE_WIRE_BUS)&(1<<IO_ONE_WIRE_BUS))
		{
			// If read pin == high
			return_value |= 0x80;
		}		
		#endif		
		_delay_us(WAIT_FOR_NEXT_TIME_SLOT);
		count--;	
		
	} while (count);	
	return return_value;
}

void DS18B20_StateMachine()
{
	asm("CLI");
	switch (sensor_state)
	{
	default:
	case STATE_INIT:
		//if (oneWire_do_command(0x4E)!=0); //WRITE SCRATCHPAD [4Eh]  //does not work!! Don't know way. 
			//break;
		oneWire_do_command(0x4E); //WRITE SCRATCHPAD [4Eh]
		oneWire_w_data(0xAA); //not used
		oneWire_w_data(0xAA); //not used
		oneWire_w_data(0x7F); // 12-bit resolution
		sensor_state = STATE_READ_SCRATCHPAD;
		break;
	case STATE_READ_SCRATCHPAD:
		oneWire_do_command(0xBE); //READ SCRATCHPAD [BEh]
		oneWire_r(); //not used
		oneWire_r(); //not used
		oneWire_r(); //not used
		oneWire_r(); //not used
		uint8_t read_data = oneWire_r();//Read Configuration Register	
		if (read_data!=0x7F) //Different from 12-bit resolution?
			sensor_state = STATE_INIT;
		else
			sensor_state = STATE_START_CONVERSION;
		oneWire_reset();
		break;
	case STATE_START_CONVERSION:
		oneWire_do_command(0x44); //CONVERT T [44h]
		sensor_state = STATE_READ_TEMP;
		break;
	case STATE_READ_TEMP:
		oneWire_do_command(0xBE); //READ SCRATCHPAD [BEh]
		uint16_t temp;
		temp = oneWire_r(); // LSB
		temp |= (uint16_t)oneWire_r()<<8; // MSB
		if(oneWire_reset())
			sensor_state = STATE_READ_SCRATCHPAD;
		asm("SEI");			
		// Check for sign bits in temp.
		if (temp&0xF000)
		{
			// There was sign bits present in temp.
			temp = 0; // Set to 0 because cal_PWM_duty does not handle sign vales.
		}	
		//Set duty cycle to the fan.
		PWM_setDuty(cal_PWM_duty(temp, &fan_state));
		wait_counter = 100; //=1 second
		sensor_state = STATE_WAIT;
		#if WATCHDOG==ON
		wdt_reset();
		#endif		
		break;
	case STATE_WAIT:
		if (wait_counter)
			wait_counter--;
		else
			sensor_state = STATE_START_CONVERSION;
		break;	
	}

	asm("SEI");
	if (fan_state==OFF)
	{
		//Turn off DC-DC converter.
		DC_DC_CON_TURN_OFF;
	}
	else
	{
		//Turn on DC-DC converter.
		DC_DC_CON_TURN_ON;
	}	
}