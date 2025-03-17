/* Includes **************************************************************** */
#include <algolight.h>
#include <algoC.h>

/* Private constants ******************************************************* */


/* Private macros ********************************************************** */

/* Private types *********************************************************** */

/* Private variables ******************************************************* */
AlgoLight Light1(LED_A_PIN,1);
AlgoLight Light2(LED_B_PIN,2);

const char c_color_name[7][8] = 
{
	"white",
	"red",
	"green",
	"blue",
	"purple",
	"yellow",
	"orange",
};
const uint32_t c_color_value [LIGHT_COLOR_CNT] = 
{
    LIGHT_COLOR_WHITE,
    LIGHT_COLOR_RED,
    LIGHT_COLOR_GREEN,
    LIGHT_COLOR_BLUE,
    LIGHT_COLOR_PURPLE,
    LIGHT_COLOR_YELLOW,
    LIGHT_COLOR_ORANGE,
};

uint8_t g_light_dual_state = 0;
System g_light_dual_system;
AlgoThread g_light_dual_thread;


/* Private function prototypes ********************************************* */

/* Exported functions ****************************************************** */
AlgoLight::AlgoLight(uint8_t pin,uint8_t id):
	neoPixelLed(pin,1)
{
    this->_pin = pin;
	this->id = id;
}

void AlgoLight::setColor(uint8_t r,uint8_t g,uint8_t b)
{
	this->neoPixelLed.setColor(r,g,b);
	this->neoPixelLed.updateLeds();
}

void AlgoLight::stop(void)
{
    this->state = ALGOLED_LIGHT_STATE_OFF;
    setColor(0x00,0x00,0x00);
}

uint8_t AlgoLight::run(uint32_t line,uint32_t sequance,AlgoThread & cthread,float time,uint8_t power,uint32_t color, uint8_t mode)
{
	while(g_ALGOBOT_INFO.state == ALGOBOT_STATE_PAUSE)
	{
		yield();
	}

	if((cthread.sequance != sequance) && (this->status == ALGOLED_LIGHT_STATUS_INIT))
	{
		return 0;
	}

    if((this->status != ALGOLED_LIGHT_STATUS_INIT) && (this->running_thread == &threadAlgoC))
    {
        this->stop();
        this->status = ALGOLED_LIGHT_STATUS_INIT;
        this->running_thread = 0;
    }

	yield();
	switch(this->status)
	{
		case (ALGOLED_LIGHT_STATUS_INIT):
		{
#ifdef SERIAL_ENABLE
			Serial.print(F("Control the LED ["));
			Serial.print(this->id);
			Serial.print(F("] on line ["));
			Serial.print(line);
			Serial.print(F("]. Set color ["));
			Serial.print(color,HEX);
			Serial.print(F("] with power ["));
			Serial.print(power);
			Serial.print(F("] for period ["));
			if(time == 0)
			{
				Serial.print(F("FOREVER"));
			}
			else
			{
				Serial.print(time);
			}
			Serial.println(F("] seconds"));
#endif
			this->period = time * 1000; // time in seconds
			this->timer = getSYSTIM();
			uint8_t r =  ((color >> 16) & 0xff) * ((float)power/LIGHT_POWER_LEVEL_CNT);
			uint8_t g =  ((color >> 8) & 0xff) * ((float)power/LIGHT_POWER_LEVEL_CNT);
			uint8_t b =  ((color) & 0xff) * ((float)power/LIGHT_POWER_LEVEL_CNT);
			this->setColor(r,g,b);
			// if((mode == OP_STATUS_BLOCKING) && (this->period != FOREVER))
            this->mode = mode;
			if(mode == OP_STATUS_BLOCKING)
			{
				this->state = ALGOLED_LIGHT_STATE_ON;
				this->timer = getSYSTIM();
				if(&cthread == &threadAlgoC)
				{
					while((this->period == FOREVER) || (chk4TimeoutSYSTIM(this->timer,this->period) == SYSTIM_KEEP_ALIVE))
					{
						yield();
						if(g_ALGOBOT_INFO.state != ALGOBOT_STATE_RUN)
						{
							stopActuators();
							cthread.sequance++;
							this->status = ALGOLED_LIGHT_STATUS_INIT;
							return 	ALGOLED_LIGHT_STATUS_COMPLETED;
						}
					}
#ifdef SERIAL_ENABLE
					Serial.println("Stop Light");
#endif
					this->stop();
					this->status = ALGOLED_LIGHT_STATUS_INIT;
					cthread.sequance++;
					return 	ALGOLED_LIGHT_STATUS_COMPLETED;
				}
				else
				{
				    this->status = ALGOLED_LIGHT_STATUS_RUNNING;
                    this->running_thread = &cthread;
                    return 	ALGOLED_LIGHT_STATUS_RUNNING;
				}
			}
			else
			{
				if(this->period == FOREVER)
				{
                    this->state = ALGOLED_LIGHT_STATE_ON;
				}
				else
				{
					this->state = ALGOLED_LIGHT_STATE_TIMED_ON;
				}
                this->status = ALGOLED_LIGHT_STATUS_RUNNING;
                cthread.sequance++;
                this->running_thread = &cthread;
                return 	ALGOLED_LIGHT_STATUS_COMPLETED;
			}
			break;
		}
		case (ALGOLED_LIGHT_STATUS_RUNNING):
		{
			if(chk4TimeoutSYSTIM(this->timer,this->period) == SYSTIM_TIMEOUT)
			{
#ifdef SERIAL_ENABLE
				Serial.print("Stop light from run: ");
                Serial.println(this->id);
#endif
                if(this->mode == OP_STATUS_BLOCKING)
                {
                    cthread.sequance++;
                }
                this->stop();
				this->status = ALGOLED_LIGHT_STATUS_INIT;
                this->running_thread = 0;
				return 	ALGOLED_LIGHT_STATUS_COMPLETED;
			}
			return 	ALGOLED_LIGHT_STATUS_RUNNING;
			break;
		}
	}
}


