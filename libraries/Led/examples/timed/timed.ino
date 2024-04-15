#include <algoC.h>

void application(ALGOC_APP)
{
	light(ALGOC,1,FOREVER,10,"Red",false);
	wait(ALGOC,4);
	stopLight(ALGOC,1);
}
