/* Define to prevent recursive inclusion *********************************** */
#ifndef __GOALGO_H
#define __GOALGO_H

/* Includes **************************************************************** */
#include <Arduino.h>
#include "systim.h"
#include <compileTime.h>
#include <algothread.h>
#include <algolight.h>
#include <algomotor.h>
#include <algosound.h>
#include <algowait.h>
#include <algosensor.h>
#include <algobreak.h>
#include <algothread.h>
#include <algoserial.h>
#include <algobutton.h>

/* Exported constants ****************************************************** */
#define DEBUG_ENABLED
#ifdef DEBUG_ENABLED
#define dprint(...)								Serial.print(__VA_ARGS__)
#define dprintln(...)							Serial.println(__VA_ARGS__)
#else
#define dprint(...)								
#define dprintln(...)							
#endif

#define repeat(system,condition)				system.cthread.sequance++;while((g_ALGOBOT_INFO.state != ALGOBOT_STATE_IDLE) && (g_ALGOBOT_INFO.state != ALGOBOT_STATE_HALT) && (condition))
#define isStopDetected(system)                   system.cthread.sequance++;if((g_ALGOBOT_INFO.state == ALGOBOT_STATE_IDLE) || (g_ALGOBOT_INFO.state == ALGOBOT_STATE_HALT)){break;}



/* Exported macros ********************************************************* */

/* Exported types ********************************************************** */

/* Exported variables ****************************************************** */
extern volatile uint32_t g_button_timer;
/* Exported functions ****************************************************** */
void initALGOBOT(void);
void stopALGOBOT(void);
int random(System name);
void blinkLed(void);
void stopActuators(void);
uint8_t isIdleALGOBOT(void);

#endif 
/* ***************************** END OF FILE ******************************* */

