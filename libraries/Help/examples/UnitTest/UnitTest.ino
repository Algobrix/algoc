/**
 *This examples enables us to test every component of the system.
 * 
 * To compile this example you need to disable debug from Tools->Debug->Disable
 */
 
#include <algoC.h>

void motorDirectionTest(ALGOC_APP);
void motorPowerTest(ALGOC_APP);
void motorRotationTest(ALGOC_APP);
void ledColorTest(ALGOC_APP);
void ledPowerTest(ALGOC_APP);
void soundTest(ALGOC_APP);
void waitSensorTest(ALGOC_APP);
void getSensorTest(ALGOC_APP);
void waitSignalTest(ALGOC_APP);
void isBusyTest(ALGOC_APP);
void resistanceToStopTest(ALGOC_APP);
void countingTest(ALGOC_APP);
void playButtonTest(ALGOC_APP);



enum TEST_ID 
{
  TEST_ID_MOTOR_DIRECTION = 1,
  TEST_ID_MOTOR_POWER,
  TEST_ID_MOTOR_ROTATION,
  TEST_ID_LED_COLOR,
  TEST_ID_LED_POWER,
  TEST_ID_SOUND,
  TEST_ID_WAIT_SENSOR,
  TEST_ID_GET_SENSOR,
  TEST_ID_WAIT_SIGNAL,
  TEST_ID_IS_BUSY,
  TEST_ID_COUNTING,
  TEST_ID_RESISTANCE_TO_STOP,
  TEST_ID_PLAY_BUTTON
};
void application(ALGOC_APP)
{
  while(1)
  {
    int testNumber = enterNumber(ALGOC);
    writeLine(ALGOC,testNumber);
    switch(testNumber)
    {
      case(TEST_ID_MOTOR_DIRECTION):
      {
        motorDirectionTest(ALGOC_FUNCTION);
        break;
      }
      case(TEST_ID_MOTOR_POWER):
      {
        motorPowerTest(ALGOC_FUNCTION);
        break;
      }
      case(TEST_ID_MOTOR_ROTATION):
      {
        motorRotationTest(ALGOC_FUNCTION);
        break;
      }
      case (TEST_ID_LED_COLOR):
      {
        ledColorTest(ALGOC_FUNCTION);
        break;
      }
      case (TEST_ID_LED_POWER):
      {
        ledPowerTest(ALGOC_FUNCTION);
        break;
      }
      case (TEST_ID_SOUND):
      {
        soundTest(ALGOC_FUNCTION);
        break;
      }
      case (TEST_ID_WAIT_SENSOR):
      {
        waitSensorTest(ALGOC_FUNCTION);
        break;
      }
      case (TEST_ID_GET_SENSOR):
      {
        getSensorTest(ALGOC_FUNCTION);
        break;
      }
      case (TEST_ID_WAIT_SIGNAL):
      {
        waitSignalTest(ALGOC_FUNCTION);
        break;
      }
      case (TEST_ID_IS_BUSY):
      {
        isBusyTest(ALGOC_FUNCTION);
        break;
      }
      case (TEST_ID_COUNTING):
      {
        countingTest(ALGOC_FUNCTION);
        break;
      }
      case (TEST_ID_RESISTANCE_TO_STOP):
      {
        resistanceToStopTest(ALGOC_FUNCTION);
        break;
      }
      case (TEST_ID_PLAY_BUTTON):
      {
        playButtonTest(ALGOC_FUNCTION);
        break;
      }
    }

  }
}

 void motorDirectionTest(ALGOC_APP)
 {
    uint8_t k = 0;
    int8_t value = 1;
    for( k = 0 ; k < 2; k++)
    {
        move(ALGOC,'A',2,10,value,true);
        move(ALGOC,'B',2,10,value,true);
        move(ALGOC,'C',2,10,value,true);
        value = value * (-1);
    }
 }
 void motorPowerTest(ALGOC_APP)
 {
   move(ALGOC,'A',5,1,CW,true);
   move(ALGOC,'B',5,5,CW,true);
   move(ALGOC,'C',5,10,CW,true);
 }

 void motorRotationTest(ALGOC_APP)
 {
   rotations(ALGOC,'A',10,1,CW,true);
   rotations(ALGOC,'B',10,5,CW,true);
   rotations(ALGOC,'C',10,10,CW,true);
 }

 void ledColorTest(ALGOC_APP)
 {
   light(ALGOC,1,FOREVER,10,"Red",false);
   light(ALGOC,2,FOREVER,10,"Green",false);
   wait(ALGOC,2);
   stopLight(ALGOC,1);
   wait(ALGOC,1);
   stopLight(ALGOC,2);
 }

 void ledPowerTest(ALGOC_APP)
 {
   int k = 0;
   for(k = 0; k <= 10; k++)
   {
     light(ALGOC,1,FOREVER,k,"Blue",false);
     light(ALGOC,2,FOREVER,k,"Blue",false);
     wait(ALGOC,0.1);
   }
   for(k = 10; k >= 0; k--)
   {
     light(ALGOC,1,FOREVER,k,"Blue",false);
     light(ALGOC,2,FOREVER,k,"Blue",false);
     wait(ALGOC,0.1);
   }
 }

 void soundTest(ALGOC_APP)
 {
   playSound(ALGOC,1,3,false);
   light(ALGOC,1,2,10,"Red",true);
   stopSound();
   light(ALGOC,1,1,10,"Green",true);
   light(ALGOC,1,FOREVER,10,"Red",false);
   playSound(ALGOC,2,10,true);
   light(ALGOC,1,1,10,"Green",true);
 }

 void waitSensorTest(ALGOC_APP)
 {
   waitSensor(ALGOC,1,10,10);
   light(ALGOC,1,FOREVER,10,"Red",false);
   waitSensor(ALGOC,1,5,5);
   light(ALGOC,1,FOREVER,5,"Red",false);
   waitSensor(ALGOC,1,1,1);
   light(ALGOC,1,FOREVER,0,"Red",false);
 }
 void getSensorTest(ALGOC_APP)
 {
   uint8_t timeout = 10;
   uint8_t p_power = 0;
   while(timeout)
   {
     uint8_t power = getSensor(ALGOC,1);
     if(p_power != power)
     {
        p_power = power;
        light(ALGOC,1,FOREVER,power,"Red",false);
        timeout--;
      }

   }
 }

 void waitSignalTest(ALGOC_APP)
 {
   int cnt = 2;
   while(cnt)
   {
     waitForPressSensor(ALGOC,2,true);
     move(ALGOC,'A',FOREVER,10,CW,false);
     wait(ALGOC,1);
     waitForPressSensor(ALGOC,2,true);
     stopMotor(ALGOC,'A');
     cnt--;
     wait(ALGOC,1);
   }
 }

