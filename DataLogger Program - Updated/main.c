/*******************************************************************************
*------------------------------Copyright Notice--------------------------------*
* Company: Ryerson Formula Racing                                              *
* Author(s): Caleb A. McNevin                                                  *
********************************************************************************
* 2013-1014 season datalogger main program                                     *
*******************************************************************************/

#define numberOfButtons 4
#define WELCOME_MESSAGE "Datalogger V1.0"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lib/ButtonPress.h"
#include "lib/LCD.h"
#include "lib/UART.h"
#include "lib/StateMachine.h"

/****************************************
 * global variable declarations
 ****************************************/
 char INT0_called = 0;
 
 
 //Port B
	
	const unsigned char lcd_busyFlag = PINB0;
	const unsigned char led1 = PINB1;
	const unsigned char lcd_E = PINB2;
	const unsigned char lcd_data = PINB3;
	const unsigned char lcd_latch = PINB4;
	const unsigned char lcd_shift = PINB5;
	const unsigned char lcd_RS = PINB6;
	const unsigned char lcd_RW = PINB7;
	
//Port D
	
	const unsigned char buttonUp = PIND6;
	const unsigned char buttonDown = PIND5;
	const unsigned char buttonSelect = PIND4;
	const unsigned char buttonBack = PIND3;
	const unsigned char externalInterrupt = PIND2;

// Main function

int main(void)
{
	
	while(1)
	{
		switch(currentState)
		{
			case POWER_ON:
				break;
			case INITIALIZE:
				
				//Data Direction Settings
				DDRB |= (1<<led1 | 1<<lcd_E | 1<<lcd_data | 1<<lcd_latch | 1<<lcd_shift | 1<<lcd_RS | 1<<lcd_RW);
				DDRB &= ~(1<<lcd_busyFlag);
				DDRD &= ~(1<<buttonUp | 1<<buttonDown | 1<<buttonSelect | 1<<buttonBack | 1<<externalInterrupt);
				
				//Port Initialization
				PORTD |= (1<<buttonUp | 1<<buttonDown | 1<<buttonSelect | 1<<buttonBack | 1<<externalInterrupt);
				
				//LCD Initialization
				LCD_init(&PORTB,lcd_data,lcd_shift,lcd_latch,lcd_RS,lcd_RW,lcd_E,lcd_busyFlag);
				
				//UART Initialization
				UART_init();
				
				//Global Interrupt initialization
				sei();
				
				//Enable external interrupts on INT0
				GICR |= (1<<INT0);
				
				//External Interrupt initialization for INT0/PIND2 - Any Edge
				//MCUCR |= (1<<ISC01);
				MCUCR &= ~(1<<ISC00 | 1<<ISC01);
				
				//Welcome Message
				LCD_sendString(WELCOME_MESSAGE);
				_delay_ms(1500);
				
				break;
			case MENU_LOG:
				LCD_draw(DRAW_MENU_LOG);
				break;
			case LOG_INITIALIZE:
				break;
			case LOG_READ:
				break;
			case LOG_STORE:
				break;
			case LOG_INPUT:
				break;
			case LOG_STOP:
				LCD_draw(DRAW_LOG_STOP);
				break;
			case MENU_SETTINGS:
				LCD_draw(DRAW_MENU_SETTINGS);
				break;
			case MENU_TRANSFER:
				LCD_draw(DRAW_MENU_TRANSFER);
				break;
			case MENU_CALIBRATION:
				LCD_draw(DRAW_MENU_CALIBRATION);
				break;
			case MENU_TIME:
				LCD_draw(DRAW_MENU_TIME);
				break;
			default:
				UART_sendString("Fatal Error: Outside defined state definitions");
				LCD_clear();
				LCD_sendString("Fatal Error:    switch state err");
				break;
		}
		if(!INT0_called)
		{
			stateEval(DEFAULT);
		}
		INT0_called = 0;
	}
}

