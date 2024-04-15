/* Includes **************************************************************** */
#include <Arduino.h>
#include "algobutton.h"
#include "algoC.h"
#include "systim.h"

/* Private types *********************************************************** */

/* Private constants ******************************************************* */

/* Private macros ********************************************************** */

/* Private variables ******************************************************* */
uint8_t g_button_fuction_acitve_flag;
/* Private function prototypes ********************************************* */

/* Exported functions ****************************************************** */
void playButtonReset(System name)
{
	if(name.cthread.sequance != name.sequance)
	{
		return;
	}
	g_button_fuction_acitve_flag = 0;
	PCIFR = 0b00000000; //Clear all flags
	PCMSK1 |= 0x01;
	name.cthread.sequance++;
}

int waitPlayButtonPress(System name,int numberOfPresses)
{
	if(name.cthread.sequance != name.sequance)
	{
		return 0;
	}
	if(numberOfPresses <= 0)
	{
		name.cthread.sequance++;
	}
	g_button_fuction_acitve_flag = 1;
	PCMSK1 &= ~0x01;
	PCIFR = 0b00000000; //Clear all flags
	yield();
	int press_counter = 0;
	uint8_t state = digitalRead(PLAY_BUTTON_PIN);
	uint8_t p_state = state;
	Serial.println("Waiting for button press");
	while(1)
	{
		delay(10);
		yield();
		state = digitalRead(PLAY_BUTTON_PIN);
		// Serial.println(state);
		if(state != p_state)
		{
			p_state = state;
			if(state == 1)
			{
				++press_counter;
#ifdef SERIAL_ENABLE
				Serial.print("Press detected: ");
				Serial.print(press_counter);
				Serial.print("/");
				Serial.print(numberOfPresses);
#endif
			}
		}
		if(press_counter >= numberOfPresses)
		{
			break;
		}
	}
	name.cthread.sequance++;
	g_button_fuction_acitve_flag = 0;
	PCIFR = 0b00000000; //Clear all flags
	PCMSK1 |= 0x01;
	return press_counter;
}

bool getPlayButtonState(System name)
{
	if(name.cthread.sequance != name.sequance)
	{
		return 0;
	}
	g_button_fuction_acitve_flag = 1;
	PCMSK1 &= ~0x01;
	PCIFR = 0b00000000; //Clear all flags
	name.cthread.sequance++;
	delay(10);
	return !digitalRead(PLAY_BUTTON_PIN);
}

/* Private functions ******************************************************* */

/* ***************************** END OF FILE ******************************* */


