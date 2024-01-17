/* Includes **************************************************************** */
#include <Arduino.h>
#include "algomotor.h"
#include "algobot.h"
#include "systim.h"

/* Private constants ******************************************************* */


/* Private macros ********************************************************** */

/* Private types *********************************************************** */

/* Private variables ******************************************************* */

/* AlgoMotor MotorA(MOTOR_A_DIR, MOTOR_A_PWM); */
/* AlgoMotor MotorB(MOTOR_B_DIR, MOTOR_B_PWM); */
/* AlgoMotor MotorC(MOTOR_C_DIR, MOTOR_C_PWM); */


AlgoMotor MotorA(MOTOR_A_DIR, MOTOR_A_PWM,(uint16_t *) &TCNT3,(uint8_t *) &TIFR3,(uint16_t *)&OCR3A,'A');
AlgoMotor MotorB(MOTOR_B_DIR, MOTOR_B_PWM,(uint16_t *) &TCNT1,(uint8_t *) &TIFR1,(uint16_t *)&OCR1A,'B');
AlgoMotor MotorC(MOTOR_C_DIR, MOTOR_C_PWM,(uint16_t *) &TCNT4,(uint8_t *) &TIFR4,(uint16_t *)&OCR4A,'C');
extern uint32_t g_battery_voltage;

/* Private function prototypes ********************************************* */

/* Exported functions ****************************************************** */
AlgoMotor::AlgoMotor(uint8_t dirPin, uint8_t pwmPin,uint16_t * TCNT,uint8_t * TIFR,uint16_t * OCR,char id)
{
    _directionPin = dirPin;
    _pwmPin = pwmPin;
	this->id = id;
	this->state = ALGOMOTOR_STATE_IDLE;
	this->prevState = ALGOMOTOR_STATE_IDLE;
	this->speed_drop_threshold = 50;
	pinMode(_directionPin,OUTPUT);
	pinMode(_pwmPin,OUTPUT);
    pOCR = OCR;
    pTCNT = TCNT;
    pTIFR = TIFR;
}

