/* Includes **************************************************************** */
#include <algolight.h>
#include <algobot.h>

/* Private constants ******************************************************* */


/* Private macros ********************************************************** */

/* Private types *********************************************************** */

/* Private variables ******************************************************* */
AlgoLight Light1(LED_A_PIN,1);
AlgoLight Light2(LED_B_PIN,2);

const char c_color_name[7][8] = 
{
	"White",
	"Red",
	"Green",
	"Blue",
	"Purple",
	"Yellow",
	"Orange",
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






/* Private function prototypes ********************************************* */

/* Exported functions ****************************************************** */
AlgoLight::AlgoLight(uint8_t pin,uint8_t id)
{
    this->_pin = pin;
	this->id = id;
    neoPixelLed =NeoPixel(1, this->_pin, NEO_GRB + NEO_KHZ800);
    neoPixelLed.begin();
}

void AlgoLight::setColor(uint8_t r,uint8_t g,uint8_t b)
{
    neoPixelLed.setPixelColor(0, neoPixelLed.Color(r, g, b));
    neoPixelLed.show();
}

void AlgoLight::stop(void)
{
    this->state = ALGOLED_LIGHT_STATE_OFF;
    setColor(0x00,0x00,0x00);
}

uint8_t AlgoLight::run(uint32_t line,uint32_t sequance,AlgoThread & cthread,float time,uint8_t power,uint32_t color, uint8_t mode)
{
	if(cthread.sequance != sequance)
	{
		return 0;
	}
	yield();
	switch(this->status)
	{
		case (ALGOLED_LIGHT_STATUS_INIT):
		{
			Serial.print(F("Control the LED ["));
			Serial.print(this->id);
			Serial.print(F("] on line ["));
			Serial.print(line);
			Serial.print(F("]. Set color ["));
			Serial.print(color,HEX);
			Serial.print(F("] with power ["));
			Serial.print(power);
			Serial.print(F("] for period ["));
			Serial.print(time);
			Serial.println(F("] seconds"));
			this->period = time * 1000; // time in seconds
			this->timer = getSYSTIM();
			uint8_t r =  ((color >> 16) & 0xff) * ((float)power/LIGHT_POWER_LEVEL_CNT);
			uint8_t g =  ((color >> 8) & 0xff) * ((float)power/LIGHT_POWER_LEVEL_CNT);
			uint8_t b =  ((color) & 0xff) * ((float)power/LIGHT_POWER_LEVEL_CNT);
			this->setColor(r,g,b);
			if((mode == OP_STATUS_BLOCKING) && (this->period != FOREVER))
			{
				this->state = ALGOLED_LIGHT_STATE_ON;
				this->timer = getSYSTIM();
				this->status = ALGOLED_LIGHT_STATUS_RUNNING;
				if(&cthread == &threadAlgoC)
				{
					while(chk4TimeoutSYSTIM(this->timer,this->period) == SYSTIM_KEEP_ALIVE)
					{
						yield();
						if(g_ALGOBOT_INFO.state != ALGOBOT_STATE_RUN)
						{
							this->stop();
							this->status = ALGOLED_LIGHT_STATUS_INIT;
							return 	ALGOLED_LIGHT_STATUS_COMPLETED;
						}
					}
					Serial.println("Stop Light");
					this->stop();
					this->status = ALGOLED_LIGHT_STATUS_INIT;
					cthread.sequance++;
					return 	ALGOLED_LIGHT_STATUS_COMPLETED;
				}
				else
				{
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
				cthread.sequance++;
				return 	ALGOLED_LIGHT_STATUS_COMPLETED;
			}
			break;
		}
		case (ALGOLED_LIGHT_STATUS_RUNNING):
		{
			if(chk4TimeoutSYSTIM(this->timer,this->period) == SYSTIM_TIMEOUT)
			{
				Serial.println("Stop light");
				this->stop();
				this->status = ALGOLED_LIGHT_STATUS_INIT;
				cthread.sequance++;
				return 	ALGOLED_LIGHT_STATUS_COMPLETED;
			}
			return 	ALGOLED_LIGHT_STATUS_RUNNING;
			break;
		}
	}
}

uint8_t AlgoLight::runRaw(float time,uint8_t power,uint32_t color)
{

	Serial.print(F("Control the LED ["));
	Serial.print(this->id);
	Serial.print(F("]. Set color ["));
	Serial.print(color,HEX);
	Serial.print(F("] with power ["));
	Serial.print(power);
	Serial.print(F("] for period ["));
	Serial.print(time);
	Serial.println(F("] seconds"));
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
	Serial.println("Stop Light");
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
		Serial.print("Specified color [");
		Serial.print(color);
		Serial.print("] for the light on the line [");
		Serial.print(name.line);
		Serial.print("Specified color for the light on the line [");
		Serial.println("] is invalide");
	}
	uint32_t colorValue = c_color_value[k];
	switch(lightPort)
	{
		case(1):
		{
			Light1.run(name.line,name.sequance,name.cthread,seconds,power,colorValue,isBlocking);
			break;
		}
		case(2):
		{
			Light2.run(name.line,name.sequance,name.cthread,seconds,power,colorValue,isBlocking);
			break;
		}
	}
}


void light12(System name,float seconds,int power,char * color,bool isBlocking)
{
    if(isBlocking == true)
    {
		light(name,1,seconds,power,color,false);
		name.cthread.sequance--;
		light(name,2,seconds,power,color,true);
    }
    else
    {
		light(name,1,seconds,power,color,false);
		name.cthread.sequance--;
		light(name,2,seconds,power,color,false);
	}
}

void RGB12(System name,int lightPort, float seconds,int power,int R,int G,int B,bool isBlocking)
{
    uint32_t color = R;
    color = (color << 8) | G;
    color = (color << 8) | B;
	if(isBlocking == true)
    {
		Light1.run(name.line,name.sequance,name.cthread,seconds,power,color,false);
		name.cthread.sequance--;
		Light2.run(name.line,name.sequance,name.cthread,seconds,power,color,true);
    }
    else
    {
		Light1.run(name.line,name.sequance,name.cthread,seconds,power,color,false);
		name.cthread.sequance--;
		Light2.run(name.line,name.sequance,name.cthread,seconds,power,color,false);
	}
}

void stopLight(System name, int lightPort)
{
    switch(lightPort)
    {
        case(1):
		case('1'):
        {
            Light1.stop();
            break;
        }
        case(2):
		case('2'):
        {
            Light2.stop();
            break;
        }
        default:
        {

        }
    }

}

bool isLightBusy(System name, int lightPort )
{
	if(name.cthread.sequance != name.sequance)
	{
		return false;
	}

	switch(lightPort)
	{
		case(1):
		{
			name.cthread.sequance++;
			return (Light1.state == ALGOLED_LIGHT_STATE_OFF) ? false : true;
			break;
		}
		case(2):
		{
			name.cthread.sequance++;
			return (Light2.state == ALGOLED_LIGHT_STATE_OFF) ? false : true;
			break;
		}
		default:
		{
			name.cthread.sequance++;
		}
	}
	return false;
}


/* Private functions ******************************************************* */


/* ***************************** END OF FILE ******************************* */
