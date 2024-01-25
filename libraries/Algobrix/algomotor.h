/* Define to prevent recursive inclusion *********************************** */
#ifndef __ALGOMOTOR_H
#define __ALGOMOTOR_H

/* Includes **************************************************************** */
#include <Arduino.h>
#include <algothread.h>
#include <system.h>

/* Exported constants ****************************************************** */
#define NUM_OF_MOTORS                                   3
#define MOTOR_MAX_POWER_LEVEL                           3
#define MOTOR_POWER_LEVEL_CNT                           10


#define A                                               MotorA
#define B                                               MotorB
#define C                                               MotorC


enum MOTOR_DIRECTION
{
    CW = 1,
	cw = 1,
    CCW = -1,
    ccw = -1,
};

enum ALGOMOTOR_STATE
{
    ALGOMOTOR_STATE_IDLE = 0x00,
    ALGOMOTOR_STATE_ON,
    ALGOMOTOR_STATE_OFF,
    ALGOMOTOR_STATE_TIMED_ON,
	ALGOMOTOR_STATE_ROTATION,
};

enum ALGOMOTOR_STATUS
{
	ALGOMOTOR_STATUS_INIT = 0x00,
	ALGOMOTOR_STATUS_RUNNING = 0x01,
	ALGOMOTOR_STATUS_COMPLETED = 0x00
};

enum ALGOMOTOR_PORT_ID
{
    MOTOR_A = 0x00,
    MOTOR_B,
    MOTOR_C,
};

/* Exported macros ********************************************************* */

/* Exported types ********************************************************** */
class AlgoMotor 
{
    private:
        uint8_t _directionPin;
        uint8_t _pwmPin;
    public:
		char id;
        uint8_t state;
		uint8_t prevState;
		uint8_t status;
        uint8_t outputState;
        uint8_t pwmValue;
		uint8_t power;
        uint8_t direction;
		uint16_t rotCnt;
		uint16_t rotations;
		float * rotationCounterFloat;
		int * rotationCounterInt;
		uint8_t rotationCounterFlag;
        uint32_t timer;
        uint32_t period;
        uint16_t * pOCR;
        uint16_t * pTCNT;
        uint8_t * pTIFR;

		uint32_t speed_timer;
		uint16_t speed_cnt;
		uint16_t speed;
		uint8_t speed_drop_threshold;

		


        AlgoMotor(uint8_t dirPin, uint8_t pwmPin,uint16_t * TCNT,uint8_t * TIFR,uint16_t * OCR,char id); 
        uint8_t run(int line,int sequance,AlgoThread & cthread, float time,int power,int dir,int mode);
		uint8_t rotation(uint32_t line,uint32_t sequance,AlgoThread & cthread, float rotation,uint8_t power,int8_t dir,uint8_t mode);
        void changeSpeed(uint8_t pwm);
        void setPower(uint32_t power);
        void setRotationCnt(float rot);
		uint16_t getNumberOfRotations(void);
		void stop();
		void stop(int line,int sequance,AlgoThread & cthread);
        uint32_t getRuntime(void);
		uint8_t rotationRaw(float rotation,uint8_t power,int8_t dir);
};
/* Exported variables ****************************************************** */
extern AlgoMotor MotorA;
extern AlgoMotor MotorB;
extern AlgoMotor MotorC;

/* Exported functions ****************************************************** */
int isMotorCompleted(AlgoMotor & motor);

void move(System name,char motorPort,float seconds,float power,int direction,bool isBlocking);
void moveAB(System name,float seconds,float power,int direction,bool isBlocking);
void moveABC(System name,float seconds,float power,int direction,bool isBlocking);

void rotations(System name,char motorPort,float rotations,float power,int direction,bool isBlocking);
void rotationsAB(System name,float rotations,float power,int direction,bool isBlocking);
void rotationsABC(System name,float rotations,float power,int direction,bool isBlocking);

void stopMotor(System name,char motorPort);

void stopCounting(System name, char motorPort);

bool isMotorBusy(System name, char motorPort);

void resistanceToStop(System name, char motorPort, float  threshold);


// Specializations for known types
template <typename T>
struct TypeName {
    static int numberIsInt(T number) { return 1; }
};
template <>
struct TypeName<int> {
    static int numberIsInt(int number) { return 1; }
};

template <>
struct TypeName<float> {
    static int numberIsInt(float number) { return 0; }
};

