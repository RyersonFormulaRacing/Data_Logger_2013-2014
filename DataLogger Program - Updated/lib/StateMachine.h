#ifndef StateMachine
#define StateMachine

#include <avr/io.h>

/*******************************
 * typedefs
 *******************************/

typedef enum {
	ERROR_BUTTON
} error;

typedef enum {
	POWER_ON,
	INITIALIZE,
	MENU_LOG,
	LOG_INITIALIZE,
	LOG_READ,
	LOG_STORE,
	LOG_INPUT,
	LOG_STOP,
	MENU_SETTINGS,
	MENU_TRANSFER,
	MENU_CALIBRATION,
	MENU_TIME	   
} state;

typedef enum {
	NILACTION,
	Log_save,
	Log_delete,
	Menu_transfer,
	Menu_calibrate,
	Menu_setTime
} action;

typedef enum {
	UP,
	DOWN,
	SELECT,
	BACK,
	DEFAULT
} event;

typedef struct {
      state nextState;
      action actionToDo;
}  stateElement;

stateElement stateMatrix[12][5] = {
	{{POWER_ON,NILACTION},{POWER_ON,NILACTION},{POWER_ON,NILACTION},{POWER_ON,NILACTION},{INITIALIZE,NILACTION}},
	{{INITIALIZE,NILACTION},{INITIALIZE,NILACTION},{INITIALIZE,NILACTION},{INITIALIZE,NILACTION},{MENU_LOG,NILACTION}},
	{{MENU_SETTINGS,NILACTION},{MENU_TRANSFER,NILACTION},{LOG_INITIALIZE,NILACTION},{MENU_LOG,NILACTION},{MENU_LOG,NILACTION}},
	{{LOG_INITIALIZE,NILACTION},{LOG_INITIALIZE,NILACTION},{LOG_INITIALIZE,NILACTION},{LOG_INITIALIZE,NILACTION},{LOG_READ,NILACTION}},
	{{LOG_READ,NILACTION},{LOG_READ,NILACTION},{LOG_READ,NILACTION},{LOG_READ,NILACTION},{LOG_STORE,NILACTION}},
	{{LOG_STORE,NILACTION},{LOG_STORE,NILACTION},{LOG_STORE,NILACTION},{LOG_STORE,NILACTION},{LOG_INPUT,NILACTION}},
	{{LOG_INPUT,NILACTION},{LOG_INPUT,NILACTION},{LOG_INPUT,NILACTION},{LOG_STOP,NILACTION},{LOG_READ,NILACTION}},
	{{LOG_STOP,NILACTION},{LOG_STOP,NILACTION},{MENU_LOG,Log_save},{MENU_LOG,Log_delete},{LOG_STOP,NILACTION}},
	{{MENU_TRANSFER,NILACTION},{MENU_LOG,NILACTION},{MENU_CALIBRATION,NILACTION},{MENU_SETTINGS,NILACTION},{MENU_SETTINGS,NILACTION}},
	{{MENU_LOG,NILACTION},{MENU_SETTINGS,NILACTION},{MENU_LOG,Menu_transfer},{MENU_TRANSFER,NILACTION},{MENU_TRANSFER,NILACTION}},
	{{MENU_TIME,NILACTION},{MENU_TIME,NILACTION},{MENU_SETTINGS,Menu_calibrate},{MENU_SETTINGS,NILACTION},{MENU_CALIBRATION,NILACTION}},
	{{MENU_CALIBRATION,NILACTION},{MENU_CALIBRATION,NILACTION},{MENU_SETTINGS,Menu_setTime},{MENU_SETTINGS,NILACTION},{MENU_TIME,NILACTION}}
};


/*****************************************
 * global variable declarations
 *****************************************/
 
state currentState = POWER_ON;
action actionToDo = NILACTION;


/*****************************************
 * prototypes
 *****************************************/
 
action stateEval(event e);

action stateEval(event e)
{
	//cli();
	stateElement stateEvaluation = stateMatrix[currentState][e];
	
	currentState = stateEvaluation.nextState;
	
	//sei();
	return stateEvaluation.actionToDo;
}



#endif