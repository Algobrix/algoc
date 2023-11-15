#include <algobot.h>

void application(ALGOC_APP)
{
	waitSignal(ALGOC,2,true);
	light(ALGOC,1,FOREVER,10,"Red",false);
	wait(ALGOC,1);
	waitSignal(ALGOC,2,true);
	stopLight(ALGOC,1);
}