void AlgoLight::loop(void)
{
//     if(this->status == ALGOLED_LIGHT_STATUS_RUNNING)
//     {
//         if(this->mode != OP_STATUS_BLOCKING)
//         {
//             if(chk4TimeoutSYSTIM(this->timer,this->period) == SYSTIM_TIMEOUT)
//             {
// #ifdef SERIAL_ENABLE
// 				Serial.print("Stop light: ");
//                 Serial.println(this->id);
// #endif
//                 this->status = ALGOLED_LIGHT_STATUS_INIT;
//                 this->stop();
//                 this->running_thread = 0;
//             }
//         }
//
//     }
//
}


uint8_t AlgoLight::runRaw(float time,uint8_t power,uint32_t color)
{

#ifdef SERIAL_ENABLE
	Serial.print(F("Control the LED ["));
	Serial.print(this->id);
	Serial.print(F("]. Set color ["));
	Serial.print(color,HEX);
	Serial.print(F("] with power ["));
	Serial.print(power);
	Serial.print(F("] for period ["));
	Serial.print(time);
	Serial.println(F("] seconds"));
#endif
	this->period = time * 1000; // time in seconds
	this->timer = getSYSTIM();
	uint8_t r =  ((color >> 16) & 0xff) * ((float)power/LIGHT_POWER_LEVEL_CNT);
	uint8_t g =  ((color >> 8) & 0xff) * ((float)power/LIGHT_POWER_LEVEL_CNT);
	uint8_t b =  ((color) & 0xff) * ((float)power/LIGHT_POWER_LEVEL_CNT);
	this->setColor(r,g,b);
	this->state = ALGOLED_LIGHT_STATE_ON;
	this->timer = getSYSTIM();
	this->status = ALGOLED_LIGHT_STATUS_RUNNING;
	while(chk4TimeoutSYSTIM(this->timer,this->period) == SYSTIM_KEEP_ALIVE)
	{
		yield();
		// if(g_ALGOBOT_INFO.state != ALGOBOT_STATE_RUN)
		// {
		// 	this->stop();
		// 	this->status = ALGOLED_LIGHT_STATUS_INIT;
		// 	return 	ALGOLED_LIGHT_STATUS_COMPLETED;
		// }
	}
#ifdef SERIAL_ENABLE
	Serial.println("Stop Light");
#endif
	this->stop();
	this->status = ALGOLED_LIGHT_STATUS_INIT;
	return 	ALGOLED_LIGHT_STATUS_COMPLETED;
}

int isLightCompleted(AlgoLight & light)
{
	yield();
	return (light.state == ALGOLED_LIGHT_STATE_OFF) ? true : false;
}

void light(System name,int lightPort, float seconds,int power,char * color,bool isBlocking)
{
	uint8_t k = 0;
	while( color[k] != '\0' )
	{
		if(color[k] >= 'A' && color[k] <='Z') 
		{
			color[k] = (color[k] - 'A') + 'a';
		}
		k++;
	}
	for( k = 0; k < 8; k++ )
	{
		uint8_t res = strcmp(color,c_color_name[k]);
		if(res == 0)
		{
			break;
		}
	}
	if(k == 8)
	{
#ifdef SERIAL_ENABLE
		Serial.print("Specified color [");
		Serial.print(color);
		Serial.print("] for the light on the line [");
		Serial.print(name.line);
		Serial.print("Specified color for the light on the line [");
		Serial.println("] is invalide");
#endif
	}
	uint32_t colorValue = c_color_value[k];
	switch(lightPort)
	{
		case(1):
		case('1'):
		{
			Light1.run(name.line,name.sequance,name.cthread,seconds,power,colorValue,isBlocking);
			break;
		}
		case(2):
		case('2'):
		{
			Light2.run(name.line,name.sequance,name.cthread,seconds,power,colorValue,isBlocking);
			break;
		}
	}
}


