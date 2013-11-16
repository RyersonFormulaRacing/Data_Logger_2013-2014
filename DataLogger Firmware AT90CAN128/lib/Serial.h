#ifndef Serial
#define Serial

#include <avr/io.h>

#ifndef SERIAL_INTERFACES
#define SERIAL_INTERFACES 1
#endif

/***************************************
 * function prototypes
 ***************************************/
 
void Serial_init(uint8_t interface, volatile uint8_t* portPointer, unsigned char dataPin, unsigned char shiftPin, unsigned char latchPin);
void Serial_shiftOut(uint8_t interface, unsigned char data);
void Serial_shiftTic(uint8_t interface);
void Serial_latchTic(uint8_t interface);

/***************************************
 * variable declarations
 ***************************************/

volatile unsigned char Serial_dataPin[SERIAL_INTERFACES];
volatile unsigned char Serial_shiftPin[SERIAL_INTERFACES];
volatile unsigned char Serial_latchPin[SERIAL_INTERFACES];
volatile uint8_t* Serial_portPointer[SERIAL_INTERFACES] = {0};

/***************************************
 * function definitions
 ***************************************/

void Serial_init(uint8_t interface, volatile uint8_t* portPointer, unsigned char dataPin, unsigned char shiftPin, unsigned char latchPin)
{
	// Assign Global Variables
	Serial_portPointer[interface] = portPointer;
	Serial_dataPin[interface] = dataPin;
	Serial_shiftPin[interface] = shiftPin;
	Serial_latchPin[interface] = latchPin;
}

void Serial_shiftOut(uint8_t interface, unsigned char data)
{
	for(int i=0;i<8;i++)
	{
		if((data & (1<<(7-i))) == (1<<(7-i)))	//If bit in the 'data' byte is set . . .
		{
			*Serial_portPointer[interface] |= (1<<Serial_dataPin[interface]);	//set dataPin high
		}
		else	//Otherwise . . .
		{
			*Serial_portPointer[interface] &= ~(1<<Serial_dataPin[interface]);	//set datapin low
		}
		Serial_shiftTic(interface);	//Then tic the data in with the shift-clock and repeat
	}
	Serial_latchTic(interface);	//tic the latch to output from shift register
	*Serial_portPointer[interface] &= ~(1<<Serial_dataPin[interface]);
}

void Serial_shiftTic(uint8_t interface)
{
	*Serial_portPointer[interface] |= (1<<Serial_shiftPin[interface]);
	*Serial_portPointer[interface] &= ~(1<<Serial_shiftPin[interface]);
}

void Serial_latchTic(uint8_t interface)
{
	*Serial_portPointer[interface] |= (1<<Serial_latchPin[interface]);
	*Serial_portPointer[interface] &= ~(1<<Serial_latchPin[interface]);
}

#endif