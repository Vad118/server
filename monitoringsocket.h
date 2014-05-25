// Клиенты хранятся в dispatcher->dispatcher_table.
// При подключении нового клиента, он отправляет данные на 2 сокета.
// На первом получает свой client_id, и отправляет его первым сообщением
// на второй. Здесь описан второй сокет
#ifndef MONITORINGSOCKET_H
#define MONITORINGSOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include "dispatcher.h"
#include <QThread>
#include <QObject>

class MonitoringSocket
{
    friend class MonitoringCheckNewMultithread;

    dispatcher *disp;
    SOCKET monitorSocket;
    WSADATA WsaData;
public:
    MonitoringSocket(dispatcher *disp_par);
    int initialize();
    string receiveMessage(int client_id);
    void sendMessage(int client_id);
    void stop();
    string echo(int client_id);
};

class MonitoringCheckNewMultithread: public QThread
{
    Q_OBJECT
    MonitoringSocket *monitoringSocketObj;
protected:
    void run();
public:
    void init(MonitoringSocket *monitoringSocket);
};

#endif // MONITORINGSOCKET_H
