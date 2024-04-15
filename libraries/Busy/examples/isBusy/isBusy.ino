#include <algoC.h>


void application(ALGOC_APP)
{
	bool motorBusy = false;
	bool lightBusy = false;
	bool soundBusy = false;

	move(ALGOC,'A',2,10,1,false);
	light(ALGOC,1,2,10,"Blue",false);
	playSound(ALGOC,1,1,false);


	motorBusy = isMotorBusy(ALGOC,'A');
	write(ALGOC, "Motor Busy? ");
	writeLine(ALGOC,motorBusy);
	lightBusy = isLightBusy(ALGOC,1);
	write(ALGOC, "Light Busy? ");
	writeLine(ALGOC,lightBusy);
	soundBusy = isSoundBusy(ALGOC);
	write(ALGOC, "Sound Busy? ");
	writeLine(ALGOC,soundBusy);

	wait(ALGOC,10);

	motorBusy = isMotorBusy(ALGOC,'A');
	write(ALGOC, "Motor Busy? ");
	writeLine(ALGOC,motorBusy);
	lightBusy = isLightBusy(ALGOC,1);
	write(ALGOC, "Light Busy? ");
	writeLine(ALGOC,lightBusy);
	soundBusy = isSoundBusy(ALGOC);
	write(ALGOC, "Sound Busy? ");
	writeLine(ALGOC,soundBusy);
}
