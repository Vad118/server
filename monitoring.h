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

class Monitoring:public QObject
{
    Q_OBJECT
    dispatcher *disp;
    _graphics *graphicObj;
    void getClientsList(); // Получаем список клиентов, заполняем clientsList
    void getArbitersList();
    void calculateArbiters();
    void calculateCoordinates();
public:
    int arbitersListCount;
    clientObj clientsList[TOTAL_CLIENTS];
    arbiterObj arbitersList[TOTAL_CLIENTS]; // Арбитров на самом деле может быть больше чем клиентов. Но константа не предусмотрена, поэтому так.
    int totalArbitersCount;
    Monitoring(dispatcher *disp_obj,_graphics *graphic);
    void getClientsArray();
};

#endif // MONITORING_H
