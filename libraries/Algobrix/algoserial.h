/* Define to prevent recursive inclusion *********************************** */
#ifndef __ALGOSERIAL_H
#define __ALGOSERIAL_H
/* Includes **************************************************************** */
#include <Arduino.h>
#include <system.h>
#include <algothread.h>


/* Module configuration **************************************************** */

/* Exported constants ****************************************************** */

/* Exported macros ********************************************************* */

/* Exported types ********************************************************** */

/* Exported variables ****************************************************** */

/* Exported functions ****************************************************** */

template <typename Var> void write(System name,Var line)
{
	while(g_ALGOBOT_INFO.state == ALGOBOT_STATE_PAUSE)
	{
		yield();
	}
	if(name.cthread.sequance != name.sequance)
	{
		return;
	}
	Serial.print(line);
	name.cthread.sequance++;
}
template <typename Var> void writeLine(System name,Var line)
{
	while(g_ALGOBOT_INFO.state == ALGOBOT_STATE_PAUSE)
	{
		yield();
	}
	if(name.cthread.sequance != name.sequance)
	{
		return;
	}
	Serial.println(line);
	name.cthread.sequance++;
}

float enterNumber(System name);
char * enterString(System name);
char enterChar(System name);

#endif 

