#ifndef UART
#define UART

#include <avr/io.h>

/****************************************
 * function prototypes
 ****************************************/

void UART_init(void);
void UART_sendCharacter(char character);
void UART_sendString(char* string);
void UART_clear(void);

/****************************************
 * global variable declarations
 ****************************************/

char UART_buffer[40];	//buffer for input from Rx
unsigned int UART_index;	//index for UART_buffer

/****************************************
 * function definitions
 ****************************************/

void UART_init(void)	//Initialize UART
{
	//UART Specifications(Parity,stop bits, data bit length)
	UCSRC &= ~(1 << UMSEL);
	
	int UBRR = 34; //57600 Baud
	
	UBRRH &= ~(1 << URSEL);
	//Put the upper part of the baud number here (bits 8 to 11)
	UBRRH = (unsigned char) (UBRR >> 8);

	//Put the remaining part of the baud number here
	UBRRL = (unsigned char) UBRR; 

	//Enable the receiver and transmitter
	UCSRB = (1 << RXEN) | (1 << TXEN);

	//Set frame format to 8 data bits, no parity, 1 stop bit
	UCSRC = (1 << URSEL) | (0 << USBS) | (3 << UCSZ0);
	
	UCSRA = (1 << U2X);		//Double Speed USART
	
	//Interrupt Initialization
	
	UCSRB |= (1<<RXCIE); //Enable UART Reviever Interrupt
}

void UART_sendCharacter(char character)			//Send "character" over tx
{
	while(! (UCSRA & (1 << UDRE)) );			//Wait for UART controller to be ready
	UDR = character;
}

void UART_sendString(char* string)				//Send the null-terminated "string" over tx
{
	int i = 0;
	while(string[i] != '\0')
	{
		while(! (UCSRA & (1 << UDRE)) );	//Wait for UART controller to be ready
		UDR = string[i];
		i++;
	}
}

void UART_clear(void)
{
	UART_sendCharacter(0x0c);
	UART_sendCharacter('\r');
	return;
}

#endif