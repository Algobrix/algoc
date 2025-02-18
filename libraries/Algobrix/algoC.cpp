/* Includes **************************************************************** */
#include "algoC.h"
#include "compileTime.h"

/* Private constants ******************************************************* */


/* Private macros ********************************************************** */

/* Private types *********************************************************** */

/* Private variables ******************************************************* */
volatile ALGOBOT_INFO g_ALGOBOT_INFO;
uint8_t g_cmd;
uint8_t g_cmd_char_cnt;
char g_cmd_data[8];
uint8_t g_cmd_data_idx = 0;
float g_cmd_motor_power = 5;


/* Private function prototypes ********************************************* */
static void initMOTOR(void);
static void initENCODER(void);
static void initUI(void);
static void initLIGHT(void);
static void chkMOTOR(void);
static void chkLIGHT(void);
/* static void chkMOTOR(void); */

/* Exported functions ****************************************************** */
void initALGOBOT(void)
{
    initMOTOR();
    initENCODER();
    initLIGHT();
    initUI();
	initTHREADS();

    uint32_t timer = getSYSTIM();
    uint8_t state = 0;
}

void chkALGOBOT(void)
{
	chkTHREADS();
    chkMOTOR();
    chkLIGHT();
}

void stopActuators(void)
{
	A.stop();
    A.status = ALGOMOTOR_STATUS_INIT;
    A.running_thread = 0;
	B.stop();
    B.status = ALGOMOTOR_STATUS_INIT;
    B.running_thread = 0;
	C.stop();
    C.status = ALGOMOTOR_STATUS_INIT;
    C.running_thread = 0;
	Light1.stop();
	Light2.stop();
	soundPlayer.stop();
}
void stopALGOBOT(void)
{
	g_ALGOBOT_INFO.state = ALGOBOT_STATE_IDLE;
	AlgoThread & cThread = threadAlgoC;
	stopActuators();
	resetAllThreads();
}

void wait4CompletionALGOBOT(void)
{
	while((A.status != ALGOMOTOR_STATUS_INIT) && (B.status != ALGOMOTOR_STATUS_INIT) && (C.status != ALGOMOTOR_STATUS_INIT))
	{

	}
}


// #ifdef SERIAL_ENABLE
void chk4CmdALGOBOT(void) 
{
	if(Serial.available())
	{
		if(g_cmd == 0x00 )
		{
			char data = Serial.read();
			switch(data)
			{
				case('p'):
				case('P'):
				{
					g_serial_control_flags |= SERIAL_CONTROL_PLAY;
					while(Serial.available())
					{
						char tmp = Serial.read();
					}
					break;
				}
				case('s'):
				case('S'):
				{
	                if(Serial.available())
                    {
			            char data = Serial.read();
                        if(data == '1')
                        {
                        	int cvalue = Sensor(1);
                            Serial.println(cvalue);
                        }
                        else if(data == '2')
                        {
                        	int cvalue = Sensor(2);
                            Serial.println(cvalue);
                        }
                        else
                        {
                            stopActuators();
                            resetAllThreads();
                            while(Serial.available())
                            {
                                char tmp = Serial.read();
                            }
                        }
                    }
                    else
                    {
                        stopActuators();
                        resetAllThreads();
                        while(Serial.available())
                        {
                            char tmp = Serial.read();
                        }
                    }
                    break;
                }
				case('a'):
				case('A'):
				{
					g_cmd = 'A';
					break;
				}
				case('b'):
				case('B'):
				{
					g_cmd = 'B';
					break;
				}
				case('c'):
				case('C'):
				{
					g_cmd = 'C';
					break;
				}

			}
		}
		else
		{
			uint8_t runFlag = 0;
			if(Serial.available())
			{
				char data = Serial.read();
				if(data == ' ')
				{
					runFlag = 1;
				}
				else if(data == '\r')
				{
					runFlag = 1;
				}
				else if(data == '\n')
				{
					runFlag = 1;
				}
				else
				{
					g_cmd_data[g_cmd_data_idx++] = data;
				}
			}
			else
			{
				runFlag = 1;
			}

			if(runFlag)
			{
				switch(g_cmd)
				{
					case('A'):
					case('B'):
					case('C'):
					{
						g_cmd_data[g_cmd_data_idx++] = '\0';
						float rot = atof(&g_cmd_data[0]);
						int8_t dir = CCW;
						if(rot < 0)
						{
							dir =  CW;
							rot = rot * (-1);
						}
						switch(g_cmd)
						{
							case('A'):
							{
								MotorA.rotationRaw(rot,g_cmd_motor_power,dir);
								break;
							}
							case('B'):
							{
								MotorB.rotationRaw(rot,g_cmd_motor_power,dir);
								break;
							}
							case('C'):
							{
								MotorC.rotationRaw(rot,g_cmd_motor_power,dir);
								break;
							}
						}
						g_cmd_data_idx = 0x00;
						g_cmd = 0x00;
						runFlag = 0;
						break;
					}
				}
			}
		}
	}
}
// #endif

