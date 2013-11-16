#ifndef ButtonPress
#define ButtonPress

#ifndef numberOfButtons
#define numberOfButtons 1
#endif

#include <avr/io.h>

typedef enum {
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_SELECT,
	BUTTON_BACK
} buttonName;

char ButtonPressed(buttonName button, unsigned char pinOfButton, unsigned char portBit, int confidenceLevel);

char ButtonPressed(buttonName button, unsigned char pinOfButton, unsigned char portBit, int confidenceLevel)
{
	static char Pressed[numberOfButtons];
	static int Pressed_Confidence_Level[numberOfButtons]; //Measure button press confidence
	static int Released_Confidence_Level[numberOfButtons]; //Measure button release confidence
	if (bit_is_clear(pinOfButton, portBit))
	{
		Pressed_Confidence_Level[button] ++; //Increase Pressed Confidence
		Released_Confidence_Level[button] = 0; //Reset released confidence since there's a button press
		if (Pressed_Confidence_Level[button] > confidenceLevel) //Indicator of good button press
		{
			if(Pressed[button] == 0)
			{
				Pressed[button] = 1;
				return 1;
			}
			//Zero it so a new pressed condition can be evaluated
			Pressed_Confidence_Level[button] = 0;
		}
	}
	else
	{
		Released_Confidence_Level[button] ++; //increased released confidence level
		Pressed_Confidence_Level[button] = 0; //Reset pressed confidence level since the button has been released
		if (Released_Confidence_Level[button] > confidenceLevel)
		{
			Pressed[button] = 0;
			Released_Confidence_Level[button] = 0;
		}
	}
	return 0;
}

#endif