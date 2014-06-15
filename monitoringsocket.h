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
#include <fstream>
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
    // 2 - стоп
    // 3 - следующий шаг
    // 4 - сохранение
    int command;
};

struct saveActor
{
     char behavior[STR_SIZE];
     char parameters[5][50];
     int count;
     char id[STR_SIZE];

     int totalSaveCount;
     int totalUnreadMessages;
};

// Структура получаемая от клиента при мониторинге/трассировке/сохранении
struct receiveStruct
{
    // 0 - create
    // 1 - send
    // 2 - become
    // 31 - save_start_send
    // 32 - save_end_send
    int command;
    char text[STR_SIZE];
    char arbiter_id[STR_SIZE];
    char arbiter_parent[10];
};

class MonitoringSocket:public QObject
{
    Q_OBJECT
    friend class MonitoringCheckNewMultithread;
    friend class MonitoringReceiveMultithread;

    dispatcher *disp;
    SOCKET monitorSocket;
    WSADATA WsaData;
    Monitoring *monitoring;

    int total_saved_actors;
    int total_saved_pull_messages;

public:
    int monitoringType;  // Соответствует sendStruct
    string save_file;

    char visible_arbiters[TOTAL_ARBITERS][10];
    int total_visible_arbiters;

    saveActor saveActorsStruct[TOTAL_ARBITERS];
    dispatcher_answer clientsMessagesPull[TOTAL_ARBITERS];
    dispatcher_answer all_received_answers[TOTAL_ARBITERS];

    MonitoringSocket(dispatcher *disp_par, Monitoring *monitoring);
    int initialize();
    receiveStruct receiveMessage(int client_id);
    void sendMessage(int client_id, sendStruct send_struct);
    void stop();
    string echo(int client_id);
    void sendCommand(int command);

    void getMonitoringMessage();
    void draw();
    void collectActorsAndTheirMessages();    // Сбор актеров, функция вызывается для каждого клиента и заполняет saveActorsStruct
    void save();
    void loadFile();
    bool isVisibleArbiter(char *arbiter_id);
signals:
    void showClientSignal(int x,int y,char* str);
    void paintArbiterSignal(int x, int y, int client_x, int client_y, char* text);
    void paintTraceObjectSignal(int x, int y, int arbiter_x, int arbiter_y, char* text, int type);
    void graphicsClear();
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
