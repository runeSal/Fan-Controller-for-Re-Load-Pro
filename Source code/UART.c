/*
 * UART.c
 *
 * Created: 05-11-2014 12:54:42
 * Author: Rune Salberg-Bak
 */

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "UART.h"
#include "commen_def.h"
#include "global_var.h"
#include "PWM.h"

uint8_t rx_state = 0;
uint8_t rx_setPointer;
uint16_t rx_setValue;
uint8_t update_eeprom = OFF;

void UART_init(void)
{
	// Set UART to 15200 baud 8/n/1
	
	// RX Complete Interrupt Enable
	// Receiver Enable
	UCSRB = (1<<RXCIE) | (1<<RXEN);
	
	// 8 data bits
	UCSRC = (1<<UCSZ1) | (1<<UCSZ0);
	
	UBRRH = 0;
	UBRRL = 1;
}

static void set_FanRegValue(uint8_t setPointer, uint16_t setValue)
{
	if (setPointer<5)
		setValue *= 16;
		
	switch (setPointer)
	{
	case 1:
		FAN_T_MAX = setValue;
		break;
	case 2:
		FAN_T_MIN = setValue;
		break;
	case 3:
		FAN_T_START = setValue;
		break;
	case 4:
		FAN_T_OFF = setValue;
		break;
	case 5:
	default:
		FAN_PWM_MIN = (setValue*PWM_COUNTER_TOP+50)/100; //Calculate setValue% of PWM_COUNTER_TOP value.
		break;
	}
}

ISR(USART0_RX_vect)
{	
	do 
	{
		if (!(UCSRA&(1<<DOR)))
		{
			//No Data OverRun
			
			uint8_t rx_data = UDR;
			if (rx_state<4)
			{
				if (rx_state==0)
				{
					if (rx_data=='F')
						rx_state++;//rx_state=1;
					else
						rx_state=0;
				}
				else if(rx_state==1)
				{
					if (rx_data=='A')
						rx_state++;//rx_state=2;	
					else
						rx_state=0;		
				}
				else if(rx_state==2)
				{
					if (rx_data>='1' && rx_data<='5')
					{
						rx_state++;//rx_state=3;
						rx_setPointer = rx_data-'0';
						rx_setValue = 0;
					}
					else
						rx_state=0;
				}
				else //if(rx_state==3)
				{
					if (rx_data=='_')
						rx_state++;//rx_state=4;	
					else
						rx_state=0;		
				}			
			}
			else
			{
				// rx_state is 4 or over.
				
				if (rx_data>='0' && rx_data<='9')
				{
					// Conversion from ASCII to value
					rx_setValue += rx_data-'0';
					if (rx_state==4)
					{
						rx_state = 5;
						rx_setValue *= 10;				
					}
					else if (rx_state==5)
					{
						rx_state=0;
						set_FanRegValue(rx_setPointer,rx_setValue);
						update_eeprom = ON;
					}
					else
						rx_state=0;
				}
				else
				{
					rx_state=0;
				}
			}		
		}
		else
		{
			// Receive buffer is full and has lost received data. 
			
			rx_state=0;
			// Disabling the Receiver will flush the receive buffer.
			UCSRB &= ~(1<<RXEN);// Disable Receiver.
			// Enable Receiver.
			UCSRB |= 1<<RXEN;		
		}
	
	// Is there unread data in the receive buffer. Then go to interrupt start, else end interrupt.
	} while (UCSRA&(1<<RXC));
}