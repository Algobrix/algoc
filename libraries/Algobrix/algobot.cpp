/* Includes **************************************************************** */
#include "algobot.h"
#include "compileTime.h"

/* Private constants ******************************************************* */


/* Private macros ********************************************************** */

/* Private types *********************************************************** */

/* Private variables ******************************************************* */
volatile ALGOBOT_INFO g_ALGOBOT_INFO;
const uint32_t c_current_build_time = __TIME_UNIX__;
uint8_t g_cmd;
uint8_t g_cmd_char_cnt;
char g_cmd_data[8];
uint8_t g_cmd_data_idx = 0;
float g_cmd_motor_power = 5;
uint8_t g_cmd_light_color = LIGHT_COLOR_WHITE;


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
    g_ALGOBOT_INFO.state = ALGOBOT_STATE_IDLE;
    while(0)
    {
        if(chk4TimeoutSYSTIM(timer,200) == SYSTIM_TIMEOUT)
        {
            timer = getSYSTIM();
            state ^= 0x01;
            digitalWrite(PLAY_LED_PIN,state);
        }
        if(digitalRead(PLAY_BUTTON_PIN) == 0)
        {
            digitalWrite(PLAY_LED_PIN,0);
            break;
        }
    }
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
	B.stop();
	C.stop();
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
					break;
				}
				case('s'):
				case('S'):
				{
					stopActuators();
					resetAllThreads();
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
				case('m'):
				case('M'):
				{
					g_cmd = 'M';
					break;
				}
				case('t'):
				case('T'):
				{
					g_cmd = 'T';
					break;
				}
				case('l'):
				case('L'):
				{
					g_cmd = 'L';
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
					case('M'):
					{
						g_cmd_data[g_cmd_data_idx++] = '\0';
						g_cmd_motor_power = atof(&g_cmd_data[0]);
						if(g_cmd_motor_power > 10)
						{
							g_cmd_motor_power = 10;
						}
						else if(g_cmd_motor_power < 0)
						{
							g_cmd_motor_power = 0;
						}
						g_cmd_data_idx = 0x00;
						g_cmd = 0x00;
						runFlag = 0;
						break;
					}
					case('T'):
					{
						g_cmd_data[g_cmd_data_idx++] = '\0';
						g_cmd_light_color = atof(&g_cmd_data[0]);
						if(g_cmd_light_color >= LIGHT_COLOR_CNT)
						{
							g_cmd_light_color = 0;
						}

						g_cmd_data_idx = 0x00;
						g_cmd = 0x00;
						runFlag = 0;
						break;
					}
					case('L'):
					{
						uint8_t port = g_cmd_data[0] - '0';
						g_cmd_data[g_cmd_data_idx++] = '\0';
						float period = atof(&g_cmd_data[1]);
						uint32_t colorValue = c_color_value[g_cmd_light_color];
						switch(port)
						{
							case(1):
							{
								Light1.runRaw(period,g_cmd_motor_power,colorValue);
								break;
							}
							case(2):
							{
								Light2.runRaw(period,g_cmd_motor_power,colorValue);
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

	if(!(PINC & (1 << PC0)))
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
					Serial.println("Power off");

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
	if((g_cmd == 0x00) && Serial.available())
	{
		char control = Serial.read();
		switch(control)
		{
			case('p'):
			case('P'):
			{
				break;
			}
			case('s'):
			case('S'):
			{
				resetAllThreads();
				stopALGOBOT();
				break;
			}

		}

	}

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
    if(MotorA.state == ALGOMOTOR_STATE_TIMED_ON)
    {
        if(chk4TimeoutSYSTIM(MotorA.timer,MotorA.period) == SYSTIM_TIMEOUT)
        {
            MotorA.stop();
        }
    }

    if(MotorA.state != ALGOMOTOR_STATE_IDLE)
	{
		if(chk4TimeoutSYSTIM(MotorA.speed_timer,200) == SYSTIM_TIMEOUT)
		{
			MotorA.speed_timer = getSYSTIM();
			if(MotorA.speed == 0)
			{
				MotorA.speed = MotorA.speed_cnt;
				MotorA.speed_cnt = 0;
			}
			else
			{
				if((MotorA.speed * (MotorA.speed_drop_threshold/100.)) > MotorA.speed_cnt)
				{
					MotorA.state = ALGOMOTOR_STATE_OFF;
				}
				MotorA.speed_cnt = 0;
			}
		}
	}

    if(MotorB.state == ALGOMOTOR_STATE_TIMED_ON)
    {
        if(chk4TimeoutSYSTIM(MotorB.timer,MotorB.period) == SYSTIM_TIMEOUT)
        {
            MotorB.stop();
        }
    }
    if(MotorB.state != ALGOMOTOR_STATE_IDLE)
	{
		if(chk4TimeoutSYSTIM(MotorB.speed_timer,200) == SYSTIM_TIMEOUT)
		{
			MotorB.speed_timer = getSYSTIM();
			if(MotorB.speed == 0)
			{
				MotorB.speed = MotorB.speed_cnt;
				MotorB.speed_cnt = 0;
			}
			else
			{
				if((MotorB.speed * (MotorB.speed_drop_threshold/100.)) > MotorB.speed_cnt)
				{
					MotorB.state = ALGOMOTOR_STATE_OFF;
				}
				MotorB.speed_cnt = 0;
			}
		}
	}

    if(MotorC.state == ALGOMOTOR_STATE_TIMED_ON)
    {
        if(chk4TimeoutSYSTIM(MotorC.timer,MotorC.period) == SYSTIM_TIMEOUT)
        {
            MotorC.stop();
        }
    }
	if(MotorC.state != ALGOMOTOR_STATE_IDLE)
	{
		if(chk4TimeoutSYSTIM(MotorC.speed_timer,200) == SYSTIM_TIMEOUT)
		{
			MotorC.speed_timer = getSYSTIM();
			if(MotorC.speed == 0)
			{
				MotorC.speed = MotorC.speed_cnt;
				MotorC.speed_cnt = 0;
			}
			else
			{
				if((MotorC.speed * (MotorC.speed_drop_threshold/100.)) > MotorC.speed_cnt)
				{
					MotorC.state = ALGOMOTOR_STATE_OFF;
				}
				MotorC.speed_cnt = 0;
			}
		}
	}

}

void initLIGHT(void)
{
    Light1.stop();
    Light2.stop();
}
void chkLIGHT(void)
{
    if(Light1.state == ALGOLED_LIGHT_STATE_TIMED_ON)
    {
        if(chk4TimeoutSYSTIM(Light1.timer,Light1.period) == SYSTIM_TIMEOUT)
        {
            Light1.stop();
        }
    }
    if(Light2.state == ALGOLED_LIGHT_STATE_TIMED_ON)
    {
        if(chk4TimeoutSYSTIM(Light2.timer,Light2.period) == SYSTIM_TIMEOUT)
        {
            Light2.stop();
        }
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
		// Serial.println(MotorA.rotCnt);
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
			if(0)
			{
				*MotorB.pOCR = 0;
				*MotorB.pTCNT = 0;
				*MotorB.pTIFR = 0;
			}
			else
			{
				*MotorB.pOCR = 1;
				*MotorB.pTCNT = 0;
				*MotorB.pTIFR = 0;
			}
			MotorB.stop();
		}
		else
		{
			MotorB.rotations -= *MotorB.pOCR;
		}
	}

	if(MotorB.rotationCounterFlag)
	{
		MotorB.rotCnt++;
		MotorB.speed_cnt++;
		if(MotorB.rotationCounterFloat != 0)
		{
			*MotorB.rotationCounterFloat = (float) MotorB.rotCnt / 360;
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
		// Serial.println(MotorB.rotCnt);
		MotorB.rotCnt++;
		MotorB.speed_cnt++;
		*MotorB.pTCNT = 0;
		*MotorB.pTIFR = 0;
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
	  }
	  else
	  {
		  MotorC.rotations -= *MotorB.pOCR;
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
