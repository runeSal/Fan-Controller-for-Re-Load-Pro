/*
 * main.c
 *
 * Created: 01-11-2014 13:44:24
 * Author: Rune Salberg-Bak
 */

#include <stdint.h>
#include <avr/io.h>
#include <avr/wdt.h>

#include "DS18B20.h"
#include "settings.h"
#include "commen_def.h"
#include "eeprom_access.h"
#include "PWM.h"
#include "global_var.h"
#include "UART.h"
#include "DC_DC_Con.h"

uint8_t setSignal_counter = 0;

static void init_timer0()
{
	// Set prescaler to 256.
	TCCR0B = (1<<CS02);
	
	// Timer 0 Compare Register.
	OCR0A = 144;
	
	/* (3.6864MHz/(N*OCR+1))^-1 = time     (3.6864MHz/(256*144+1))^-1 = 10.00ms*/
}

static void setSignal()
{
	//Turn the fan on to tell the user that the UART FAN setting was successfully.
	
	setSignal_counter--;
	//Turn on DC-DC converter.
	DC_DC_CON_TURN_ON;	
	PWM_setDuty(PWM_COUNTER_TOP); // Fan 100% ON
}

static void unused_pin_pullUp()
{
	PORTB |= ((1<<PORTB1)|(1<<PORTB3)|(1<<PORTB4)|(1<<PORTB5)|(1<<PORTB6)|(1<<PORTB7));
	PORTD |= ((1<<PORTD2)|(1<<PORTD3)|(1<<PORTD4)|(1<<PORTD5)|(1<<PORTD6));
}

int main(void)
{
	//Turn off DC-DC converter.
	DC_DC_CON_TURN_OFF;
	//Set DC-DC converter ON_OFF pin to output.
	DC_DC_CON_PIN_OUT;
	PWM_init();
	eeprom_to_SRAM();
	init_timer0();
	UART_init();
	unused_pin_pullUp();
	#if WATCHDOG==ON
	wdt_enable(WDTO_2S);
	#endif
	asm("SEI");
	
    while(1)
    {
		/*------------- TASK RUN EVERY 10ms -------------*/
		if (setSignal_counter==0)
			DS18B20_StateMachine();
		else
			setSignal(); // Run when a fan value was set.
		
		if (update_eeprom==ON)
		{
			update_eeprom = OFF;
			setSignal_counter = 50; //=500ms
			eeprom_update();
		}
		/*-----------------------------------------------*/
		
		while((TIFR&(1<<OCIE0A))==0)
		{
			// Wait for Compare on timer0 Flag gets high.
		}
		// Reset timer
		TCNT0 = 0;
		// Clear Compare Flag
		TIFR |= 1<<OCIE0A;
    }
}