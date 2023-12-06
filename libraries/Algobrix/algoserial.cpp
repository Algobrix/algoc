/* Includes **************************************************************** */
#include <algobot.h>
#include "algoserial.h"
#include <systim.h>

/* Private types *********************************************************** */

/* Private constants ******************************************************* */

/* Private macros ********************************************************** */

/* Private variables ******************************************************* */
/* Private function prototypes ********************************************* */

/* Exported functions ****************************************************** */

float enterNumber(System name)
{
	while(g_ALGOBOT_INFO.state == ALGOBOT_STATE_PAUSE)
	{
		yield();
	}
	if(name.cthread.sequance != name.sequance)
	{
		return 0;
	}
	while(Serial.available() == 0)
	{
		blinkLed();
	}
	char buffer[32];
	uint8_t k = 0;
	uint32_t timer = getSYSTIM();
	while(chk4TimeoutSYSTIM(timer,200) == SYSTIM_KEEP_ALIVE)
	{

	}
	while(Serial.available() != 0)
	{
		if(k < 32)
		{
			buffer[k++] = Serial.read();
		}
	}
	float number = atof(&buffer[0]);
	name.cthread.sequance++;
	return number;
}

static char s_algoserial_buffer[32];
char * enterString(System name)
{
	while(g_ALGOBOT_INFO.state == ALGOBOT_STATE_PAUSE)
	{
		yield(); 
	} 
	if(name.cthread.sequance != name.sequance)
	{
		return 0;
	}
	while(Serial.available() == 0)
	{
		blinkLed();
	}
	uint8_t k = 0;
	uint32_t timer = getSYSTIM();
	while(chk4TimeoutSYSTIM(timer,200) == SYSTIM_KEEP_ALIVE)
	{

	}

	while(Serial.available() != 0)
	{
		if(k < 32)
		{
			s_algoserial_buffer[k++] = Serial.read();
		}
	}
	s_algoserial_buffer[k] = '\0';
	name.cthread.sequance++;
	return s_algoserial_buffer;
}

char enterChar(System name)
{
	while(g_ALGOBOT_INFO.state == ALGOBOT_STATE_PAUSE)
	{
		yield();
	}
	if(name.cthread.sequance != name.sequance)
	{
		return 0;
	}
	while(Serial.available() == 0)
	{
		blinkLed();
	}
	char buffer[32];
	uint8_t k = 0;
	uint32_t timer = getSYSTIM();
	while(chk4TimeoutSYSTIM(timer,200) == SYSTIM_KEEP_ALIVE)
	{

	}

	while(Serial.available() != 0)
	{
		if(k < 32)
		{
			buffer[k++] = Serial.read();
		}
	}

	name.cthread.sequance++;
	return buffer[0];
}
/* Private functions ******************************************************* */

/* ***************************** END OF FILE ******************************* */


