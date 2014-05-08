#ifndef SERVER_H
#define SERVER_H


#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include "dispatcher.h"
#include "graphics.h"

#include <string>
#include <iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")

class _server
{
private:
    dispatcher *disp;
    SOCKET ServerSocket;
    WSADATA WsaData;
    int idclient;
    _graphics *graphics;
public:
    _server(_graphics *graphic);
    int initialize();
    void stop();
    void checkForNewClients();
    dispatcher_answer receiveMessage(int client_id);
    void clientDisconnected(int client_id);
    void sendMessage(int client_id, dispatcher_answer answer);
    int getClients();
    dispatcher_answer processMessage(dispatcher_answer received_answer);
    int getTotalClients();
    void send_file(int client_id);
    void clearArbiters();
    void showAnswer(dispatcher_answer received_answer, bool final=true);
    void sendScriptToClients();
    void work_cycle(dispatcher_answer received_answer, dispatcher_answer answer);
    void showClients();

};

#endif // SERVER_H