uint8_t AlgoMotor::run(int line,int sequance,AlgoThread & cthread, float time,int power,int dir,int mode)
{
	while(g_ALGOBOT_INFO.state == ALGOBOT_STATE_PAUSE)
	{
		yield();
	}
	if(cthread.sequance != sequance)
	{
		return 0;
	}

	yield();
	switch(this->status)
	{
		case (ALGOMOTOR_STATUS_INIT):
		{
			if(dir == 1)
			{
				this->direction = 0;
			}
			else if(dir == -1)
			{
				this->direction = 1;
			}
			else
			{
				this->status = ALGOMOTOR_STATUS_INIT;
				Serial.print(F("Control the motor ["));
				Serial.print(id);
				Serial.print(F("] on line ["));
				Serial.print(line);
				Serial.print(F("] has a unsupported value ["));
				Serial.print(dir);
				Serial.println(F("]"));
				cthread.sequance++;
				return 	ALGOMOTOR_STATUS_COMPLETED;
			}
			Serial.print(F("Control the motor ["));
			Serial.print(id);
			Serial.print(F("] on line ["));
			Serial.print(line);
			Serial.print(F("]. Set direction ["));
			Serial.print(dir);
			Serial.print(F("] with power ["));
			Serial.print(power);
			Serial.print(F("] for period ["));
			Serial.print(time);
			Serial.println(F("]"));
			this->period = time * 1000;
			this->timer = getSYSTIM();
			this->rotCnt = 0;
			this->speed = 0;
			this->speed_cnt = 0;
			this->speed_timer = getSYSTIM();
			if(this->rotationCounterFlag == 0)
			{
				*pOCR = 10;
				*pTCNT = 0;
				*pTIFR = 0;
			}
			else
			{
				*pOCR = 1;
				*pTCNT = 0;
				*pTIFR = 0;
			}
			digitalWrite(_directionPin, this->direction);
			if(power > 10)
			{
				power = 10;
			}
			setPower(power);
			if((mode == OP_STATUS_BLOCKING))
			{
				this->prevState = this->state;
				this->state = ALGOMOTOR_STATE_ON;
				this->timer = getSYSTIM();
				if(&cthread == &threadAlgoC)
				{
					if(this->period != 0)
					{
						while(chk4TimeoutSYSTIM(this->timer,this->period) == SYSTIM_KEEP_ALIVE)
						{
							yield();
							if(g_ALGOBOT_INFO.state != ALGOBOT_STATE_RUN)
							{
								stopActuators();
								cthread.sequance++;
								this->status = ALGOMOTOR_STATUS_INIT;
								return 	ALGOMOTOR_STATUS_COMPLETED;
							}
							if(chk4TimeoutSYSTIM(this->speed_timer,200) == SYSTIM_TIMEOUT)
							{
								this->speed_timer = getSYSTIM();
								if(this->speed == 0)
								{
									this->speed = this->speed_cnt;
									this->speed_cnt = 0;
								}
								else
								{
									if((this->speed * (this->speed_drop_threshold/100.)) > this->speed_cnt)
									{
										break;
									}
									this->speed_cnt = 0;
								}
							}
						}

					}
					else
					{
						while(1)
						{
							yield();
							if(g_ALGOBOT_INFO.state != ALGOBOT_STATE_RUN)
							{
								stop();
								this->status = ALGOMOTOR_STATUS_INIT;
								return 	ALGOMOTOR_STATUS_COMPLETED;
							}
							if(this->state == ALGOMOTOR_STATE_OFF)
							{
								break;
							}
						}
					}
					stop();
					this->status = ALGOMOTOR_STATUS_INIT;
					cthread.sequance++;
					return 	ALGOMOTOR_STATUS_COMPLETED;
				}
				else
				{
					this->status = ALGOMOTOR_STATUS_RUNNING;
					return 	ALGOMOTOR_STATUS_RUNNING;
				}

			}
			else
			{
				if(this->period == FOREVER)
				{
					this->prevState = this->state;
					this->state = ALGOMOTOR_STATE_ON;
				}
				else
				{
					this->prevState = this->state;
					this->state = ALGOMOTOR_STATE_TIMED_ON;
				}
				cthread.sequance++;
				return 	ALGOMOTOR_STATUS_COMPLETED;
			}
			break;
		}
		case (ALGOMOTOR_STATUS_RUNNING):
		{
			if(chk4TimeoutSYSTIM(this->speed_timer,200) == SYSTIM_TIMEOUT)
			{
				this->speed_timer = getSYSTIM();
				if(this->speed == 0)
				{
					this->speed = this->speed_cnt;
					this->speed_cnt = 0;
				}
				else
				{
					if((this->speed * (this->speed_drop_threshold/100.)) > this->speed_cnt)
					{
						stop();
						this->status = ALGOMOTOR_STATUS_INIT;
						cthread.sequance++;
						return 	ALGOMOTOR_STATUS_COMPLETED;
					}
					this->speed_cnt = 0;
				}
			}

			if(chk4TimeoutSYSTIM(this->timer,this->period) == SYSTIM_TIMEOUT)
			{
				stop();
				this->status = ALGOMOTOR_STATUS_INIT;
				cthread.sequance++;
				return 	ALGOMOTOR_STATUS_COMPLETED;
			}
			return 	ALGOMOTOR_STATUS_RUNNING;
			break;
		}
	}
}

uint16_t AlgoMotor::getNumberOfRotations(void)
{
	return rotCnt;
}