void blinkLed(void) 
{
	if(chk4TimeoutSYSTIM(g_playTimer,200) == SYSTIM_TIMEOUT)
	{
		g_playTimer = getSYSTIM();
		g_playState ^= 0x01;
		pinMode(PLAY_LED_PIN,OUTPUT);
		digitalWrite(PLAY_LED_PIN,g_playState);
	}

}
uint8_t yield(void)
{
    if(g_ALGOBOT_INFO.state == ALGOBOT_STATE_HALT)
    {
        return 1;
    }
    if((g_ALGOBOT_INFO.state == ALGOBOT_STATE_RUN) || (g_ALGOBOT_INFO.state == ALGOBOT_STATE_PAUSE))
	{
		blinkLed();
		if(g_ALGOBOT_INFO.stopFlag)
		{
			g_ALGOBOT_INFO.stopFlag = 0;
			stopActuators();
		}
	}


	if((g_button_fuction_acitve_flag == 0) &&(!(PINC & (1 << PC0))))
	{
		digitalWrite(PLAY_LED_PIN,0);
		uint32_t timer = getSYSTIM();
		uint8_t flag = 0;
		while(!(PINC & (1 << PC0)))
		{
			if(g_ALGOBOT_INFO.state != ALGOBOT_STATE_POWER_OFF)
			{
				if(chk4TimeoutSYSTIM(timer,2000) == SYSTIM_TIMEOUT)
				{
					g_ALGOBOT_INFO.state = ALGOBOT_STATE_POWER_OFF;
					digitalWrite(PLAY_BUTTON_PIN,HIGH);
					pinMode(PLAY_BUTTON_PIN,LOW);
					digitalWrite(PLAY_LED_PIN,0);
					flag = 1;
				}
				else if(chk4TimeoutSYSTIM(timer,500) == SYSTIM_TIMEOUT)		
				{
					flag = 1;
					g_ALGOBOT_INFO.state = ALGOBOT_STATE_HALT;
					digitalWrite(PLAY_LED_PIN,1);
				}
			}
		}
		if((flag == 0) && (g_ALGOBOT_INFO.state != ALGOBOT_STATE_IDLE) && (g_ALGOBOT_INFO.state != ALGOBOT_STATE_POWER_OFF))
		{
			cli();
			// resetAllThreads();
			if(chk4TimeoutSYSTIM(g_button_timer,500) == SYSTIM_TIMEOUT)
			{
				g_ALGOBOT_INFO.state = ALGOBOT_STATE_RUN;
			}
			sei();
		}
		else
		{
			return 1;
		}
	}

	chkALGOBOT();
	chk4CmdALGOBOT();
	// if((g_cmd == 0x00) && Serial.available())
	// {
	// 	char control = Serial.read();
	// 	switch(control)
	// 	{
	// 		case('p'):
	// 		case('P'):
	// 		{
	// 			while(Serial.available())
	// 			{
	// 				char tmp = Serial.read();
	// 			}
	// 			break;
	// 		}
	// 		case('s'):
	// 		case('S'):
	// 		{
	// 			resetAllThreads();
	// 			stopALGOBOT();
	// 			while(Serial.available())
	// 			{
	// 				char tmp = Serial.read();
	// 			}
	// 			break;
	// 		}

	// 	}

	// }

	return 0;
}


int random(System name)
{
	if(name.cthread.sequance != name.sequance)
	{
		return false;
	}

	name.cthread.sequance++;
	int value = rand() % 11;
	return value;
}

/* Private functions ******************************************************* */
void initMOTOR(void)
{
  // Initialize TIMER2 for PWM of motors A and B.
  TCNT2 = 0;
  TIFR2 = 0;
  TCCR2A = 0;
  TCCR2B = (TCCR2B & B11111100) | B00000011;  // Clock I/O with prescaler of 64.
  OCR2A = 0;                                  // Compare Value A - for motor A
  OCR2B = 0;                                  // Compare Value B - for motor B
  TIMSK2 = (TIMSK2 & B11111111) | B00000000;  // Timer2 disabled by default - enabled when used for enabling the motors.
}

