#include <GoAlgo.h>

uint8_t level3 = 100;
uint8_t level2 = 10;
uint8_t level1 = 1;

void application(ALGOC_APP	)
{
	uint8_t cTouchTime = 0;
	uint8_t pTouchTime = cTouchTime;
	uint8_t led1State = 0;
	uint8_t led2State = 0;

	while(1)
	{
		cTouchTime = waitSensor(ALGOC,2,0,250);  
		if(cTouchTime != pTouchTime)
		{ 
			pTouchTime = cTouchTime;
			if(cTouchTime > level3)
			{
				// stop the code execution
				break;
			}
			else if(cTouchTime > level2)
			{
				if(led1State == 0)
				{
					led1State = 1;
					light(ALGOC,1,FOREVER,10,"Red",false);  
				}
				else
				{
					led1State = 0;
					light(ALGOC,1,FOREVER,0,"Red",false);  
				}
			}
			else if(cTouchTime > level1)
			{
				if(led2State == 0)
				{
					led2State = 1;
					light(ALGOC,2,FOREVER,10,"Red",false);  
				}
				else
				{
					led2State = 0;
					light(ALGOC,2,FOREVER,0,"Red",false);  
				}
			}
			wait(ALGOC,0.5); 
		}
    
	}
}
