#include <algoC.h>

void application(ALGOC_APP)
{
    move(ALGOC,'A',FOREVER,5,CW,false);
    move(ALGOC,'B',4,5,cw,true);
    stopMotor(ALGOC,'A'); 
}