void light12(System name,float seconds,int power,char * color,bool isBlocking)
{
    if(&name.cthread == &threadAlgoC)
    {
        light(name,1,seconds,power,color,false);
        name.cthread.sequance--;
        light(name,2,seconds,power,color,isBlocking);
    }
    else
    {
        if((name.cthread.sequance != name.sequance) && (g_light_dual_state != 2))
        {
            return;
        }

        uint8_t k = 0;
        while( color[k] != '\0' )
        {
            if(color[k] >= 'A' && color[k] <='Z') 
            {
                color[k] = (color[k] - 'A') + 'a';
            }
            k++;
        }
        for( k = 0; k < 8; k++ )
        {
            uint8_t res = strcmp(color,c_color_name[k]);
            if(res == 0)
            {
                break;
            }
        }
        if(k == 8)
        {
#ifdef SERIAL_ENABLE
            Serial.print("Specified color [");
            Serial.print(color);
            Serial.print("] for the light on the line [");
            Serial.print(name.line);
            Serial.print("Specified color for the light on the line [");
            Serial.println("] is invalide");
#endif
        }
        uint32_t colorValue = c_color_value[k];

        switch(g_light_dual_state )
        {
            case(0):
            {
                g_light_dual_system.line = name.line;
                g_light_dual_system.sequance = 0;
                g_light_dual_thread.sequanceCnt = 2;
                g_light_dual_thread.sequance = 0;
                g_light_dual_system.cthread = g_light_dual_thread;
                g_light_dual_state = 1;
                break;
            }
            case(1):
            {
			    Light1.run(g_light_dual_system.line,0,g_light_dual_thread,seconds,power,colorValue,false);
			    Light2.run(g_light_dual_system.line,1,g_light_dual_thread,seconds,power,colorValue,isBlocking);
                if(isBlocking)
                {
                    if((Light1.status == ALGOLED_LIGHT_STATUS_INIT) && (Light2.status == ALGOLED_LIGHT_STATUS_INIT))
                    {
                        Serial.println("Completed");
                        g_light_dual_state = 0;
                        name.cthread.sequance++;
                    }
                }
                else
                {
                    Serial.println("Go to the state 2");
                    g_light_dual_state = 2;
                    name.cthread.sequance++;
                }
                break;
            }
            case(2):
            {
			    Light1.run(g_light_dual_system.line,0,g_light_dual_thread,seconds,power,colorValue,false);
			    Light2.run(g_light_dual_system.line,1,g_light_dual_thread,seconds,power,colorValue,isBlocking);
                if((Light1.status == ALGOLED_LIGHT_STATUS_INIT) && (Light2.status == ALGOLED_LIGHT_STATUS_INIT))
                {
                    Serial.print("Completed non-blocking");
                    g_light_dual_state = 0;
                }
                break;
            }

        }
    }
}

void RGB(System name,int lightPort, float seconds,int power,int R,int G,int B,bool isBlocking)
{
    uint32_t color = R;
    color = (color << 8) | G;
    color = (color << 8) | B;
	switch(lightPort)
	{
		case(1):
		case('1'):
		{
			Light1.run(name.line,name.sequance,name.cthread,seconds,power,color,isBlocking);
			break;
		}
		case(2):
		case('2'):
		{
			Light2.run(name.line,name.sequance,name.cthread,seconds,power,color,isBlocking);
			break;
		}
	}
}


void RGB12(System name,float seconds,int power,int R,int G,int B,bool isBlocking)
{
    uint32_t color = R;
    color = (color << 8) | G;
    color = (color << 8) | B;
	Light1.run(name.line,name.sequance,name.cthread,seconds,power,color,false);
	name.cthread.sequance--;
	Light2.run(name.line,name.sequance,name.cthread,seconds,power,color,isBlocking);
}

void stopLight(System name, int lightPort)
{
	if(name.cthread.sequance != name.sequance)
	{
		return;
	}
	yield();

    switch(lightPort)
    {
        case(1):
		case('1'):
        {
            Light1.stop(name.line,name.sequance,name.cthread);
            break;
        }
        case(2):
		case('2'):
        {
            Light2.stop(name.line,name.sequance,name.cthread);
            break;
        }
        default:
        {

        }
    }

}

void AlgoLight::stop(int line,int sequance,AlgoThread & cthread)
{	
    if(cthread.sequance != sequance)
	{
		return;
	}
    this->state = ALGOLED_LIGHT_STATE_OFF;
    setColor(0x00,0x00,0x00);
	cthread.sequance++;
    if(this->running_thread != 0)
    {
        this->running_thread->sequance++;
        this->running_thread = 0;
    }
	return;
}


bool isLightBusy(System name, int lightPort )
{
	if(name.cthread.sequance != name.sequance)
	{
		return false;
	}

	name.cthread.sequance++;
	switch(lightPort)
	{
		case(1):
		{
			return (Light1.state == ALGOLED_LIGHT_STATE_OFF) ? false : true;
			break;
		}
		case(2):
		{
			return (Light2.state == ALGOLED_LIGHT_STATE_OFF) ? false : true;
			break;
		}
		default:
		{
		}
	}
	return false;
}


/* Private functions ******************************************************* */


/* ***************************** END OF FILE ******************************* */
