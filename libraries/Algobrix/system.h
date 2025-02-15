/**
* @file        system.h
* @author      semir-t
* @date        Juni 2023
* @version     1.0.0
*/

/* Define to prevent recursive inclusion *********************************** */
#ifndef __SYSTEM_H
#define __SYSTEM_H
/* Includes **************************************************************** */
#include <Arduino.h>
#include "algothread.h"

/* Module configuration **************************************************** */

/* Exported constants ****************************************************** */

/* Exported macros ********************************************************* */

#define ALGOC					System(__LINE__,cThread.sequanceCnt++,cThread)

/* Exported types ********************************************************** */
class System 
{
    private:

    public:
		uint32_t line;
		uint32_t sequance;
		AlgoThread & cthread;
        System(void); 
        System(uint32_t line,uint32_t sequance,AlgoThread & cthread); 
};

/* Exported variables ****************************************************** */

/* Exported functions ****************************************************** */

#endif 

