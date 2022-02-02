#define F_CPU 16000000UL

#include <avr/io.h>

/*
 *  useful macros for setting data directions in a data direction register (DDR)
 *
 *
 *  Setting, clearing, and reading bits in registers.
 *	reg is the name of a register; pin is the index (0..7)
 *  of the bit to set, clear or read.
 *  (WRITE_BIT is a combination of CLEAR_BIT & SET_BIT)
 */
#define SET_BIT(reg, pin)			(reg) |= (1 << (pin))
#define CLEAR_BIT(reg, pin)			(reg) &= ~(1 << (pin))
#define WRITE_BIT(reg, pin, value)	(reg) = (((reg) & ~(1 << (pin))) | ((value) << (pin)))
#define BIT_VALUE(reg, pin)			(((reg) >> (pin)) & 1)
#define BIT_IS_SET(reg, pin)		(BIT_VALUE((reg),(pin))==1)


//Functions declaration
void setup(void);
void process(void);
void uart_init(unsigned int ubrr);
unsigned char uart_getchar(void);
void uart_putchar(unsigned char data);


//UART definitions
//define baud rate 
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1


//receiving buffer
unsigned char rx_buf;


void setup(void) {
  
	// initialise uart 
	uart_init(MYUBRR);

	// Enable B5 as output, led on B5
	SET_BIT(DDRB, 5);

}

void process(void) {
  
   //define a character to sent
    static char sent_char = 'a';
  
   //send serial data
	uart_putchar(sent_char);
  
    //receive serial data
    rx_buf = uart_getchar(); 
  
  	//toggle the LED to indicate data has been received
  	if (rx_buf =='a')
      	PORTB ^= (1<<PB5);
  
    sent_char++;
   //reset and start from 'a' again
	if ( sent_char > 'z' ) sent_char = 'a';
  
  
}

int main(void) {
	setup();

	for ( ;; ) {
		process();
		_delay_ms(100);
	}
}





/*  ****** serial definitions ************ */

// Initialize the UART
void uart_init(unsigned int ubrr) {
	
	UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)(ubrr);
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (3 << UCSZ00);
	
}

// Transmit data
void uart_putchar(unsigned char data) {
	
  
    while (!( UCSR0A & (1<<UDRE0))); /* Wait for empty transmit buffer*/
          
    UDR0 = data;            /* Put data into buffer, sends the data */
        	
}

// Receive data
unsigned char uart_getchar(void) {
/* Wait for data to be received */ 
  while ( !(UCSR0A & (1<<RXC0)) );
  
/* Get and return received data from buffer */
return UDR0;
	
}