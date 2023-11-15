/* Includes **************************************************************** */
#include <algosensor.h>
#include <algobot.h>

/* Private constants ******************************************************* */


/* Private macros ********************************************************** */

/* Private types *********************************************************** */

/* Private variables ******************************************************* */
OWI owi1(PC3,(uint8_t *)& PINC,(uint8_t *)& PORTC,(uint8_t *)& DDRC);
OWI owi2(PC2,(uint8_t *)& PINC,(uint8_t *)& PORTC,(uint8_t *)& DDRC);
AlgoSensor Sensor1(SENSOR_A_PIN,&owi1);
AlgoSensor Sensor2(SENSOR_B_PIN,&owi2);

/* Private function prototypes ********************************************* */

/* Exported functions ****************************************************** */
AlgoSensor::AlgoSensor(uint8_t pin, OWI * owi) 
{
    this->pin = pin;
    this->owi = owi;
    pinMode(pin, INPUT);
}


uint8_t AlgoSensor::getValue() 
{
    float dutyCycle = 0;
    uint8_t value = 0;
    if(this->type == ALGOSENSOR_TYPE_1WIRE)
    {
        if(digitalRead(pin) == 0) 
        {
            this->type = ALGOSENSOR_TYPE_PWM;
            Serial.println("PWM device detected\n");
            return 0;
        }
        this->owi->readValue(0xbe,&value);
    }
    else
    {
        unsigned long pwmHighValue = pulseIn(pin, HIGH, PULSE_TIMEOUT);
        if(pwmHighValue != 0) 
        {
            dutyCycle = (float(pwmHighValue) / CYCLE_TIME) * 100.0f;
        } 
        else if (digitalRead(pin)) 
        {
            /* dutyCycle = 100; */
            this->type = ALGOSENSOR_TYPE_1WIRE;
            dutyCycle = 0;
            Serial.println("1Wire device detected\n");
        } 
        else 
        {
            dutyCycle = 0;
        }
        value = uint8_t(round(dutyCycle / 10));
    }
    /* Serial.print("Value:"); */
    /* Serial.println(value); */
    return value;
}


uint8_t Sensor(AlgoSensor & sensor)
{
    return sensor.getValue();
}
uint8_t Sensor(uint8_t port)
{
    switch(port)
    {
        case('1'):
        case(1):
        {
            return Sensor1.getValue();
            break;
        }
        case('2'):
        case(2):
        {
            return Sensor2.getValue();
            break;
        }
        default:
        {

        }
    }
}


void waitSensor(System name,int sensorPort, int signalValue)
{
	if(name.cthread.sequance != name.sequance)
	{
		return;
	}
	yield();
    uint8_t cvalue;
	switch(name.cthread.waitState)
	{
		case(ALGOTHREAD_WAIT_STATE_INIT):
		{
			Serial.print("Wait for sensor on line [");
			Serial.print(name.line);
			Serial.print("] for value [");
			Serial.print(signalValue);
			Serial.println("]");
			name.cthread.waitTimer = getSYSTIM();
			if(&name.cthread == &threadAlgoC)
			{
				while(1)
				{
					delay(10);
					yield();
					cvalue = Sensor(sensorPort);
					Serial.print("Value: ");
					Serial.println(cvalue);
					if(cvalue == signalValue)
					{
						break;
					}
					if(g_ALGOBOT_INFO.state != ALGOBOT_STATE_RUN)
					{
						name.cthread.waitState = ALGOTHREAD_WAIT_STATE_INIT;
						return;
					}
				}
				name.cthread.waitState = ALGOTHREAD_WAIT_STATE_INIT;
				name.cthread.sequance++;
				return;
			}
			else
			{
				// cthread.waitState = ALGOTHREAD_WAIT_STATE_RUN;
				return;
			}
			break;
		}
		case(ALGOTHREAD_WAIT_STATE_RUN):
		{
			cvalue = Sensor(sensorPort);
			if(cvalue != 0x00)
			{
				name.cthread.waitState = ALGOTHREAD_WAIT_STATE_INIT;
				name.cthread.sequance++;
				return;
			}
			return;
			break;
		}
	}
}

