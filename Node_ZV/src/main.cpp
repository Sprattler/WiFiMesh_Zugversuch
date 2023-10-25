#include "board_Node.h"
#include "mesh_Node.h"


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
