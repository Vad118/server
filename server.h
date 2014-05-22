#ifndef SERVER_H
#define SERVER_H


#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <string>
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include "dispatcher.h"
#include "graphics.h"
#include "monitoringsocket.h"
#include "monitoring.h"
#include <QThread>
#include <QObject>


#include <string>
#include <iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")

class _server
{
friend class MultiThreadServerPart;
private:
    dispatcher *disp;
    SOCKET ServerSocket;
    WSADATA WsaData;
    int idclient;
    _graphics *graphics;
    Monitoring *monitoring;
    MonitoringSocket *monitoringSocket;
public:
    _server(_graphics *graphics, Monitoring *monitoring);
    int initialize();
    void stop();
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
    void work_cycle();

};


class MultiThreadServerPart:public QThread
{
    Q_OBJECT
    _server *server;

    void showClients();
protected:
        void run();
public:
        void init(_server *server);
        void checkForNewClients();
signals:
    void showClientSignal(int x,int y,char* str);
    void graphicsClear();
};

#endif // SERVER_H
