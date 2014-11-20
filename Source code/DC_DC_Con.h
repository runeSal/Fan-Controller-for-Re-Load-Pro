/*
 * DC_DC_Con.h
 *
 * Created: 09-11-2014 22:51:15
 * Author: Rune Salberg-Bak
 */

#ifndef DC_DC_CON_H_
#define DC_DC_CON_H_

#define HIGH 1
#define LOW 0
#define DC_DC_CON_ACTIVE LOW

#define IO_DCDC_CON			1
#define IO_DCDC_CON_PORT	PORTD
#define IO_DCDC_CON_DDR		DDRD

#define IO_PIN_SET_HIGH(bit) (bit ## _PORT |= (1<<bit))
#define IO_PIN_SET_LOW(bit) (bit ## _PORT &= ~(1<<bit))

#define IO_PIN_SET_TO_OUT(bit) (bit ## _DDR |= (1<<bit))


#if DC_DC_CON_ACTIVE==HIGH
	#define DC_DC_CON_TURN_ON	IO_PIN_SET_HIGH(IO_DCDC_CON)
	#define DC_DC_CON_TURN_OFF	IO_PIN_SET_LOW(IO_DCDC_CON)
#else
	#define DC_DC_CON_TURN_ON   IO_PIN_SET_LOW(IO_DCDC_CON)
	#define DC_DC_CON_TURN_OFF	IO_PIN_SET_HIGH(IO_DCDC_CON)
#endif

#define DC_DC_CON_PIN_OUT  IO_PIN_SET_TO_OUT(IO_DCDC_CON)
 
#endif /* DC_DC_CON_H_ */