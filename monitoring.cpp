#include "monitoring.h"

Monitoring::Monitoring(dispatcher *disp_obj,graphics *graphic)
{
    disp=disp_obj;
    graphicsObj=graphic;
}

void show()
{
    getClientsList();
    showClients();
    showArbiters();
}

void Monitoring::getClientsList()
{
    clientsList.clear();
    for(int i=0;i<disp->nclients;i++)
    {
        clientObj _clientObj;
        _clientObj.worker_addr=disp->dispatcher_table[i]->worker_addr;
        clientsList->push_back(_clientObj);
    }
}

void Monitoring::showClients()
{
    graphicObj->clear();
    calculateCoordinates();
    for(int i=0;i<clientsList->size();i++)
    {
        string text=clientsList[i].worker_addr;
        graphicObj->paintClient(clientsList[i].position_x,clientsList[i].position_y,text);
    }
}

void Monitoring::calculateCoordinates()
{
    float step=(graphicObj->getWidth()-CLIENT_RECT_WIDTH*2)/clientsList->size();
    int position_x=0;
    int position_y=40;
    for(int i=0;i<clientsList->size();i++)
    {
        position_x+=step;
        clientsList[i].position_x=position_x;
        clientsList[i].position_y=position_x;
    }
}
