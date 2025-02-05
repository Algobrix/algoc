/**
* @file        algothread.h
* @author      semir-t
* @date        Maj 2023
* @version     1.0.0
*/

/* Define to prevent recursive inclusion *********************************** */
#ifndef __ALGOTHREAD_H
#define __ALGOTHREAD_H
/* Includes **************************************************************** */
#include <Arduino.h>
#include <compileTime.h>

/* Module configuration **************************************************** */
#define THREAD_CNT				16	
#define ALGOC_APP				AlgoThread & cThread
#define ALGOC_FUNCTION			cThread

#define START_THREAD(thread_id)				\
											AlgoThread & cThread = thread_id;\
											if(cThread.state == ALGOTHREAD_STATE_IDLE)\
											{\
												cThread.state = ALGOTHREAD_STATE_RUNNING;\
                                            }\
											cThread.sequanceCnt = 0; \
											

#define END_THREAD(thread_id)				{\
												if((thread_id.state != ALGOTHREAD_STATE_COMPLETED) && (thread_id.sequance >= cThread.sequanceCnt) && (thread_id.subThreadIdx == thread_id.subThreadCompleted))\
												{\
													thread_id.state = ALGOTHREAD_STATE_COMPLETED;\
													{\
                                                        for(uint8_t k = 0; k < thread_id.subThreadIdx; k++)\
                                                        {\
														    thread_id.subThread[k]->state = ALGOTHREAD_STATE_IDLE;\
                                                        }\
													}\
												}\
											}
#define START_SUBTHREAD(parent,child)		\
											AlgoThread & cThread = child;\
											uint32_t sequance = parent.sequanceCnt++;\
											if(cThread.state == ALGOTHREAD_STATE_IDLE)\
											{\
												if(parent.sequance != sequance)\
												{\
													return;\
												}\
												if(parent.subThreadIdx == 0)\
												{\
													parent.subThreadSequance = sequance;\
												}\
												parent.subThread[parent.subThreadIdx++] = &child;\
												child.sequance = 0;\
												cThread.state = ALGOTHREAD_STATE_RUNNING;\
											}\
											cThread.sequanceCnt = 0; \

#define END_SUBTHREAD(parent,thread_id)		{\
												if((thread_id.state != ALGOTHREAD_STATE_COMPLETED) && (thread_id.sequance == cThread.sequanceCnt))\
												{\
													thread_id.state = ALGOTHREAD_STATE_COMPLETED;\
													parent.subThreadCompleted++;\
												}\
												parent.sequance++;\
											}


#define START_LOOP(cnt)						{\
												if((cThread.sequanceCnt == cThread.sequance))\
												{\
													if(cThread.loopState == ALGOTHREAD_LOOP_STATE_IDLE) \
													{\
														cThread.cntStart = 0;\
														cThread.cntEnd = cnt;\
														cThread.loopSequance = cThread.sequance;\
													}\
													cThread.sequance++;\
												}\
												cThread.sequanceCnt++;\
											}
#define END_LOOP()							{\
												if(cThread.loopState == ALGOTHREAD_LOOP_STATE_IDLE)\
												{\
													cThread.loopState = ALGOTHREAD_LOOP_STATE_ACTIVE;\
												}\
												if(cThread.subThreadIdx != cThread.subThreadCompleted)\
												{\
													cThread.sequance = cThread.subThreadSequance;\
												}\
												else \
												{ \
													for(uint8_t k = 0; k < cThread.subThreadIdx; k++)\
													{\
														cThread.subThread[k]->state = ALGOTHREAD_STATE_IDLE;\
														cThread.subThread[k]->sequance = 0;\
													}\
													cThread.subThreadIdx = 0;\
													cThread.subThreadCompleted = 0;\
													if(cThread.sequance == cThread.sequanceCnt)\
													{\
													if(cThread.cntEnd == 0)\
													{\
														cThread.sequance = cThread.loopSequance;\
													}\
													else\
													{\
														cThread.cntStart++;\
														if(cThread.cntStart == cThread.cntEnd)\
														{\
															cThread.loopState = ALGOTHREAD_LOOP_STATE_IDLE;\
															cThread.sequance++;\
														}\
														else\
														{\
															cThread.sequance = cThread.loopSequance;\
														}\
													}\
												}\
												cThread.sequanceCnt++;\
												}	\
											}


/* Exported constants ****************************************************** */
enum ALGOTHREAD_STATE
{
	ALGOTHREAD_STATE_IDLE = 0x00,
	ALGOTHREAD_STATE_RUNNING,
	ALGOTHREAD_STATE_COMPLETED,
};

