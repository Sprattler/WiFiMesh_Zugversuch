#include "board_Client.h"
#include "mesh_Client.h"


Scheduler myScheduler; // External Schedules -- in all other headerfiles

void setup()
{

  setupBoard(&myScheduler);


  setupMesh(&myScheduler);
}

void loop()
{
  updateMesh();
}
