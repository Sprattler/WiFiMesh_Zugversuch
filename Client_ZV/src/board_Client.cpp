#include "board_Client.h"
#include <Arduino.h>

// Scheduler boardSheduler;
bool flag = true;

Task BlinkOnRX(TASK_SECOND, TASK_FOREVER, []()
               {

                if(flag = true)
                  {digitalWrite(2, HIGH);
                  flag = false;}
                  else
                     {digitalWrite(2, LOW);
                     flag = false;}

    ; });

void setupBoard(Scheduler *myScheduler)
{
  pinMode(2, OUTPUT);
  Serial.begin(115200);
  myScheduler->addTask(BlinkOnRX);
  BlinkOnRX.enable();
};
