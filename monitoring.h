#ifndef MONITORING_H
#define MONITORING_H

#include <vector>
#include <iostream>
#include "dispatcher.h"
#include "graphics.h"

using namespace std;

struct clientObj
{
    char worker_addr[STR_SIZE];
    int position_x;
    int position_y;
};

struct arbiterObj
{
    char arbiter_id[STR_SIZE];
    int position_x;
    int position_y;
    int clientsListId;
};

struct traceObjects
{
    int position_x;
    int position_y;
    int type;
    char text[STR_SIZE];
    char arbiter_id[STR_SIZE];

    traceObjects():position_x(0),position_y(0),type(-1){}
};


class Monitoring:public QObject
{
    Q_OBJECT
    dispatcher *disp;
    _graphics *graphicObj;
    void getClientsList(); // Получаем список клиентов, заполняем clientsList
    void getArbitersList();
    void calculateArbiters();
    void calculateCoordinates();
    void calculateTraceObjects();
public:
    int arbitersListCount;
    clientObj clientsList[TOTAL_CLIENTS];
    arbiterObj arbitersList[TOTAL_ARBITERS];
    traceObjects traceObjectsList[TOTAL_ARBITERS];
    int totalArbitersCount;
    Monitoring(dispatcher *disp_obj,_graphics *graphic);
    void getClientsArray();
};

#endif // MONITORING_H
