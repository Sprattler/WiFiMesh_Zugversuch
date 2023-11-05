#include "mesh_Client.h"
#include "board_Client.h"



String nodeName = "Client";
namedMesh mesh;

Task taskSendMessage(TASK_MINUTE, TASK_FOREVER, []()
                     {
    String msg = String("-->Node1<-- ON RX ");
    mesh.sendBroadcast(msg,false); });

void setupMesh(Scheduler *myScheduler)
{

    mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION); // Conf. Debug messages

    mesh.init(MESH_SSID, MESH_PASSWORD, myScheduler, MESH_PORT); // see at #1

    mesh.setRoot();

    mesh.setName(nodeName);

    mesh.onReceive([](String &from, String &msg)
                   { Serial.printf("Client-RX: %s, %s\n", from.c_str(), msg.c_str());});

    mesh.onChangedConnections([]()
                              { Serial.printf("Changed connection\n"); });

    myScheduler->addTask(taskSendMessage);
    taskSendMessage.enable();
}

void updateMesh()
{

    mesh.update();
}