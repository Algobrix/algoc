/* Define to prevent recursive inclusion *********************************** */
#ifndef __ALGOLED_LIGHT_H
#define __ALGOLED_LIGHT_H

/* Includes **************************************************************** */
#include <Arduino.h>
#include <system.h>
// #include <neopixel.h>
#include <TinyWS2812B.h>
#include <algothread.h>
#include <algoC.h>

/* Exported constants ****************************************************** */
#define LIGHT_POWER_LEVEL_CNT                           10

enum ALGOLED_LIGHT_STATE
{
	ALGOLED_LIGHT_STATE_OFF = 0x00,
	ALGOLED_LIGHT_STATE_ON,
    ALGOLED_LIGHT_STATE_TIMED_ON,
};

enum ALGOLED_THREAD_STATE
{
	ALGOLED_THREAD_STATE_INIT = 0x00,
	ALGOLED_THREAD_STATE_RUN,
};

enum ALGOLED_LIGHT_COLOR
{
    LIGHT_COLOR_WHITE = 0xffffff,
    LIGHT_COLOR_RED = 0xff0000,
    LIGHT_COLOR_GREEN = 0x00ff00,
    LIGHT_COLOR_BLUE = 0x0000ff,
    LIGHT_COLOR_PURPLE = 0xff00ff,
    LIGHT_COLOR_YELLOW = 0xFF6F00,
    LIGHT_COLOR_ORANGE = 0xFF4F00,
	LIGHT_COLOR_CNT = 7,
};



enum ALGOLED_LIGHT_STATUS
{
	ALGOLED_LIGHT_STATUS_INIT = 0x00,
	ALGOLED_LIGHT_STATUS_RUNNING = 0x01,
	ALGOLED_LIGHT_STATUS_COMPLETED = 0x00
};
/* Exported macros ********************************************************* */

/* Exported types ********************************************************** */
class AlgoLight
{
    private:
        // NeoPixel neoPixelLed;
		TinyWS2812B neoPixelLed;
        uint8_t _pin;
    public:
		uint8_t id;
        uint8_t state;
        float period;
        uint32_t timer;
		uint8_t threadState;
		uint8_t status;
        uint8_t mode;
		AlgoLight(uint8_t pin,uint8_t id);
		AlgoThread * running_thread;

        void setColor(uint8_t r,uint8_t g,uint8_t b);
        void stop(void);
		void stop(int line,int sequance,AlgoThread & cthread);
        uint8_t run(uint32_t line,uint32_t sequance,AlgoThread & cthread,float time,uint8_t power,uint32_t color, uint8_t mode);
		uint8_t runRaw(float time,uint8_t power,uint32_t color);

        void loop(void);
};

/* Exported variables ****************************************************** */
extern AlgoLight Light1;
extern AlgoLight Light2;
extern const uint32_t c_color_value [LIGHT_COLOR_CNT];


/* Exported functions ****************************************************** */
void light(System Name,int lightPort, float seconds,int power,char * color,bool isBlocking);

int isLightCompleted(AlgoLight & light);

void light12(System name,float seconds,int power,char * color,bool isBlocking);
void RGB(System name,int lightPort, float seconds,int power,int R,int G,int B,bool isBlocking);
void RGB12(System name,float seconds,int power,int R,int G,int B,bool isBlocking);
void stopLight(System name, int lightPort);
bool isLightBusy(System name, int lightPort);
#endif 
/* ***************************** END OF FILE ******************************* */


