#ifndef MONITORING_H
#define MONITORING_H

#include <vector>
#include <iostream>
#include "dispatcher.h"
#include "graphics.h"

using namespace std;

struct clientObj
{
    string worker_addr;
    int position_x;
    int position_y;
};

class Monitoring:public QObject
{
    Q_OBJECT
    dispatcher *disp;
    _graphics *graphicObj;
    void getClientsList(); // Получаем список клиентов, заполняем clientsList
    void showArbiters();
    void calculateCoordinates();
public:
    int nclients;
    clientObj clientsList[TOTAL_CLIENTS];

    Monitoring(dispatcher *disp_obj,_graphics *graphic);
    void getClientsArray();
};

#endif // MONITORING_H
