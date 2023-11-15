/* Includes **************************************************************** */
#include "algobot.h" 
#include "algothread.h"

/* Private types *********************************************************** */

/* Private constants ******************************************************* */

/* Private macros ********************************************************** */
/* Private variables ******************************************************* */
AlgoThread threads[THREAD_CNT];
AlgoThread & thread0 = threads[0];
AlgoThread & thread1 = threads[1];
AlgoThread & thread2 = threads[2];
AlgoThread & thread3 = threads[3];
AlgoThread & thread4 = threads[4];
AlgoThread & thread5 = threads[5];
AlgoThread & thread6 = threads[6];
AlgoThread & thread7 = threads[7];
AlgoThread & thread8 = threads[8];
AlgoThread & thread9 = threads[9];
AlgoThread & thread10 = threads[10];
AlgoThread & thread11 = threads[11];
AlgoThread & thread12 = threads[12];
AlgoThread & thread13 = threads[13];
AlgoThread & thread14 = threads[14];
AlgoThread & thread15 = threads[15];
// AlgoThread & thread16 = threads[16];
// AlgoThread & thread17 = threads[17];
// AlgoThread & thread18 = threads[18];
// AlgoThread & thread19 = threads[19];
// AlgoThread & thread20 = threads[20];
// AlgoThread & thread21 = threads[21];
// AlgoThread & thread22 = threads[22];
// AlgoThread & thread23 = threads[23];
// AlgoThread & thread24 = threads[24];
// AlgoThread & thread25 = threads[25];
// AlgoThread & thread26 = threads[26];
// AlgoThread & thread27 = threads[27];
// AlgoThread & thread28 = threads[28];
// AlgoThread & thread29 = threads[29];
// AlgoThread & thread30 = threads[30];
// AlgoThread & thread31 = threads[31];

AlgoThread threadAlgoC;
/* Private function prototypes ********************************************* */

/* Exported functions ****************************************************** */

void initTHREADS(void)
{

}
void chkTHREADS(void)
{
	return;
	thread0_run();
	thread1_run();
	thread2_run();
	thread3_run();
	thread4_run();
	thread5_run();
	thread6_run();
	thread7_run();
	thread8_run();
	thread9_run();
	thread10_run();
	thread11_run();
	thread12_run();
	thread13_run();
	thread14_run();
	thread15_run();
	thread16_run();
	thread17_run();
	thread18_run();
	thread19_run();
	thread20_run();
	thread21_run();
	thread22_run();
	thread23_run();
	thread24_run();
	thread25_run();
	thread26_run();
	thread27_run();
	thread28_run();
	thread29_run();
	thread30_run();
	thread31_run();
}

bool areAllThreadsCompleted(void)
{
	uint8_t k = 0;
	for( k = 0; k < THREAD_CNT; k++)
	{
		if(threads[k].state == ALGOTHREAD_STATE_RUNNING)
		{
			return false;
		}
	}
	return true;
}

void resetAllThreads(void)
{
	uint8_t k = 0;
	for( k = 0; k < THREAD_CNT; k++)
	{
		threads[k].state = ALGOTHREAD_STATE_IDLE;
		threads[k].sequance = 0;
		threads[k].sequanceCnt = 0;
		threads[k].cntStart = 0;
		threads[k].cntEnd = 0;
		threads[k].loopState = 0;
		threads[k].loopSequance = 0;
		threads[k].subThreadIdx = 0;
	}
	threadAlgoC.state = ALGOTHREAD_STATE_IDLE;
	threadAlgoC.sequance = 0;
	threadAlgoC.sequanceCnt = 0;
	threadAlgoC.cntStart = 0;
	threadAlgoC.cntEnd = 0;
	threadAlgoC.loopState = 0;
	threadAlgoC.loopSequance = 0;
	threadAlgoC.subThreadIdx = 0;
	Serial.println("All threads are reseted");
}


void resetThreadConfiguration(AlgoThread & thread)
{
	thread.state = 0x00;
	thread.status = ALGOTHREAD_STATUS_IDLE;
}

bool isThreadCompleted(AlgoThread & thread)
{
	if(thread.status == ALGOTHREAD_STATUS_COMPLETED)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool setThreadCompletedFlag(AlgoThread & thread)
{
	thread.status = ALGOTHREAD_STATUS_COMPLETED;
}

void thread0_run(void)
{ 

}

void thread1_run(void)
{ 

}

void thread2_run(void)
{ 

}

void thread3_run(void)
{ 

}
void thread4_run(void)
{ 

}
void thread5_run(void)
{ 

}
void thread6_run(void)
{ 

}
void thread7_run(void)
{ 

}
void thread8_run(void)
{ 

}
void thread9_run(void)
{ 

}
void thread10_run(void)
{ 

}
void thread11_run(void)
{ 

}
void thread12_run(void)
{ 

}
void thread13_run(void)
{ 

}
void thread14_run(void)
{ 

}
void thread15_run(void)
{ 

}
void thread16_run(void)
{ 

}
void thread17_run(void)
{ 

}
void thread18_run(void)
{ 

}
void thread19_run(void)
{ 

}
void thread20_run(void)
{ 

}
void thread21_run(void)
{ 

}
void thread22_run(void)
{ 

}
void thread23_run(void)
{ 

}
void thread24_run(void)
{ 

}
void thread25_run(void)
{ 

}
void thread26_run(void)
{ 

}
void thread27_run(void)
{ 

}
void thread28_run(void)
{ 

}
void thread29_run(void)
{ 

}
void thread30_run(void)
{ 

}
void thread31_run(void)
{ 

}

/* Private functions ******************************************************* */

/* ***************************** END OF FILE ******************************* */


