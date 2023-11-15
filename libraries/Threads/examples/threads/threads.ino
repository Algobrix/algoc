#include <algobot.h>

void thread0_run(void);
void thread1_run(void);
void thread2_run(void);
void thread3_run(void);

void application(ALGOC_APP)
{
  do
  {
    thread0_run();
    thread1_run();
  }while(areAllThreadsCompleted() == false);
}

void thread0_run(void)
{
  START_THREAD(thread0);
  move(ALGOC,'B',2,10,CW,true);
  wait(ALGOC,2);
  move(ALGOC,'C',2,10,CW,true);
  END_THREAD(thread0);
}


void thread1_run(void)
{
  START_THREAD(thread1)
  START_LOOP(5);
  light(ALGOC,1,1,10,"Blue",true);
  light(ALGOC,1,1,0,"Blue",true);
  thread2_run();
  thread3_run();
  END_LOOP();
  END_THREAD(thread1)
}

void thread2_run(void)
{
  START_SUBTHREAD(thread1,thread2);
  light(ALGOC,1,1,10,"Red",true);
  light(ALGOC,2,1,0,"Red",true);
  END_SUBTHREAD(thread1,thread2);
}

void thread3_run(void)
{
  START_SUBTHREAD(thread1,thread3);
  playSound(ALGOC,3,5,true);
  END_SUBTHREAD(thread1,thread3);
}