int waitSensor(System name,int sensorPort, int minSignalValue, int maxSignalValue)
{
	if(name.cthread.sequance != name.sequance)
	{
		return 0xff;
	}
	yield();
    uint8_t cvalue;
	switch(name.cthread.waitState)
	{
		case(ALGOTHREAD_WAIT_STATE_INIT):
		{
			Serial.print("Wait for sensor on line [");
			Serial.print(name.line);
			Serial.print("] for value in range [");
			Serial.print(minSignalValue);
			Serial.print("-");
			Serial.print(maxSignalValue);
			Serial.println("]");
			name.cthread.waitTimer = getSYSTIM();
			if(&name.cthread == &threadAlgoC)
			{
				while(1)
				{
					delay(10);
					yield();
					cvalue = Sensor(sensorPort);
					Serial.print("Current sensor value: ");
					Serial.println(cvalue);
					if((cvalue >= minSignalValue) && (cvalue <= maxSignalValue))
					{
						break;
					}
				}
				name.cthread.waitState = ALGOTHREAD_WAIT_STATE_INIT;
				name.cthread.sequance++;
				// return OP_STATUS_COMPLETED;
				return cvalue;
			}
			else
			{
				name.cthread.waitState = ALGOTHREAD_WAIT_STATE_RUN;
				// return OP_STATUS_RUNNING;
				return 0xff;
			}
			break;
		}
		case(ALGOTHREAD_WAIT_STATE_RUN):
		{
			cvalue = Sensor(sensorPort);
			if((cvalue >= minSignalValue) && (cvalue <= maxSignalValue))
			{
				name.cthread.waitState = ALGOTHREAD_WAIT_STATE_INIT;
				name.cthread.sequance++;
				// return OP_STATUS_COMPLETED;
				return cvalue;
			}
			return 0xff;
			break;
		}
	}
}

void waitSignal(System name,int sensorPort, bool logicState)
{
	if(name.cthread.sequance != name.sequance)
	{
		return;
	}
	yield();
    uint8_t cvalue;
	switch(name.cthread.waitState)
	{
		case(ALGOTHREAD_WAIT_STATE_INIT):
		{
			Serial.print("Wait for sensor on line [");
			Serial.print(name.line);
			Serial.print("] for value in range [");
			if(logicState)
			{
				Serial.print("True");
			}
			else
			{
				Serial.print("False");
			}
			Serial.println("]");
			name.cthread.waitTimer = getSYSTIM();
			if(&name.cthread == &threadAlgoC)
			{
				while(1)
				{
					delay(10);
					yield();
					cvalue = Sensor(sensorPort);
					Serial.print("Current value: ");
					Serial.println(cvalue);

					if((logicState == false) && cvalue == 0)
					{
						break;
					}
					else if((logicState == true) && cvalue != 0)
					{
						break;
					}
					if(g_ALGOBOT_INFO.state != ALGOBOT_STATE_RUN)
					{
						name.cthread.waitState = ALGOTHREAD_WAIT_STATE_INIT;
						return;
					}
				}
				name.cthread.waitState = ALGOTHREAD_WAIT_STATE_INIT;
				name.cthread.sequance++;
				return;
			}
			else
			{
				// cthread.waitState = ALGOTHREAD_WAIT_STATE_RUN;
				return;
			}
			break;
		}
		case(ALGOTHREAD_WAIT_STATE_RUN):
		{
			cvalue = Sensor(sensorPort);
			if(cvalue != 0x00)
			{
				name.cthread.waitState = ALGOTHREAD_WAIT_STATE_INIT;
				name.cthread.sequance++;
				return;
			}
			return;
			break;
		}
	}
}


int getSensor(System name,int sensorPort)
{
	// if(name.cthread.sequance != name.sequance)
	// {
	// 	return 0;
	// }
	yield();
	Serial.print("On line [");
	Serial.print(name.line);
	Serial.print("] current value from sensor on port [");
	Serial.print(sensorPort);
	Serial.print("] is [");
	int cvalue = Sensor(sensorPort);
	Serial.print(cvalue);
	Serial.println("]");
	// name.cthread.sequance++;
	return cvalue;
}


/* Private functions ******************************************************* */

/* ***************************** END OF FILE ******************************* */
