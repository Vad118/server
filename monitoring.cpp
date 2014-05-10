#include "monitoring.h"

Monitoring::Monitoring(dispatcher *disp_obj, _graphics *graphic)
{
    disp=disp_obj;
    graphicObj=graphic;
    nclients=0;//TEST
}

void Monitoring::show()
{
    getClientsList();
    showClients();
    //showArbiters();
}

void Monitoring::getClientsList()
{
    //nclients=disp->nclients;
    nclients++;//TEST
    for(int i=0;i<disp->nclients;i++)
    {
        clientObj _clientObj;
        //_clientObj.worker_addr=disp->table[i].worker_addr;
        _clientObj.worker_addr=i; // TEST
        clientsList[i]=_clientObj;
    }
}

void Monitoring::showClients()
{
    graphicObj->clear();
    calculateCoordinates();
    for(int i=0;i<nclients;i++)
    {
        char* text;
        itoa(clientsList[i].worker_addr,text,10);
        graphicObj->paintClient(clientsList[i].position_x,clientsList[i].position_y,text);
    }
}

void Monitoring::calculateCoordinates()
{
    float step=(graphicObj->getWidth()-CLIENT_RECT_WIDTH*2)/nclients;
    int position_x=0;
    int position_y=40;
    for(int i=0;i<nclients;i++)
    {
        position_x+=step;
        clientsList[i].position_x=position_x;
        clientsList[i].position_y=position_y;
    }
}