uint8_t AlgoMotor::rotation(uint32_t line,uint32_t sequance,AlgoThread & cthread, float rotation,uint8_t power,int8_t dir,uint8_t mode)
{
	while(g_ALGOBOT_INFO.state == ALGOBOT_STATE_PAUSE)
	{
		yield();
	}

	if(cthread.sequance != sequance)
	{
		return 0;
	}

	yield();
	switch(this->status)
	{
		case (ALGOMOTOR_STATUS_INIT):
		{
			if(dir == 1)
			{
				this->direction = 0;
			}
			else if(dir == -1)
			{
				this->direction = 1;
			}
			else
			{
				this->status = ALGOMOTOR_STATUS_INIT;
				Serial.print(F("Control the motor ["));
				Serial.print(id);
				Serial.print(F("] on line ["));
				Serial.print(line);
				Serial.print(F("] has a unsupported value ["));
				Serial.print(dir);
				Serial.println(F("]"));
				cthread.sequance++;
				return 	ALGOMOTOR_STATUS_COMPLETED;
			}
			Serial.print("Control the motor [");
			Serial.print(id);
			Serial.print("] on line [");
			Serial.print(line);
			Serial.print("]. Set direction [");
			Serial.print(dir);
			Serial.print("] with power [");
			Serial.print(power);
			Serial.print("] for number of rotations [");
			Serial.print(rotation);
			Serial.println("]");
			this->period = FOREVER;
			this->timer = getSYSTIM();
			this->power = power;
			digitalWrite(_directionPin, this->direction);
			if(power > 10)
			{
				power = 10;
			}
			setPower(power);
			if(rotation != FOREVER)
			{
				setRotationCnt(rotation);
				if(mode == OP_STATUS_BLOCKING)
				{
					this->prevState = this->state;
					this->state = ALGOMOTOR_STATE_ROTATION;
					this->timer = getSYSTIM();
					if(&cthread == &threadAlgoC)
					{
						while(this->state == ALGOMOTOR_STATE_ROTATION)
						{
							yield();
							if(g_ALGOBOT_INFO.state != ALGOBOT_STATE_RUN)
							{
								stop();
								this->status = ALGOMOTOR_STATUS_INIT;
								return 	ALGOMOTOR_STATUS_COMPLETED;
							}
							if(chk4TimeoutSYSTIM(this->speed_timer,200) == SYSTIM_TIMEOUT)
							{
								this->speed_timer = getSYSTIM();
								if(this->speed == 0)
								{
									this->speed = this->speed_cnt;
									this->speed_cnt = 0;
								}
								else
								{
									if((this->speed * (this->speed_drop_threshold/100.)) > this->speed_cnt)
									{
										break;
									}
									this->speed_cnt = 0;
								}
							}

						}
						stop();
						this->status = ALGOMOTOR_STATUS_INIT;
						cthread.sequance++;
						return 	ALGOMOTOR_STATUS_COMPLETED;
					}
					else
					{
						this->status = ALGOMOTOR_STATUS_RUNNING;
						return 	ALGOMOTOR_STATUS_RUNNING;
					}

				}
				else
				{
					this->prevState = this->state;
					this->state = ALGOMOTOR_STATE_ROTATION;
					cthread.sequance++;
					return 	ALGOMOTOR_STATUS_COMPLETED;
				}

			}
			else
			{
				this->prevState = this->state;
				this->state = ALGOMOTOR_STATE_ON;
			}
			break;
		}
		case (ALGOMOTOR_STATUS_RUNNING):
		{
			if(chk4TimeoutSYSTIM(this->timer,this->period) == SYSTIM_TIMEOUT)
			{
				stop();
				this->status = ALGOMOTOR_STATUS_INIT;
				cthread.sequance++;
				return 	ALGOMOTOR_STATUS_COMPLETED;
			}
			return 	ALGOMOTOR_STATUS_RUNNING;
			break;
		}
	}
}

