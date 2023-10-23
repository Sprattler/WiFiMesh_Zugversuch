#include "mesh_Client.h"

void setup()
{

  setupBoard();
  setupMesh();

}

void loop()
{

  // it will run the user scheduler as well
  mesh.update();
}
