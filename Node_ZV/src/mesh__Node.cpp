#include "mesh_Node.h"
#include "board_Node.h"



String nodeName = "Node1";
namedMesh mesh;

Task taskSendMessage(TASK_SECOND * 30, TASK_FOREVER, []()
                     {
    String msg = String("-->Node1<-- ON RX ");
    mesh.sendSingle(853244037,msg); });

void setupMesh(Scheduler *myScheduler)
{

    mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION); // Conf. Debug messages

    mesh.init(MESH_SSID, MESH_PASSWORD, myScheduler, MESH_PORT); // see at #1

    mesh.setContainsRoot();

    mesh.setName(nodeName);

    mesh.onReceive([](String &from, String &msg)
                   { Serial.printf("??????RX??????: %s, %s\n", from.c_str(), msg.c_str()); BlinkOnRX();});

    mesh.onChangedConnections([]()
                              { Serial.printf("Changed connection\n"); });

    myScheduler->addTask(taskSendMessage);
    taskSendMessage.enable();
}

void updateMesh()
{

    mesh.update();
}