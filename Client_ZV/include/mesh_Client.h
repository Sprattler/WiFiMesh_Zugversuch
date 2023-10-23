
#pragma once
#include<namedMesh.h>

#define MESH_SSID "BodeZiehtAmBaum"
#define MESH_PASSWORD "KimsGroßerZehIstGroß"
#define MESH_PORT 5555

Scheduler userScheduler; // External Schedules -- 0
namedMesh mesh;

/*
--
-

-
---
*/

String nodeName = "Client";

Task taskSendMessage(TASK_SECOND * 30, TASK_ONCE, []()
                     {
    String msg = String("Client Found You");
    mesh.sendBroadcast(msg, false); });

/*
--
-

-
---
*/

void setupMesh()
{

    mesh.setDebugMsgTypes(ERROR | DEBUG | CONNECTION);                  // Conf. Debug messages 

    mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT);     //see at #1

    mesh.setName(nodeName); 
/*
    mesh.onReceive([](uint32_t from, String &msg)
                   { Serial.printf("Received message by id from: %u, %s\n", from, msg.c_str()); });
*/
    mesh.onReceive([](String &from, String &msg)
                   { Serial.printf("Received message by name from: %s, %s\n", from.c_str(), msg.c_str());
                     });

    mesh.onChangedConnections([]()
                              { Serial.printf("Changed connection\n"); });

    userScheduler.addTask(taskSendMessage);
    taskSendMessage.enable();
}