ISR(USART_RXC_vect)
{
	UART_buffer[UART_index] = UDR;
	UART_sendCharacter(UART_buffer[UART_index]);
	if(UART_buffer[UART_index] == '\r' || UART_index>=39)
	{
		UART_buffer[UART_index]='\0';
		LCD_clear();
		LCD_sendString(&UART_buffer[0]);
		//parseStringInput(&UART_buffer[0]);	//not yet defined
		UART_clear();
		if(UART_index>=39)
		{
			UART_sendString("Buffer Overflow!");
		}
		UART_index = 0;
	}
	else
	{
		UART_index++;
	}
	if(UART_buffer[UART_index-1] == 8)	//backspace
	{
		UART_index -= 2;
	}
}

ISR(INT0_vect)	//Intterrupt for button presses (falling edge)
{
	/*static unsigned int Time_Held_Low;
	char ButtonPressed = 0;
	
	if(!bit_is_clear(PIND,externalInterrupt))
	{
		Time_Held_Low = TCNT1;
	}
	
	if(TCNT1 >= Time_Held_Low)
	{
		if((TCNT1 - Time_Held_Low) > 32000)
		{
			ButtonPressed = 1;
		}
	}
	else if(((TCNT1 + 65535) - Time_Held_Low) > 32000)
	{
		ButtonPressed = 1;
	}
	
	if(ButtonPressed)
	{*/
	_delay_ms(150);
	event eventOccured = -1;
	int buttonsPressed = 0;
	INT0_called = 1;
	
	
	
	if(bit_is_clear(PIND,buttonUp))
	{
		eventOccured = UP;
		buttonsPressed++;
	}
	if(bit_is_clear(PIND,buttonDown))
	{
		eventOccured = DOWN;
		buttonsPressed++;
	}
	if(bit_is_clear(PIND,buttonSelect))
	{
		eventOccured = SELECT;
		buttonsPressed++;
	}
	if(bit_is_clear(PIND,buttonBack))
	{
		eventOccured = BACK;
		buttonsPressed++;
	}
	
	if(buttonsPressed > 1)
	{
		//Future error reporting system
		LCD_clear();
		LCD_sendString("ERROR - > Button");
		return;
	}
	else if(buttonsPressed == 0)
	{
		LCD_clear();
		LCD_sendString("ERROR - < Button");
		return;
	}
	
	if(eventOccured >= 0 && eventOccured < numberOfButtons)
	{
		switch (stateEval(eventOccured))	//call action
		{
			case NILACTION:
				UART_sendString("NILACTION Called\n");
				//LCD_clear();
				//LCD_sendString("NILACTION Called");
				//_delay_ms(1000);
				break;
			case Log_save:
				UART_sendString("Log_save Called\n");
				LCD_clear();
				LCD_sendString("Log_save Called");
				_delay_ms(1000);
				break;
			case Log_delete:
				UART_sendString("Log_delete Called\n");
				LCD_clear();
				LCD_sendString("Log_delete      Called");
				_delay_ms(1000);
				break;
			case Menu_transfer:
				UART_sendString("Menu_transfer Called\n");
				LCD_clear();
				LCD_sendString("Menu_transfer   Called");
				_delay_ms(1000);
				break;
			case Menu_calibrate:
				UART_sendString("Menu_calibrate Called\n");
				LCD_clear();
				LCD_sendString("Menu_calibrate  Called");
				_delay_ms(1000);
				break;
			case Menu_setTime:
				UART_sendString("Menu_setTime Called\n");
				LCD_clear();
				LCD_sendString("Menu_setTime    Called");
				_delay_ms(1000);
				break;
			default:
				UART_sendString("default Error\n");
				LCD_clear();
				LCD_sendString("NILACTION Called");
				_delay_ms(1000);
				break;
		}
	}
	//}
}

/*ISR(ADC_vect)
{
	
}*/