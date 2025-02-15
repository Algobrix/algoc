/* Includes **************************************************************** */
#include "system.h"

/* Private types *********************************************************** */

/* Private constants ******************************************************* */

/* Private macros ********************************************************** */

/* Private variables ******************************************************* */

/* Private function prototypes ********************************************* */

/* Exported functions ****************************************************** */
System::System(void):
	cthread(threadAlgoC)
{
    this->line = 0;
    this->sequance = 0;
}
System::System(uint32_t line,uint32_t sequance,AlgoThread & cthread):
	cthread(cthread)
{
	this->line = line;
	this->sequance = sequance;
}

/* Private functions ******************************************************* */

/* ***************************** END OF FILE ******************************* */


