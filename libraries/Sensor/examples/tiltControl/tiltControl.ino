#include <GoAlgo.h>

void application(ALGOC_APP)
{
    uint8_t newValue;
    uint8_t lastValue;
    while(1)
    {
          newValue = waitSensor(ALGOC,1,0,4);
          if(lastValue != newValue)
          {
            lastValue = newValue;
            switch(lastValue)
            {
              case(0):
              {
                stopLight(ALGOC,1);
                stopLight(ALGOC,2);
                break;
              }
              case(1):
              {
                light(ALGOC,1,FOREVER,10,"Red",false);
                stopLight(ALGOC,2);
                break;
              }
              case(2):
              {
                light(ALGOC,1,FOREVER,10,"Blue",false);
                stopLight(ALGOC,2);
                break;
              }
              case(3):
              {
                light(ALGOC,1,FOREVER,10,"White",false);
                stopLight(ALGOC,2);
                break;
              }
              case(4):
              {
                light(ALGOC,1,FOREVER,10,"Green",false);
                stopLight(ALGOC,2);
                break;
              }
            }
          }
      
    }
}
