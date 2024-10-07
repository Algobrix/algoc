/*
  * Documentation on the Distance AlgoSensor:
  * Senses distance between 2 - 100 centimeters
  * The 100 cm is divided between 0-10 in the code
  * 0-2 cm will match sensor output of 0 --> Sense below 2 cm --> False
  * 2-20 .......................... of 1-2 --> Low / True
  * 20-50 ......................... of 3-5 --> Med / True
  * 50-100 ........................ of 5-10 --> High / True
  * 100+ ...........................of 0 --> Sense above 100 cm --> False
*/
/* Define to prevent recursive inclusion *********************************** */
#ifndef __SENSOR_H
#define __SENSOR_H

/* Includes **************************************************************** */
#include <Arduino.h>
#include "algothread.h"
#include <system.h>
#include <owi.h>

/* Exported constants ****************************************************** */
#define NUM_OF_SENSORS 2
/* #define PULSE_TIMEOUT 4000ul // ul = unsigned long */
#define PULSE_TIMEOUT 10000ul // ul = unsigned long
#define CYCLE_TIME 2000.0f // 2 MS @ Microseconds 500Hz Frequency // f = float
                           //
						   //
enum ALGOSENSOR_TYPE
{
    ALGOSENSOR_TYPE_PWM = 0x00,
    ALGOSENSOR_TYPE_1WIRE,
};
/* Exported macros ********************************************************* */

/* Exported types ********************************************************** */
class AlgoSensor 
{
    private:
        uint8_t pin;
        uint8_t type;
        OWI * owi;
		uint8_t p_value;
        
    public:
        AlgoSensor(uint8_t pin,OWI * owi);
        int getValue();
};

/* Exported variables ****************************************************** */
extern AlgoSensor Sensor1;
extern AlgoSensor Sensor2;


/* Exported functions ****************************************************** */
int Sensor(AlgoSensor & sensor);
int Sensor(uint8_t port);

int waitSensor(System name,int sensorPort, int minSignalValue, int maxSignalValue);
void waitForPressSensor(System name,int sensorPort, bool logicState);

int getSensor(System name,int sensorPort);

#endif 
/* ***************************** END OF FILE ******************************* */

