#ifndef LCD
#define LCD

#include <avr/io.h>
#include <util/delay.h>

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
void LCD_shiftOut(unsigned char data);
void LCD_shiftTic(void);
void LCD_latchTic(void);
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

LCD_drawType LCD_lastDraw = NILDRAW;
static unsigned char LCD_dataPin, LCD_shiftPin, LCD_latchPin, LCD_RS, LCD_RW, LCD_E, LCD_busyFlag;	//for LCD_lastSentMethod, 0=command,1=character
static volatile uint8_t* LCD_portPointer = 0;

void LCD_init(volatile uint8_t* portPointer, unsigned char dataPin, unsigned char shiftPin, unsigned char latchPin, unsigned char RS, unsigned char RW, unsigned char E, unsigned char busyFlag)
{
	// Assign Global Variables
	LCD_portPointer = portPointer;
	LCD_dataPin = dataPin;
	LCD_shiftPin = shiftPin;
	LCD_latchPin = latchPin;
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

void LCD_shiftOut(unsigned char data)
{
	for(int i=0;i<8;i++)
	{
		if((data & (1<<(7-i))) == (1<<(7-i)))	//If bit in the 'data' byte is set . . .
		{
			*LCD_portPointer |= (1<<LCD_dataPin);	//set dataPin high
		}
		else	//Otherwise . . .
		{
			*LCD_portPointer &= ~(1<<LCD_dataPin);	//set datapin low
		}
		LCD_shiftTic();	//Then tic the data in with the shift-clock and repeat
	}
	LCD_latchTic();	//tic the latch to output from shift register
	*LCD_portPointer &= ~(1<<LCD_dataPin);
}

void LCD_shiftTic(void)
{
	*LCD_portPointer |= (1<<LCD_shiftPin);
	*LCD_portPointer &= ~(1<<LCD_shiftPin);
}

void LCD_latchTic(void)
{
	*LCD_portPointer |= (1<<LCD_latchPin);
	*LCD_portPointer &= ~(1<<LCD_latchPin);
}

void LCD_sendCommand(char command)
{
	LCD_waitForNotBusy();
	*LCD_portPointer &= ~(1<<LCD_RS | 1<<LCD_RW);
	LCD_shiftOut(command);
	LCD_flash();
}

void LCD_sendCharacter(char character)
{
	LCD_waitForNotBusy();
	*LCD_portPointer &= ~(1<<LCD_RW);
	*LCD_portPointer |= (1<<LCD_RS);
	LCD_shiftOut(character);
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