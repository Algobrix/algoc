#include <algoC.h>

void application(ALGOC_APP)
{
	waitSensor(ALGOC,1,1,4);
	light(ALGOC,1,FOREVER,10,"Red",false);
	waitSensor(ALGOC,1,5,10);
	stopLight(ALGOC,1);
}
