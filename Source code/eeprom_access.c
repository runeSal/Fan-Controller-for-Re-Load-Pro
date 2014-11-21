/*
 * eeprom_access.c
 *
 * Created: 06-11-2014 01:27:34
 * Author: Rune Salberg-Bak
 */

#include <stdint.h>
#include <avr/io.h>
#include <avr/eeprom.h>

#include "global_var.h"

uint16_t EEMEM EE_FAN_T_MAX		=	49*16; //=49°C
uint16_t EEMEM EE_FAN_T_MIN		=	40*16; //=40°C
uint16_t EEMEM EE_FAN_T_START	=	39*16; //=39°C
uint16_t EEMEM EE_FAN_T_OFF		=	38*16; //=38°C
uint16_t EEMEM EE_FAN_DUTY_MIN	=	35; //=20%

void eeprom_update(void)
{
	eeprom_update_word(&EE_FAN_T_MAX, FAN_T_MAX);
	eeprom_update_word(&EE_FAN_T_MIN, FAN_T_MIN);
	eeprom_update_word(&EE_FAN_T_START, FAN_T_START);
	eeprom_update_word(&EE_FAN_T_OFF, FAN_T_OFF);
	eeprom_update_word(&EE_FAN_DUTY_MIN, FAN_DUTY_MIN);
}

void eeprom_to_SRAM(void)
{
	FAN_T_MAX	=	eeprom_read_word(&EE_FAN_T_MAX);
	FAN_T_MIN	=	eeprom_read_word(&EE_FAN_T_MIN);
	FAN_T_START	=	eeprom_read_word(&EE_FAN_T_START);
	FAN_T_OFF	=	eeprom_read_word(&EE_FAN_T_OFF);
	FAN_DUTY_MIN	=	eeprom_read_word(&EE_FAN_DUTY_MIN);
}