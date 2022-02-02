/*
Setting data directions in a data direction register (DDR) 
*/

/*

Setting, clearing, and reading bits in registers.
reg is the name of a register; pin is the index (0..7)
of the bit to set, clear or read.
(WRITE_BIT is a combination of CLEAR_BIT & SET_BIT) */
#define SET_BIT(reg, pin) (reg) |= (1 << (pin)) #define CLEAR_BIT(reg, pin) (reg) &= ~(1 << (pin)) #define WRITE_BIT(reg, pin, value) (reg) = (((reg) & ~(1 << (pin))) | ((value) << (pin))) #define BIT_VALUE(reg, pin) (((reg) >> (pin)) & 1) #define BIT_IS_SET(reg, pin) (BIT_VALUE((reg),(pin))==1)

/* File: turn_on_led.c
 * Description: C program for the ATMEL AVR microcontroller (ATmega328 chip)
 * Turn on LED 
 * 
 * Includes (pretty much compulsory for using the Teensy this semester)
 * 	- avr/io.h: port and pin definitions (i.e. DDRB, PORTB, PB1, etc)
 *
 */
// AVR header file for all registers/pins

#include <avr/io.h> 


int main(void) {

	// LED 0 is connected to pin B4 (port B)
	// set PORTB pin 4 for output
	DDRB=0b00010000;
	
   // turn ON LEDs by sending '1' to pin 4
	PORTB=0b00010000;

	return 0;

}


/**********************************
// version using bit shift


#include <avr/io.h> 


int main(void) {

	// LED 0 is connected to pin B4 (port B)
	// set PORTB pin 4 for output
	DDRB = 1 << PINB4;
	
   // turn ON LEDs by sending '1' to pin 4
	PORTB = 1 << PINB4;

	return 0;

}
*/