void initENCODER(void)
{

  pinMode(MOTOR_A_ENCODER, INPUT);
  pinMode(MOTOR_B_ENCODER, INPUT);
  pinMode(MOTOR_C_ENCODER, INPUT);
  // When Changing Bits -> Use mask so that only the significant bits are affected

  // MOTOR A - Initialize Timer3
  TCNT3 = 0;                                  // Timer/Counter Register. The actual timer value is stored here.
  TIFR3 = 0;                                  // Timer/Counter Interrupt Flag Register
  TCCR3A = 0;                                 // PWM Mode is OFF.
  TCCR3B = (TCCR3B & B11110001) | B00001110;  // External clock source on T3 pin. Clock on falling edge. + CTC = Clear Timer (on) Compare Mode.
  OCR3A = 0;                                  // The Compare value of Timer4 --> If TCNT4 == OCR3A --> Call Interrupt.
  TIMSK3 = (TIMSK3 & B11111101) | B00000010;  // Enable Timer3 compare interrupt - ALWAYS ENABLED.
  
  // MOTOR B - Initialize Timer1
  TCNT1 = 0;
  TIFR1 = 0;
  TCCR1A = 0;
  TCCR1B = (TCCR1B & B11110001) | B00001110;
  OCR1A = 0;
  TIMSK1 = (TIMSK1 & B11111101) | B00000010;
  
  // MOTOR C - Initialize Timer4
  TCNT4 = 0;
  TIFR4 = 0;
  TCCR4A = 0;
  TCCR4B = (TCCR4B & B11110001) | B00001110;
  OCR4A = 0;
  TIMSK4 = (TIMSK4 & B11111101) | B00000010;

}

void chkMOTOR(void)
{
	AlgoMotor * motor[3] = {&MotorA,&MotorB,&MotorC};
	uint8_t k = 0;
	for( k = 0; k < 3; k++ )
	{
		if(motor[k]->state == ALGOMOTOR_STATE_TIMED_ON)
		{
			if(chk4TimeoutSYSTIM(motor[k]->timer,motor[k]->period) == SYSTIM_TIMEOUT)
			{
				motor[k]->stop();
			}
		}

		if(motor[k]->state != ALGOMOTOR_STATE_IDLE)
		{
			if(chk4TimeoutSYSTIM(motor[k]->speed_timer,motor[k]->resistanceToStopPeriod) == SYSTIM_TIMEOUT)
			{
				motor[k]->speed_timer = getSYSTIM();
				if(motor[k]->speed == 0)
				{
					motor[k]->speed = motor[k]->speed_cnt;
					motor[k]->speed_cnt = 0;
				}
				else
				{
					if((motor[k]->speed * (motor[k]->speed_drop_threshold/100.)) > motor[k]->speed_cnt)
					{
						motor[k]->state = ALGOMOTOR_STATE_OFF;
						motor[k]->stop();
					}
					motor[k]->speed_cnt = 0;
				}
			}
		}
	}
}

uint8_t isIdleALGOBOT(void)
{
    uint8_t status = 0;
    status += MotorA.status;
    status += MotorB.status;
    status += MotorC.status;
    status += Light1.status;
    status += Light2.status;
    status += soundPlayer.status;
    return status;
}

void initLIGHT(void)
{
    Light1.stop();
    Light2.stop();
}
void chkLIGHT(void)
{
	AlgoLight * light[2] = {&Light1,&Light2};
	uint8_t k = 0;
	for( k = 0; k < 2; k++ )
	{
        light[k]->loop();
    }
}

void initUI(void)
{
	pinMode(PLAY_LED_PIN,OUTPUT);
    pinMode(PLAY_BUTTON_PIN,INPUT);
	/* pinMode(POWER_LED_PIN,OUTPUT); */
}

ISR(TIMER2_COMPA_vect) 
{
  noInterrupts();
  if(MotorA.outputState) 
  {
    digitalWrite(MOTOR_A_PWM, LOW);
    OCR2A = 0;
    MotorA.outputState = 0;
  } 
  else 
  {
    digitalWrite(MOTOR_A_PWM, HIGH);
    OCR2A = MotorA.pwmValue;
    MotorA.outputState = 1;
  }
  interrupts();
}

ISR(TIMER2_COMPB_vect) 
{
  noInterrupts();
  if(MotorB.outputState) 
  {
    digitalWrite(MOTOR_B_PWM, LOW);
    OCR2B = 0;
    MotorB.outputState = 0;
  } 
  else 
  {
    digitalWrite(MOTOR_B_PWM, HIGH);
    OCR2B = MotorB.pwmValue;
    MotorB.outputState = 1;
  }
  interrupts();
}

