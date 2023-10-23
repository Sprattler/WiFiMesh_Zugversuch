#include "mesh_Client.h"
#include "board.h"



String nodeName = "Client";
namedMesh mesh;

Task taskSendMessage(TASK_SECOND * 30, TASK_FOREVER, []()
                     {
    String msg = String("Client Found You");
    mesh.sendBroadcast(msg, false); });

void setupMesh(Scheduler *myScheduler)
{

    mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION); // Conf. Debug messages

    mesh.init(MESH_SSID, MESH_PASSWORD, myScheduler, MESH_PORT); // see at #1

    mesh.setName(nodeName);

    mesh.onReceive([](String &from, String &msg)
                   { Serial.printf("Received message by name from: %s, %s\n", from.c_str(), msg.c_str()); BlinkOnRX();});

    mesh.onChangedConnections([]()
                              { Serial.printf("Changed connection\n"); });

    myScheduler->addTask(taskSendMessage);
    taskSendMessage.enable();
}

void updateMesh()
{

    mesh.update();
}