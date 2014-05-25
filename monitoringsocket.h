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
#include "monitoring.h"
#include <QThread>
#include <QObject>

// Структура комманды отправляемой клиенту
struct sendStruct
{
    // 0 - обычный режим
    // 1 - мониторинг
    // 2 - трассировка
    // 3 - следующий шаг
    // 4 - сохранение
    int command;
};

// Структура получаемая от клиента при мониторинге/трассировке
struct receiveStruct
{
    // 0 - create
    // 1 - send
    // 2 - become
    int command;
    char text[STR_SIZE];
};

class MonitoringSocket
{
    friend class MonitoringCheckNewMultithread;
    friend class MonitoringReceiveMultithread;

    dispatcher *disp;
    SOCKET monitorSocket;
    WSADATA WsaData;
    Monitoring *monitoring;
public:
    MonitoringSocket(dispatcher *disp_par, Monitoring *monitoring);
    int initialize();
    receiveStruct receiveMessage(int client_id);
    void sendMessage(int client_id, sendStruct send_struct);
    void stop();
    string echo(int client_id);
    void sendCommand(int command);
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

class MonitoringReceiveMultithread: public QThread
{
    Q_OBJECT
    MonitoringSocket *monitoringSocketObj;
protected:
    void run();
    void draw();
public:
    void init(MonitoringSocket *monitoringSocket);
signals:
    void showClientSignal(int x,int y,char* str);
    void paintArbiterSignal(int x, int y, int client_x, int client_y, char* text);
    void paintTraceObjectSignal(int x, int y, int arbiter_x, int arbiter_y, char* text, int type);
    void graphicsClear();
};

#endif // MONITORINGSOCKET_H
