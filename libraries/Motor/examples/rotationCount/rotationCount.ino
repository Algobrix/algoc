#include <algobot.h>

void application(ALGOC_APP)
{
    float rotationCounter = 0;
    startCounting(ALGOC,'A',rotationCounter);
    move(ALGOC,'A',1,5,CW,true);
    stopCounting(ALGOC,'A');
    Serial.print("Number of rotations made by motorA: ");
    Serial.println(rotationCounter);
    rotationCounter = 0;
    startCounting(ALGOC,'B',rotationCounter);
    rotations(ALGOC,'B',5,5,CW,false);
    wait(ALGOC,1);
    stopCounting(ALGOC,'B');
    Serial.print("Number of rotations made by motorB: ");
    Serial.println(rotationCounter);
    wait(ALGOC,3);
}
