/* Includes **************************************************************** */
#include <Arduino.h>
#include "algoC.h"
#include "algobreak.h"

/* Private types *********************************************************** */

/* Private constants ******************************************************* */

/* Private macros ********************************************************** */

/* Private variables ******************************************************* */

/* Private function prototypes ********************************************* */

/* Exported functions ****************************************************** */
uint8_t breakPoint(System name,char breakChar)
{
	if(name.cthread.sequance != name.sequance)
	{
		return 0;
	}

#ifdef SERIAL_ENABLE
    Serial.print("Reached break point at line[");
    Serial.print(name.line);
    Serial.print("]. Enter [");
    if(breakChar != 0)
    {
        Serial.print(breakChar);
    }
    else
    {
        Serial.print("any char");
    }
    Serial.println("] to continue");
#endif

    uint32_t time = getSYSTIM();
    while(1)
    {
        if(Serial.available())
        {
            if(breakChar == 0)
            {
                break;
            }
            else if(Serial.read() == breakChar )
            {
                break;
            }
        }
    }
    delay(100);
    while(Serial.available())
    {
        uint8_t tmp = Serial.read();
    }
    // now we need to update the timers based on the wait period
    uint32_t period = getElapsedTimeSYSTIM(time);
    MotorA.period += period;
    MotorB.period += period;
    MotorC.period += period;
    Light1.period += period;
    Light2.period += period;
    uint8_t k = 0;
    for(k = 0; k < THREAD_CNT; k++)
    {
        threads[k].waitPeriod += period;
    }
    threadAlgoC.waitPeriod += period;
    name.cthread.sequance++;
}

/* Private functions ******************************************************* */

/* ***************************** END OF FILE ******************************* */


