#include <algoC.h>


void application(ALGOC_APP)
{
  waitPlayButtonPress(ALGOC,3);
  light(ALGOC,'1',FOREVER,10,"Blue",false);
  repeat(ALGOC,1)
  {
    if(getPlayButtonState(ALGOC) == true)
    {
      break;
    }
  }
  playButtonReset(ALGOC);
}