void isBusyTest(ALGOC_APP)
{
  bool motorBusy = false;
  bool lightBusy = false;
  bool soundBusy = false;
  const char motors[3]={'A','B','C'};
  const char lights[2]={1,2};

  int p = 0;
  int k = 0;

  for(k = 0; k < 3; k++)
  {
    move(ALGOC,motors[k],3,3,CW,false);
  }
  for(k = 0; k < 2; k++)
  {
    light(ALGOC,lights[k],3,10,"Blue",false);
  }
  playSound(ALGOC,1,1,false);

  for(p = 0; p < 2; p++)
  {
    for(k = 0; k < 3; k++)
    {
      motorBusy = isMotorBusy(ALGOC,motors[k]);
      write(ALGOC, "Motor ");
      write(ALGOC, motors[k]);
      write(ALGOC, " busy: ");
      writeLine(ALGOC,motorBusy);
    }
    for(k = 0; k < 2; k++)
    {
      lightBusy = isLightBusy(ALGOC,lights[k]);
      write(ALGOC, "Light ");
      write(ALGOC, k);
      write(ALGOC, " busy: ");
      writeLine(ALGOC,lightBusy);
    }
    soundBusy = isSoundBusy(ALGOC);
    write(ALGOC, "Sound busy: ");
    writeLine(ALGOC,soundBusy);
    if(p == 0)
    {
      wait(ALGOC,10);
    }
  }
}

void countingTest(ALGOC_APP)
{
  int start = 0;

  waitPlayButtonPress(ALGOC,1);
  

  float counterA = 0;
  float counterB = 0;
  float counterC = 0;

  startCounting(ALGOC, 'A', counterA);
  startCounting(ALGOC, 'B', counterB);
  startCounting(ALGOC, 'C', counterC);

  moveABC(ALGOC, FOREVER, 3, CW, false);

  wait(ALGOC, 3);
  
  stopMotor(ALGOC, 'A');
  stopMotor(ALGOC, 'B');
  stopMotor(ALGOC, 'C');

  stopCounting(ALGOC, 'A');
  stopCounting(ALGOC, 'B');
  stopCounting(ALGOC, 'C');

  rotations(ALGOC, 'A', counterA, 3, CCW, false);
  rotations(ALGOC, 'B', counterB, 3, CCW, false);
  rotations(ALGOC, 'C', counterC, 3, CCW, false);
}

void resistanceToStopTest(ALGOC_APP)
{
  float counter = 0;

  resistanceToStop(ALGOC, 'A', 0.5);
  startCounting(ALGOC, 'A', counter);
  move(ALGOC, 'A', 10, 5, CW, true);
  stopCounting(ALGOC, 'A');

  write(ALGOC, "Rotations made with 50% threshhold: ");
  writeLine(ALGOC, counter);

  wait(ALGOC, 2);
  rotations(ALGOC, 'A', counter, 5, CCW, true);
  wait(ALGOC, 1);

  counter = 0;
  resistanceToStop(ALGOC, 'A', 0.3);
  startCounting(ALGOC, 'A', counter);
  move(ALGOC, 'A', 10, 5, CW, true);
  stopCounting(ALGOC, 'A');

  write(ALGOC, "Rotations made with 30% threshhold: ");
  writeLine(ALGOC, counter);

  wait(ALGOC, 2);
  rotations(ALGOC, 'A', counter, 5, CCW, true);
  wait(ALGOC, 1);
  
  counter = 0;
  resistanceToStop(ALGOC, 'A', 0.1);
  startCounting(ALGOC, 'A', counter);
  move(ALGOC, 'A', 10, 5, CW, true);
  stopCounting(ALGOC, 'A');

  write(ALGOC, "Rotations made with 10% threshhold: ");
  writeLine(ALGOC, counter);

  wait(ALGOC, 2);
  rotations(ALGOC, 'A', counter, 5, CCW, true);
}

void playButtonTest(ALGOC_APP)
{
  int timeout = 3;
  while(timeout) 
  {
    light(ALGOC, 1, 3, 10, "Red", false);
    light(ALGOC, 2, 3, 10, "Red", false);
    playSound(ALGOC, 1, 1, false);
    move(ALGOC, 'A', 3, 2, CW, false);
    wait(ALGOC,2);
    waitSensor(ALGOC, 1, 5, 10);
    --timeout;

  }
}
