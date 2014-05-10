#ifndef MONITORING_H
#define MONITORING_H

#include <vector>
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
    vector<client_obj> *clientsList;
    dispatcher *disp;
    graphics *graphicObj;
    void getClientsList(); // Получаем список клиентов, заполняем clientsList
    void showClients();
    void showArbiters();
    void calculateCoordinates();
public:
    Monitoring(dispatcher *disp_obj,graphics *graphic);
    void show();
};

#endif // MONITORING_H
