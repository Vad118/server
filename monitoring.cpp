#include "monitoring.h"

Monitoring::Monitoring(dispatcher *disp_obj, _graphics *graphic)
{
    disp=disp_obj;
    graphicObj=graphic;
}

void Monitoring::getClientsArray()
{
    getClientsList();
    calculateCoordinates();
    //return clientsList;
    //Если включен мониторинг
    //showArbiters();
}


void Monitoring::getClientsList()
{
    for(int i=0;i<disp->nclients;i++)
    {
        clientObj _clientObj;
        itoa(disp->table[i].worker_addr,_clientObj.worker_addr,10);
        clientsList[i]=_clientObj;
    }
}

void Monitoring::calculateCoordinates()
{
    float step=(graphicObj->getWidth()-CLIENT_RECT_WIDTH)/(disp->nclients+1);
    int position_x=0;
    int position_y=40;
    for(int i=0;i<disp->nclients;i++)
    {
        position_x+=step;
        clientsList[i].position_x=position_x;
        clientsList[i].position_y=position_y;
    }
}