uint8_t AlgoMotor::rotationRaw(float rotation,uint8_t power,int8_t dir)
{
	Serial.print("Control the motor [");
	Serial.print(id);
	Serial.print("]. Set direction [");
	Serial.print(dir);
	Serial.print("] with power [");
	Serial.print(power);
	Serial.print("] for number of rotations [");
	Serial.print(rotation);
	Serial.println("]");
	this->direction = (dir == 1) ? 1 : 0;
	this->period = FOREVER;
	this->timer = getSYSTIM();
	setRotationCnt(rotation);

	digitalWrite(_directionPin, this->direction);
	if(power > 10)
	{
		power = 10;
	}
	setPower(power);
	this->prevState = this->state;
	this->state = ALGOMOTOR_STATE_ROTATION;
	this->timer = getSYSTIM();
	while(this->state == ALGOMOTOR_STATE_ROTATION)
	{
		yield();
		// if(g_ALGOBOT_INFO.state != ALGOBOT_STATE_RO)
		// {
		// 	Serial.println("Test2");
		// 	stop();
		// 	this->status = ALGOMOTOR_STATUS_INIT;
		// 	return 	ALGOMOTOR_STATUS_COMPLETED;
		// }
		if(chk4TimeoutSYSTIM(this->speed_timer,200) == SYSTIM_TIMEOUT)
		{
			this->speed_timer = getSYSTIM();
			if(this->speed == 0)
			{
				this->speed = this->speed_cnt;
				this->speed_cnt = 0;
			}
			else
			{
				if((this->speed * (this->speed_drop_threshold/100.)) > this->speed_cnt)
				{
					break;
				}
				this->speed_cnt = 0;
			}
		}

	}
	stop();
	this->status = ALGOMOTOR_STATUS_INIT;
	return 	ALGOMOTOR_STATUS_COMPLETED;
}

void AlgoMotor::changeSpeed(uint8_t pwm) 
{
    pwmValue = pwm;
    switch(_pwmPin) 
    {
        case (MOTOR_A_PWM): 
        {
            switch (pwmValue) 
            {
                case 0:
                case 255: 
                {
                    outputState = 0; 
                    digitalWrite(_pwmPin, pwmValue);
                    TIMSK2 = (TIMSK2 & B11111101) | B00000000; // Disable Compare A Interrupt
                    break;
                }
                default: 
                {
                    OCR2A = pwmValue;
                    TIMSK2 = (TIMSK2 & B11111101) | B00000010; // Enable Compare A Interrupt
                    break;
                }
            }
            break;
        }
        case (MOTOR_B_PWM): 
        {
            switch(pwmValue) 
            {
                case 0:
                case 255: 
                {
                    outputState = 0; 
                    digitalWrite(_pwmPin, pwmValue);
                    TIMSK2 = (TIMSK2 & B11111011) | B00000000; // Disable Compare B Interrupt
                    break;
                }
                default: 
                {
                    OCR2B = pwmValue;
                    TIMSK2 = (TIMSK2 & B11111011) | B00000100; // Enable Compare B Interrupt
                    break;
                }
            }
            break;
        }
        default: 
        {
            analogWrite(_pwmPin, pwmValue);
            break;
        }
    }
}

void AlgoMotor::stop()
{
	changeSpeed(0);
	// if(state == ALGOMOTOR_STATE_IDLE)
	// {
	// 	return;
	// }
	// else if(state == ALGOMOTOR_STATE_ROTATION)
	// {
	// }
	// else
	// {
	// 	changeSpeed(0);
	// }
	this->prevState = this->state;
	this->state = ALGOMOTOR_STATE_IDLE;
	this->status = ALGOMOTOR_STATUS_INIT;
	return;

}
void AlgoMotor::stop(int line,int sequance,AlgoThread & cthread)
{
	if(cthread.sequance != sequance)
	{
		return;
	}
	if(state == ALGOMOTOR_STATE_IDLE)
	{

		cthread.sequance++;
		return;
	}
	else if(state == ALGOMOTOR_STATE_ROTATION)
	{
		changeSpeed(0);
		if(this->rotationCounterFlag)	
		{
			*this->rotationCounterFloat = (float) *this->pOCR / 720;
		}
	}
	else
	{
		changeSpeed(0);
	}
	Serial.print(F("Stop motor: "));
	Serial.println (this->id);
	this->prevState = this->state;
	this->state = ALGOMOTOR_STATE_IDLE;
	this->status = ALGOMOTOR_STATUS_INIT;
	cthread.sequance++;
	return;
}
uint32_t AlgoMotor::getRuntime(void)
{
    return getElapsedTimeSYSTIM(timer);		
}

