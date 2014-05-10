#ifndef MONITORING_H
#define MONITORING_H

#include <vector>
#include "server.h"
#include "dispatcher.h"
#include "graphics.h"


struct clientObj
{
    int worker_addr;
    int position_x;
    int position_y;
};

class Monitoring
{
    clientObj clientsList[TOTAL_CLIENTS];
    dispatcher *disp;
    _graphics *graphicObj;
    int nclients;
    void getClientsList(); // Получаем список клиентов, заполняем clientsList
    void showClients();
    void showArbiters();
    void calculateCoordinates();
public:
    Monitoring(dispatcher *disp_obj,_graphics *graphic);
    void show();
};

#endif // MONITORING_H
