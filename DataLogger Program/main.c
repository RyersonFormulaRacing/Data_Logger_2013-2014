#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <stdio.h>

void press(void);
void sendChar(char data);
void sendString(char* data);
void sendString_l(char* data, int length);
void UART_Init(void);
void Ports_Init(void);
void ADC_Init(void);
void Clock_Init(void);

char *DataStream = NULL;
char sTime[10];
char naTime[4];
unsigned char receiveData = 0;
bool startDataStream = false;	//Previously int
int channel = 0;
int SpaceAllocated = 1050;
int BitsWritten = 0;
unsigned int Counter = 0;
unsigned int lastCounter = 0;
unsigned long nTime = 0;

int main(void)
{
	ADC_Init();
	Clock_Init();
	Ports_Init();
	UART_Init();
	//char adcResult[5];
	DataStream = malloc(SpaceAllocated);
	
	while(1)
	{
		/*Increment counter and nTime every 10000 ticks*/
		if(TCNT1 >= 10000)
		{
			TCNT1 = 0;
			nTime ++;
			Counter++;
			if(Counter>10000)
			{
				Counter = 0;
			}
		}
		
		/*If there's data in the UART recieve buffer, handle it*/
		if (UCSRA & (1 << RXC))
		{
			receiveData = UDR;
			//sendChar(receiveData);
			if (receiveData == 'a') PORTB |= (1<<PINB2);
			else if (receiveData == 'b') PORTB &= ~(1<<PINB2);
		}
		
		/*If DataStream is started, and the counter divides evenly by 4, and the adc has data ready, record and output the data*/
		if ((ADCSRA & (1 << ADIF)) && Counter%4 == 0 && lastCounter != Counter && startDataStream)
		{
			lastCounter = Counter;
			//itoa(ADCH, adcResult, 10);
			//sendChar('(');
			//sendChar(channel+88);
			//sendChar(')');
			//sendString(adcResult);
			sendChar(ADCH);
			//sendChar(',');
			if(channel==2)
			{
				//sendChar('\n');
				//itoa(nTime, sTime, 10);
				//sendString(sTime);		//Time
				//sendChar(nTime);
				for(int i=0;i<4;i++)
				{
					naTime[i] = (nTime & (0x000000FFUL<<(i*8))) >> (i*8);
				}
				sendString_l(naTime,4);
				//sendChar(',');
			}
			//_delay_ms(1000);
			ADCSRA &= ~(1<<ADIF);
			
			if(channel <= 0)
			{
				ADMUX = 0b00100000;		//X
				channel++;
			}
			else if(channel == 1)
			{
				ADMUX = 0b00100001;		//Y
				channel++;
			}
			else if(channel >= 2)
			{
				ADMUX = 0b00100010;		//Z
				channel = 0;
			}
			
			ADCSRA |= 1<<ADSC;
		}
		
		if (bit_is_clear(PINB, 0))
		{
			Pressed_Confidence_Level++;
			Released_Confidence_Level = 0;
			if (Pressed_Confidence_Level > 100)
			{
				if (Pressed == 0)
				{
					Pressed = 1;
					if(startDataStream)startDataStream = 0;
					else if(!startDataStream)startDataStream = 1;
					nTime = 0;
					press();					
				}
				Pressed_Confidence_Level = 0;
			}
		}
		else
		{
			Released_Confidence_Level++;
			Pressed_Confidence_Level = 0;
			if (Released_Confidence_Level > 100)
			{
				Pressed = 0;
				Released_Confidence_Level = 0;
			}
		}
	}
}

void press(void)
{
	PORTB ^= (1<<PINB1);
	if(startDataStream)
	{
		sendChar(0);
		sendChar(0);
		sendChar(0);
		sendChar(0);
	}
}

void sendChar(char data, int n)				//Send character "data" over tx n times
{
	for(int i=0;i<n;i++)					//Send the character n times
	{
		while(! (UCSRA & (1 << UDRE)) );	//Wait for UART controller to be ready
		UDR = data;							//Assign data to UART output register to be handled
	}
}

void sendString(char* data)					//Send the null-terminated string pointed to by "data" over tx
{
	int i = 0;
	while(data[i] != '\0')
	{
		while(! (UCSRA & (1 << UDRE)) );	//Wait for UART controller to be ready
		UDR = data[i];
		i++;
	}
}

void sendString_l(char* data, int length)	//Send "length" bytes of the string "data" over tx
{
	int i = 0;
	while(i<length)
	{
		while(! (UCSRA & (1 << UDRE)) );	//Wait for UART controller to be ready
		UDR = data[i];
		i++;
	}
}

void UART_Init(void)						//Initialize UART 
{
	//UART Specifications(Parity,stop bits, data bit length)
	UCSRC &= ~(1 << UMSEL);
	
	int UBRR = 12; //9600bps
	
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
}

void Ports_Init(void)
{
	//Set LED pins for output and turn off the leds
	DDRB |= 1<<PINB1 | 1<<PINB2;
	PORTB &= ~(1<<PINB1 | 1<<PINB2);
	
	//Initialization of Button and debouncing variables
	DDRB &= ~(1<<PINB0);
	PORTB |= 1<<PINB0;
	int Pressed = 0;
	int Pressed_Confidence_Level = 0;
	int Released_Confidence_Level = 0;
}


void ADC_Init(void)
{
	//Configure the ADC
	ADCSRA |= 1<<ADPS2;		//ADC Prescaler division factor 16
	ADMUX |= 1<<ADLAR;		//Left Adjusted Result
	ADCSRA |= 1<<ADEN;		//ADC Enable
	ADCSRA |= 1<<ADSC;		//ADC Start First Convertion
}

void Clock_Init(void)
{
	TCCR1B |= 1<<CS10;	//No Prescalling; 1 MHz
}