void AlgoMotor::setPower(uint32_t power)
{
	uint32_t value;
	uint32_t battery = g_battery_voltage;
	if(1)
	{
		if(battery > 9000)
		{
			battery = 9000;
		}
		uint8_t period = 255 * (0.8 + (9000-battery)/10000.);
		value = (power * period)/MOTOR_POWER_LEVEL_CNT;
	}
	else
	{
		value = (power * 255)/MOTOR_POWER_LEVEL_CNT;
	}
	Serial.print("Scale speed: ");
	Serial.println(value);
	Serial.print("Scale period: ");
	Serial.println(period);
	Serial.print("Battery: ");
	Serial.println(battery);
    changeSpeed(value);
}

void AlgoMotor::setRotationCnt(float rot)
{
	this->rotCnt = 0;
	this->speed = 0;
	this->speed_cnt = 0;
	if(rot < 1)
	{
		this->rotations = 0;
		if(this->rotationCounterFlag == 0)
		{
			int tmp = 0;
			if(rot < 0.2)
			{
				float power_coeff = 1.51;
				float rot_coeff = -8.59;
				float intercept_coeff = 1.87;
				tmp = (rot * 360) - ((power_coeff * this->power) + (rot_coeff * rot) - intercept_coeff);
			}
			else if(rot)
			{
				float power_coeff = 1.86;
				float rot_coeff = 1.27;
				float intercept_coeff = -0.71;
				tmp = (rot * 360) - ((power_coeff * this->power) + (rot_coeff * rot) - intercept_coeff);
			}
			if(tmp <= 0)
			{
				tmp = 1;
			}
			*pOCR = tmp;
			*pTCNT = 0;
			*pTIFR = 0;
		}
		else
		{
			*pOCR = rot * 360 * 2;
			*pTCNT = 0;
			*pTIFR = 0;
		}

	}
	else
	{
		this->rotations = (rot * 360);
		if(this->rotationCounterFlag == 0)
		{
			*pOCR = 1;
			*pTCNT = 0;
			*pTIFR = 0;
		}
		else
		{
			*pOCR = 1;
			*pTCNT = 0;
			*pTIFR = 0;
		}

	}
	this->speed_timer = getSYSTIM();
	}


void move(System name,char motorPort,float seconds,float power,int direction,bool isBlocking)
{

	switch(motorPort)
	{
		case('A'):
		{
			MotorA.run(name.line,name.sequance,name.cthread,seconds,power,direction,isBlocking);
			break;
		}
		case('B'):
		{
			MotorB.run(name.line,name.sequance,name.cthread,seconds,power,direction,isBlocking);
			break;
		}
		case('C'):
		{
			MotorC.run(name.line,name.sequance,name.cthread,seconds,power,direction,isBlocking);
			break;
		}

	}
}
			

int isMotorCompleted(AlgoMotor & motor)
{
	yield();
	return (motor.state == ALGOMOTOR_STATE_OFF) ? true : false;
}

void moveAB(System name,float seconds,float power,int direction,bool isBlocking)
{
    if(isBlocking == true)
    {
		MotorA.run(name.line,name.sequance,name.cthread,seconds,power,direction,false);
		name.cthread.sequance--;
		MotorB.run(name.line,name.sequance,name.cthread,seconds,power,direction,true);
    }
    else
    {
		MotorA.run(name.line,name.sequance,name.cthread,seconds,power,direction,false);
		name.cthread.sequance--;
		MotorB.run(name.line,name.sequance,name.cthread,seconds,power,direction,false);
    }
}

