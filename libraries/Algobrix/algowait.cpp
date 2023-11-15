/* Includes **************************************************************** */
#include "algowait.h"
#include "algobot.h"

/* Private constants ******************************************************* */


/* Private macros ********************************************************** */

/* Private types *********************************************************** */

/* Private variables ******************************************************* */

/* Private function prototypes ********************************************* */

/* Exported functions ****************************************************** */
void wait(System name,float seconds)
{
	if(name.cthread.sequance != name.sequance)
	{
		return;
	}
	yield();
	switch(name.cthread.waitState)
	{
		case(ALGOTHREAD_WAIT_STATE_INIT):
		{
			Serial.print("Wait on line [");
			Serial.print(name.line);
			Serial.print("] for [");
			Serial.print(seconds);
			Serial.println("] seconds");
			name.cthread.waitTimer = getSYSTIM();
            name.cthread.waitPeriod = seconds * 1000;
			if(&name.cthread == &threadAlgoC)
			{
				while(chk4TimeoutSYSTIM(name.cthread.waitTimer,name.cthread.waitPeriod) == SYSTIM_KEEP_ALIVE)
				{
					yield();
					if(g_ALGOBOT_INFO.state != ALGOBOT_STATE_RUN)
					{
						name.cthread.sequance++;
						name.cthread.waitState = ALGOTHREAD_WAIT_STATE_INIT;
						return;
					}
				}
				name.cthread.waitState = ALGOTHREAD_WAIT_STATE_INIT;
				name.cthread.sequance++;
				return;
			}
			else
			{
				name.cthread.waitState = ALGOTHREAD_WAIT_STATE_RUN;
				return;
			}
			break;
		}
		case(ALGOTHREAD_WAIT_STATE_RUN):
		{
			if(chk4TimeoutSYSTIM(name.cthread.waitTimer,name.cthread.waitPeriod))
			{
				name.cthread.waitState = ALGOTHREAD_WAIT_STATE_INIT;
				name.cthread.sequance++;
				return;
			}
			return;
			break;
		}
	}
}

/* Private functions ******************************************************* */

/* ***************************** END OF FILE ******************************* */