enum ALGOTHREAD_STATUS
{
	ALGOTHREAD_STATUS_IDLE = 0x00,
	ALGOTHREAD_STATUS_RUNNING,
	ALGOTHREAD_STATUS_COMPLETED,
};


enum ALGOTHREAD_WAIT_STATE
{
	ALGOTHREAD_WAIT_STATE_INIT = 0x00,
	ALGOTHREAD_WAIT_STATE_RUN,
};

enum ALGOTHREAD_LOOP_STATE
{
	ALGOTHREAD_LOOP_STATE_IDLE = 0x00,
	ALGOTHREAD_LOOP_STATE_ACTIVE,
};
/* Exported macros ********************************************************* */

/* Exported types ********************************************************** */
class AlgoThread;

class AlgoThread
{
	private:

	public:
		uint8_t state;
		uint8_t status;
		uint8_t sequance;
		uint8_t sequanceCnt;
		uint8_t cntStart;
		uint8_t cntEnd;
		uint8_t loopState;
		uint8_t loopSequance;
		AlgoThread * subThread[4];
		uint8_t subThreadIdx;
		uint8_t subThreadCompleted;
		uint8_t subThreadSequance;
		uint32_t waitTimer;
        uint32_t waitPeriod;
		uint8_t waitState;
};


/* Exported variables ****************************************************** */
extern AlgoThread & thread0;
extern AlgoThread & thread1;
extern AlgoThread & thread2;
extern AlgoThread & thread3;
extern AlgoThread & thread4;
extern AlgoThread & thread5;
extern AlgoThread & thread6;
extern AlgoThread & thread7;
extern AlgoThread & thread8;
extern AlgoThread & thread9;
extern AlgoThread & thread10;
extern AlgoThread & thread11;
extern AlgoThread & thread12;
extern AlgoThread & thread13;
extern AlgoThread & thread14;
extern AlgoThread & thread15;
extern AlgoThread & thread16;
extern AlgoThread & thread17;
extern AlgoThread & thread18;
extern AlgoThread & thread19;
extern AlgoThread & thread20;
extern AlgoThread & thread21;
extern AlgoThread & thread22;
extern AlgoThread & thread23;
extern AlgoThread & thread24;
extern AlgoThread & thread25;
extern AlgoThread & thread26;
extern AlgoThread & thread27;
extern AlgoThread & thread28;
extern AlgoThread & thread29;
extern AlgoThread & thread30;
extern AlgoThread & thread31;

extern AlgoThread threadAlgoC;

extern AlgoThread threads[THREAD_CNT];

/* Exported functions ****************************************************** */
void chkTHREADS(void);
void initTHREADS(void);

bool isThreadCompleted(AlgoThread & thread);
bool areAllThreadsCompleted(void);
void resetThreadConfiguration(AlgoThread & thread);
bool setThreadCompletedFlag(AlgoThread & thread);
void resetAllThreads(void);

void thread0_run(void) __attribute__((weak));
void thread1_run(void) __attribute__((weak));
void thread2_run(void) __attribute__((weak));
void thread3_run(void) __attribute__((weak));
void thread4_run(void) __attribute__((weak));
void thread5_run(void) __attribute__((weak));
void thread6_run(void) __attribute__((weak));
void thread7_run(void) __attribute__((weak));
void thread8_run(void) __attribute__((weak));
void thread9_run(void) __attribute__((weak));
void thread10_run(void) __attribute__((weak));
void thread11_run(void) __attribute__((weak));
void thread12_run(void) __attribute__((weak));
void thread13_run(void) __attribute__((weak));
void thread14_run(void) __attribute__((weak));
void thread15_run(void) __attribute__((weak));
void thread16_run(void) __attribute__((weak));
void thread17_run(void) __attribute__((weak));
void thread18_run(void) __attribute__((weak));
void thread19_run(void) __attribute__((weak));
void thread20_run(void) __attribute__((weak));
void thread21_run(void) __attribute__((weak));
void thread22_run(void) __attribute__((weak));
void thread23_run(void) __attribute__((weak));
void thread24_run(void) __attribute__((weak));
void thread25_run(void) __attribute__((weak));
void thread26_run(void) __attribute__((weak));
void thread27_run(void) __attribute__((weak));
void thread28_run(void) __attribute__((weak));
void thread29_run(void) __attribute__((weak));
void thread30_run(void) __attribute__((weak));
void thread31_run(void) __attribute__((weak));

void application(AlgoThread & cThread);
#endif 