void moveABC(System name,float seconds,float power,int direction,bool isBlocking)
{
    if(isBlocking == true)
    {
		MotorA.run(name.line,name.sequance,name.cthread,seconds,power,direction,false);
		name.cthread.sequance--;
		MotorB.run(name.line,name.sequance,name.cthread,seconds,power,direction,false);
		name.cthread.sequance--;
		MotorC.run(name.line,name.sequance,name.cthread,seconds,power,direction,true);
    }
    else
    {
		MotorA.run(name.line,name.sequance,name.cthread,seconds,power,direction,false);
		name.cthread.sequance--;
		MotorB.run(name.line,name.sequance,name.cthread,seconds,power,direction,false);
		name.cthread.sequance--;
		MotorC.run(name.line,name.sequance,name.cthread,seconds,power,direction,false);
    }
}

void rotations(System name,char motorPort,float rotations,float power,int direction,bool isBlocking)
{
	switch(motorPort)
	{
		case('A'):
		{
			MotorA.rotation(name.line,name.sequance,name.cthread,rotations,power,direction,isBlocking);
			break;
		}
		case('B'):
		{
			MotorB.rotation(name.line,name.sequance,name.cthread,rotations,power,direction,isBlocking);
			break;
		}
		case('C'):
		{
			MotorC.rotation(name.line,name.sequance,name.cthread,rotations,power,direction,isBlocking);
			break;
		}
	}
}

void rotationsAB(System name,float rotations,float power,int direction,bool isBlocking)
{
	if(isBlocking == true)
	{
		MotorA.rotation(name.line,name.sequance,name.cthread,rotations,power,direction,false);
		name.cthread.sequance--;
		MotorB.rotation(name.line,name.sequance,name.cthread,rotations,power,direction,true);
	}
	else
	{
		MotorA.rotation(name.line,name.sequance,name.cthread,rotations,power,direction,false);
		name.cthread.sequance--;
		MotorB.rotation(name.line,name.sequance,name.cthread,rotations,power,direction,false);
	}
}

void rotationsABC(System name,float rotations,float power,int direction,bool isBlocking)
{
	if(isBlocking == true)
	{
		MotorA.rotation(name.line,name.sequance,name.cthread,rotations,power,direction,false);
		name.cthread.sequance--;
		MotorB.rotation(name.line,name.sequance,name.cthread,rotations,power,direction,false);
		name.cthread.sequance--;
		MotorC.rotation(name.line,name.sequance,name.cthread,rotations,power,direction,true);
	}
	else
	{
		MotorA.rotation(name.line,name.sequance,name.cthread,rotations,power,direction,false);
		name.cthread.sequance--;
		MotorB.rotation(name.line,name.sequance,name.cthread,rotations,power,direction,false);
		name.cthread.sequance--;
		MotorC.rotation(name.line,name.sequance,name.cthread,rotations,power,direction,false);
	}
}

void stopMotor(System name,char motorPort)
{
	switch(motorPort)
	{
		case('A'):
		{
			MotorA.stop(name.line,name.sequance,name.cthread);
			break;
		}
		case('B'):
		{
			MotorB.stop(name.line,name.sequance,name.cthread);
			break;
		}
		case('C'):
		{
			MotorC.stop(name.line,name.sequance,name.cthread);
			break;
		}
	}
}


float numberOfRotations(System name,AlgoMotor & motor)
{
	float cnt = motor.rotCnt;
	cnt = cnt / 720.;
	return cnt;
}


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


