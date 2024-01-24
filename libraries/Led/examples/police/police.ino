#include <GoAlgo.h>

void application(ALGOC_APP)
{
  uint8_t k = 0;
  for( k = 0; k < 10; k++)
  {
    light(ALGOC,1,0.2,10,"Red",true);
    light(ALGOC,2,0.2,10,"Blue",true);
  }
}
