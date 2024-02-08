/**
* @file        algobutton.h
* @author      semir-t
* @date        Januar 2024
* @version     1.0.0
*/

/* Define to prevent recursive inclusion *********************************** */
#ifndef __ALGOBUTTON_H
#define __ALGOBUTTON_H
/* Includes **************************************************************** */
#include <Arduino.h>
#include <algothread.h>
#include <system.h>


/* Module configuration **************************************************** */

/* Exported constants ****************************************************** */

/* Exported macros ********************************************************* */

/* Exported types ********************************************************** */

/* Exported variables ****************************************************** */
extern uint8_t g_button_fuction_acitve_flag;

/* Exported functions ****************************************************** */

void playButtonReset(System name);
int waitPlayButtonPress(System name,int numberOfPresses);
bool getPlayButtonState(System name);

#endif 