void stopCounting(System name, char motorPort)
{
	if(name.cthread.sequance != name.sequance)
	{
		return;
	}
	switch(motorPort)
	{
		case('A'):
		{
			if(MotorA.state == ALGOMOTOR_STATE_ROTATION)
			{
				MotorA.rotationCounterFlag = 0;
				if(*MotorA.pTCNT != 0)
				{
					if(MotorA.rotationCounterFloat != 0)
					{
						*MotorA.rotationCounterFloat = (float) *MotorA.pTCNT / 720;
					}
					if(MotorA.rotationCounterInt != 0)
					{
						*MotorA.rotationCounterInt = *MotorA.pTCNT / 720;
					}
				}
			}
			else
			{
				MotorA.rotationCounterFlag = 0;
				if(MotorA.rotationCounterFloat != 0)
				{
					*MotorA.rotationCounterFloat = (float) MotorA.rotCnt / 360;
				}
				if(MotorA.rotationCounterInt != 0)
				{
					*MotorA.rotationCounterInt = MotorA.rotCnt / 360;
				}
			}
			break;
		}
		case('B'):
		{
			if(MotorB.state == ALGOMOTOR_STATE_ROTATION)
			{
				MotorB.rotationCounterFlag = 0;
				if(*MotorB.pTCNT != 0)
				{
					if(MotorB.rotationCounterFloat != 0)
					{
						*MotorB.rotationCounterFloat = (float) *MotorB.pTCNT / 720;
					}
					if(MotorB.rotationCounterInt != 0)
					{
						*MotorB.rotationCounterInt = *MotorB.pTCNT / 720;
					}
				}

			}
			else
			{
				MotorB.rotationCounterFlag = 0;
				if(MotorB.rotationCounterFloat != 0)
				{
					*MotorB.rotationCounterFloat = (float) MotorB.rotCnt / 360;
				}
				if(MotorB.rotationCounterInt != 0)
				{
					*MotorB.rotationCounterInt =  MotorB.rotCnt / 360;
				}

			}
			break;
		}
		case('C'):
		{
			if(MotorC.state == ALGOMOTOR_STATE_ROTATION)
			{
				MotorC.rotationCounterFlag = 0;
				if(*MotorC.pTCNT != 0)
				{
					if(MotorC.rotationCounterFloat != 0)
					{
						*MotorC.rotationCounterFloat = (float) *MotorC.pTCNT / 720;
					}
					if(MotorC.rotationCounterInt != 0)
					{
						*MotorC.rotationCounterInt = *MotorC.pTCNT / 720;
					}

				}

			}
			else
			{
				MotorC.rotationCounterFlag = 0;
				if(MotorC.rotationCounterFloat != 0)
				{
					*MotorC.rotationCounterFloat = (float) MotorC.rotCnt / 360;
				}
				if(MotorC.rotationCounterInt != 0)
				{
					*MotorC.rotationCounterInt =  MotorC.rotCnt / 360;
				}

			}
			break;
		}
	}
	name.cthread.sequance++;
}

bool isMotorBusy(System name, char motorPort)
{
	if(name.cthread.sequance != name.sequance)
	{
		return false;
	}

	switch(motorPort)
	{
		case('A'):
		{
			name.cthread.sequance++;
			return (MotorA.state == ALGOMOTOR_STATE_IDLE) ? false : true;
			break;
		}
		case('B'):
		{
			name.cthread.sequance++;
			return (MotorB.state == ALGOMOTOR_STATE_IDLE) ? false : true;
			break;
		}
		case('C'):
		{
			name.cthread.sequance++;
			return (MotorC.state == ALGOMOTOR_STATE_IDLE) ? false : true;
			break;
		}
		default:
		{
			name.cthread.sequance++;
		}
	}

	return false;

}

void resistenceToStop(System name, char motorPort, float  threshold)
{
	if(name.cthread.sequance != name.sequance)
	{
		return;
	}

	threshold = threshold * 100;
	if(threshold > 100)
	{
		threshold = 100;
	}
	switch(motorPort)
	{
		case('A'):
		{
			MotorA.speed_drop_threshold = 100 - threshold;
			name.cthread.sequance++;
			break;
		}
		case('B'):
		{
			MotorB.speed_drop_threshold = 100 - threshold;
			name.cthread.sequance++;
			break;
		}
		case('C'):
		{
			MotorC.speed_drop_threshold = 100 - threshold;
			name.cthread.sequance++;
			break;
		}
		default:
		{

		}
	}
}

/* Private functions ******************************************************* */

/* ***************************** END OF FILE ******************************* */
