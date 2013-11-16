#ifndef LCD
#define LCD

#include <avr/io.h>
#include <util/delay.h>
#include "lib/Serial.h"

/************************************
 * typedefs
 ************************************/
 
typedef enum {
	NILDRAW,
	DRAW_MENU_LOG,
	DRAW_MENU_SETTINGS,
	DRAW_MENU_TRANSFER,
	DRAW_MENU_CALIBRATION,
	DRAW_MENU_TIME,
	DRAW_LOG_STOP
} LCD_drawType;

void LCD_init(volatile uint8_t* portPointer, unsigned char dataPin, unsigned char shiftPin, unsigned char latchPin, unsigned char RS, unsigned char RW, unsigned char E, unsigned char busyFlag);
void LCD_sendCommand(char command);
void LCD_sendCharacter(char character);
void LCD_sendString(char* string);
void LCD_setLocation(int index);
void LCD_clear(void);
void LCD_setE(unsigned char E);
void LCD_flash(void);
void LCD_waitForNotBusy(void);
void LCD_draw(LCD_drawType type);

/***************************************
 * variable declarations
 ***************************************/

volatile LCD_drawType LCD_lastDraw= NILDRAW;
unsigned char LCD_RS;
unsigned char LCD_RW;
unsigned char LCD_E;
unsigned char LCD_busyFlag;
volatile uint8_t* LCD_portPointer = 0;

void LCD_init(volatile uint8_t* portPointer, unsigned char dataPin, unsigned char shiftPin, unsigned char latchPin, unsigned char RS, unsigned char RW, unsigned char E, unsigned char busyFlag)
{
	// Assign Global Variables
	Serial_init(0,portPointer,dataPin,shiftPin,latchPin);	//Interface 0 as default for LCD
	LCD_portPointer = portPointer;
	LCD_RS = RS;
	LCD_RW = RW;
	LCD_E = E;
	LCD_busyFlag = busyFlag;
	
	_delay_ms(15);
	LCD_sendCommand(0b00110000);	//Datasheet specified initialization commands...
	_delay_ms(5);
	LCD_sendCommand(0b00110000);	//...
	_delay_us(150);
	LCD_sendCommand(0b00110000);	//...
	_delay_ms(2);
	
	LCD_sendCommand(0b00111000);	//8-bit interface, 2 display lines, 5x8 dot font
	LCD_clear();					//Clear screen and set cursor to home position
	LCD_sendCommand(0b00000110);	//Cursor move direction set to right
	LCD_sendCommand(0b00001100);	//Display On, Cursor Off, Cursor Not Blinking
}

void LCD_sendCommand(char command)
{
	LCD_waitForNotBusy();
	*LCD_portPointer &= ~(1<<LCD_RS | 1<<LCD_RW);
	Serial_shiftOut(0,command);
	LCD_flash();
}

void LCD_sendCharacter(char character)
{
	LCD_waitForNotBusy();
	*LCD_portPointer &= ~(1<<LCD_RW);
	*LCD_portPointer |= (1<<LCD_RS);
	Serial_shiftOut(0,character);
	LCD_flash();
}

void LCD_sendString(char* string)
{
	unsigned int i = 0;
	while(string[i] != '\0')
	{
		if(i>15)
		{
			LCD_setLocation(0x40 + (i-16));
		}
		LCD_sendCharacter(string[i]);
		i++;
	}
	LCD_lastDraw = NILDRAW;
	return;
}

void LCD_setLocation(int index)
{
	LCD_sendCommand(0x80 + index);
}

void LCD_clear(void)
{
	LCD_sendCommand(0b00000001);
	_delay_ms(50);
	LCD_sendCommand(0b00000010);
	_delay_ms(50);
	LCD_lastDraw = NILDRAW;
}

void LCD_flash(void)
{
	*LCD_portPointer |= (1<<LCD_E);
	_delay_ms(2);
	*LCD_portPointer &= ~(1<<LCD_E);
}

void LCD_waitForNotBusy(void)
{
	*LCD_portPointer |= (1<<LCD_RW);
	*LCD_portPointer &= ~(1<<LCD_RS);
	while(!bit_is_clear(*LCD_portPointer,LCD_busyFlag))
	{}
}

void LCD_draw(LCD_drawType type)
{
	if(type != LCD_lastDraw)
	{
		switch(type)
		{
			case DRAW_MENU_LOG:
				LCD_clear();
				LCD_sendString("MENU            Begin Log...");
				break;
			case DRAW_MENU_SETTINGS:
				LCD_clear();
				LCD_sendString("MENU            Settings >>");
				break;
			case DRAW_MENU_TRANSFER:
				LCD_clear();
				LCD_sendString("MENU            Transfer...");
				break;
			case DRAW_MENU_CALIBRATION:
				LCD_clear();
				LCD_sendString("MENU            Calibrate...");
				break;
			case DRAW_MENU_TIME:
				LCD_clear();
				LCD_sendString("MENU            Set Time...");
				break;
			case DRAW_LOG_STOP:
				LCD_clear();
				LCD_sendString("Save: 'select'  Delete: 'back'");
				break;
			default:
				LCD_clear();
				LCD_sendString("Error");
				break;
		}
		LCD_lastDraw = type;
	}
	return;
}

#endif