template <typename Number> void startCounting(System name,char motorPort, Number & parameter)
{
	if(TypeName<Number>::numberIsInt(parameter))
	{
		Serial.println("Parameter is int");
	}
	else
	{
		Serial.println("Parameter is float");
	}
	if(name.cthread.sequance != name.sequance)
	{
		return;
	}

	switch(motorPort)
	{
		case('A'):
		{
			MotorA.rotCnt = 0;
			if(TypeName<Number>::numberIsInt(parameter))
			{
				MotorA.rotationCounterInt = (int *) &parameter;
				MotorA.rotationCounterFloat = 0;
			}
			else
			{
				MotorA.rotationCounterFloat = (float *) &parameter;
				MotorA.rotationCounterInt = 0;
			}
			MotorA.rotationCounterFlag = 1;
			*MotorA.pOCR = 1;
			*MotorA.pTCNT = 0;
			*MotorA.pTIFR = 0;

			break;
		}
		case('B'):
		{
			MotorB.rotCnt = 0;
			if(TypeName<Number>::numberIsInt(parameter))
			{
				MotorB.rotationCounterInt = (int *) &parameter;
				MotorB.rotationCounterFloat = 0;
			}
			else
			{
				MotorB.rotationCounterFloat = (float *) &parameter;
				MotorB.rotationCounterInt = 0;
			}
			MotorB.rotationCounterFlag = 1;

			*MotorB.pOCR = 1;
			*MotorB.pTCNT = 0;
			*MotorB.pTIFR = 0;
			break;
		}
		case('C'):
		{
			MotorC.rotCnt = 0;
			if(TypeName<Number>::numberIsInt(parameter))
			{
				MotorC.rotationCounterInt = (int *) &parameter;
				MotorC.rotationCounterFloat = 0;
			}
			else
			{
				MotorC.rotationCounterFloat = (float *) &parameter;
				MotorC.rotationCounterInt = 0;
			}
			MotorC.rotationCounterFlag = 1;
			*MotorC.pOCR = 1;
			*MotorC.pTCNT = 0;
			*MotorC.pTIFR = 0;
			break;
		}
	}
	name.cthread.sequance++;

}

#ifdef TEST
void startCounting(System name, char motorPort, float  & rotationCounter);
void startCounting(System name, char motorPort, int & rotationCounter);
void startCounting(System name, char motorPort, float & rotationCounter)
{
	if(name.cthread.sequance != name.sequance)
	{
		return;
	}

	switch(motorPort)
	{
		case('A'):
		{
			MotorA.rotCnt = 0;
			MotorA.rotationCounterFloat = &rotationCounter;
			MotorA.rotationCounterInt = 0;
			MotorA.rotationCounterFlag = 1;
			*MotorA.pOCR = 1;
			*MotorA.pTCNT = 0;
			*MotorA.pTIFR = 0;

			break;
		}
		case('B'):
		{
			MotorB.rotCnt = 0;
			MotorB.rotationCounterFloat = &rotationCounter;
			MotorB.rotationCounterInt = 0;
			MotorB.rotationCounterFlag = 1;
			*MotorB.pOCR = 1;
			*MotorB.pTCNT = 0;
			*MotorB.pTIFR = 0;
			break;
		}
		case('C'):
		{
			MotorC.rotCnt = 0;
			MotorC.rotationCounterFloat = &rotationCounter;
			MotorC.rotationCounterInt = 0;
			MotorC.rotationCounterFlag = 1;
			*MotorC.pOCR = 1;
			*MotorC.pTCNT = 0;
			*MotorC.pTIFR = 0;
			break;
		}
	}
	name.cthread.sequance++;
}

void startCounting(System name, char motorPort, int & rotationCounter)
{
	if(name.cthread.sequance != name.sequance)
	{
		return;
	}

	switch(motorPort)
	{
		case('A'):
		{
			MotorA.rotCnt = 0;
			MotorA.rotationCounterInt = &rotationCounter;
			MotorA.rotationCounterFloat = 0;
			MotorA.rotationCounterFlag = 1;
			*MotorA.pOCR = 2;
			*MotorA.pTCNT = 0;
			*MotorA.pTIFR = 0;

			break;
		}
		case('B'):
		{
			MotorB.rotCnt = 0;
			MotorB.rotationCounterInt = &rotationCounter;
			MotorB.rotationCounterFloat = 0;
			MotorB.rotationCounterFlag = 1;
			*MotorB.pOCR = 2;
			*MotorB.pTCNT = 0;
			*MotorB.pTIFR = 0;
			break;
		}
		case('C'):
		{
			MotorC.rotCnt = 0;
			MotorC.rotationCounterInt = &rotationCounter;
			MotorC.rotationCounterFloat = 0;
			MotorC.rotationCounterFlag = 1;
			*MotorC.pOCR = 2;
			*MotorC.pTCNT = 0;
			*MotorC.pTIFR = 0;
			break;
		}
	}
	name.cthread.sequance++;
}
#endif



#endif 
/* ***************************** END OF FILE ******************************* */

