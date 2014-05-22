#include "monitoring.h"

Monitoring::Monitoring(dispatcher *disp_obj, _graphics *graphic)
{
    disp=disp_obj;
    graphicObj=graphic;
    nclients=0;//TEST
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
    //nclients=disp->nclients;
    nclients++;//TEST
    for(int i=0;i<disp->nclients;i++)
    {
        clientObj _clientObj;
        //_clientObj.worker_addr=disp->table[i].worker_addr;
        char* tmp;
        itoa(i,tmp,10);
        _clientObj.worker_addr=tmp; // TEST
        clientsList[i]=_clientObj;
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
