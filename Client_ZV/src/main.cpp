#include "board.h"
#include "mesh_Client.h"


Scheduler myScheduler; // External Schedules -- in all other headerfiles

void setup()
{

  setupBoard();


  setupMesh(&myScheduler);
}

void loop()
{
  updateMesh();
}