ISR(TIMER3_COMPA_vect) 
{
	noInterrupts();
	if(MotorA.state == ALGOMOTOR_STATE_ROTATION)
	{
		if(MotorA.rotations <= *MotorA.pOCR)
		{
			if(0)
			{
				*MotorA.pOCR = 0;
				*MotorA.pTCNT = 0;
				*MotorA.pTIFR = 0;
			}
			else
			{
				*MotorA.pOCR = 1;
				*MotorA.pTCNT = 0;
				*MotorA.pTIFR = 0;
			}
			MotorA.stop();
            if(MotorA.mode == OP_STATUS_BLOCKING)
            {
                MotorA.running_thread->sequance++;
            }
            MotorA.running_thread = 0;
            MotorA.status = ALGOMOTOR_STATUS_INIT;
        }
		else
		{
			MotorA.rotations -= *MotorA.pOCR;
		}
	}

	if(MotorA.rotationCounterFlag)
	{
		MotorA.rotCnt++;
		MotorA.speed_cnt++;
		if(MotorA.rotationCounterFloat != 0)
		{
			*MotorA.rotationCounterFloat = (float) MotorA.rotCnt / 360;
		}
		if(MotorA.rotationCounterInt != 0)
		{
			*MotorA.rotationCounterInt = MotorA.rotCnt / 360.;
		}
		*MotorA.pTCNT = 0;
		*MotorA.pTIFR = 0;
	}
	else
	{
		MotorA.rotCnt++;
		MotorA.speed_cnt++;
		*MotorA.pTCNT = 0;
		*MotorA.pTIFR = 0;
	}

	interrupts();
}
ISR(TIMER1_COMPA_vect) 
{
 	noInterrupts();
	if(MotorB.state == ALGOMOTOR_STATE_ROTATION)
	{
		if(MotorB.rotations <= *MotorB.pOCR)
		{
            *MotorB.pOCR = 1;
            *MotorB.pTCNT = 0;
            *MotorB.pTIFR = 0;
            MotorB.stop();
            if(MotorB.mode == OP_STATUS_BLOCKING)
            {
                MotorB.running_thread->sequance++;
            }
            MotorB.running_thread = 0;
            MotorB.status = ALGOMOTOR_STATUS_INIT;
		}
		else
		{
			MotorB.rotations -= *MotorB.pOCR;
		}
        MotorB.rotCnt++;
        MotorB.speed_cnt++;
        if(MotorB.rotationCounterFlag)
        {
            if(MotorB.rotationCounterFloat != 0)
            {
                *MotorB.rotationCounterFloat = (float) MotorB.rotCnt / 360.;
            }
            if(MotorB.rotationCounterInt != 0)
            {
                *MotorB.rotationCounterInt = MotorB.rotCnt / 360.;
            }
        }
	}
    else
    {
        MotorB.rotCnt++;
        MotorB.speed_cnt++;
        if(MotorB.rotationCounterFlag)
        {
            if(MotorB.rotationCounterFloat != 0)
            {
                *MotorB.rotationCounterFloat = (float) MotorB.rotCnt / 360.;
            }
            if(MotorB.rotationCounterInt != 0)
            {
                *MotorB.rotationCounterInt = MotorB.rotCnt / 360.;
            }
            *MotorB.pTCNT = 0;
            *MotorB.pTIFR = 0;
        }
        else
        {
            *MotorB.pTCNT = 0;
            *MotorB.pTIFR = 0;
        }

}

	interrupts();
}


ISR(TIMER4_COMPA_vect) 
{
  noInterrupts();
  if(MotorC.state == ALGOMOTOR_STATE_ROTATION)
  {
      if(MotorC.rotations <= *MotorC.pOCR)
	  {
		  *MotorC.pOCR = 1;
		  *MotorC.pTCNT = 0;
		  *MotorC.pTIFR = 0;
		  MotorC.stop();
          if(MotorC.mode == OP_STATUS_BLOCKING)
          {
              MotorC.running_thread->sequance++;
          }
          MotorC.running_thread = 0;
          MotorC.status = ALGOMOTOR_STATUS_INIT;
      }
	  else
	  {
		  MotorC.rotations -= *MotorC.pOCR;
	  }
  }

  if(MotorC.rotationCounterFlag)
  {
	  MotorC.rotCnt++;
	  MotorC.speed_cnt++;
	  if(MotorC.rotationCounterFloat != 0)
	  {
		  *MotorC.rotationCounterFloat = (float) MotorC.rotCnt / 360;
	  }
	  if(MotorC.rotationCounterInt != 0)
	  {
		  *MotorC.rotationCounterInt = MotorC.rotCnt / 360;
	  }
	  *MotorC.pTCNT = 0;
	  *MotorC.pTIFR = 0;
  }
  else
  {
	  MotorC.speed_cnt++;
	  *MotorC.pTCNT = 0;
	  *MotorC.pTIFR = 0;
  }
  interrupts();
}


/* ***************************** END OF FILE ******************************* */
