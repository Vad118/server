#include "monitoring.h"

Monitoring::Monitoring(dispatcher *disp_obj, _graphics *graphic):totalArbitersCount(0)
{
    disp=disp_obj;
    graphicObj=graphic;
}

void Monitoring::getClientsArray()
{
    getClientsList();
    calculateCoordinates();

    getArbitersList();
    calculateArbiters();

    calculateTraceObjects();
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
    int position_y=SERVER_RECT_HEIGHT+SERVER_CLIENTS_HEIGHT;
    for(int i=0;i<disp->nclients;i++)
    {
        position_x+=step;
        clientsList[i].position_x=position_x;
        clientsList[i].position_y=position_y;
    }
}

void Monitoring::getArbitersList()
{
    int k=0;
    totalArbitersCount=0;
    for(int i=0;i<disp->nclients;i++)
    {
        if(disp->table[i].arbiters_count>0)
        {
            totalArbitersCount+=disp->table[i].arbiters_count;
            char arbiters_str[STR_SIZE];
            strcpy(arbiters_str,disp->table[i].arbiters);
            char* arb_id=strtok(arbiters_str,"|");
            for(int j=0;j<disp->table[i].arbiters_count;j++)
            {
                strcpy(arbitersList[k].arbiter_id,arb_id);
                arb_id=strtok(NULL,"|");
                arbitersList[k].clientsListId=i;
                k++;
            }
        }
    }
    arbitersListCount=k;
}

void Monitoring::calculateArbiters()
{
    int i=0;
    while(i<arbitersListCount)
    {
        int client_id=arbitersList[i].clientsListId;
        if(disp->table[client_id].arbiters_count>0)
        {
            int step_x=2*CLIENT_RECT_WIDTH/disp->table[client_id].arbiters_count;
            //int countLevels=(int)ceilf(disp->table[client_id].arbiters_count/2.0f);
            int pos_x=clientsList[client_id].position_x-CLIENT_RECT_WIDTH/2;
            int start_y=clientsList[client_id].position_y+CLIENTS_ARBITERS_HEIGHT;
            int pos_y=start_y;
            //int countStepY=1;
            for(int j=0;j<disp->table[client_id].arbiters_count;j++)
            {
                arbitersList[i].position_x=pos_x;
                arbitersList[i].position_y=pos_y;
                pos_x+=step_x;
                //if(countStepY<countLevels)
                    pos_y+=ARBITERS_Y_STEP;
                //else
                //    pos_y-=ARBITERS_Y_STEP;

                //countStepY++;
                i++;
            }
        }
        else
            i++;
    }
}

void Monitoring::calculateTraceObjects()
{
    int zero_arbiter_x=0;
    int zero_arbiter_y=25;
    int z=30;
    for(int i=0;i<disp->nclients;i++)
    {
        if(traceObjectsList[i].type!=-1)
        {
            for(int j=0;j<totalArbitersCount;j++)
            {
                if(strcmp(traceObjectsList[i].arbiter_id,"-1")==0) // Значит create без арбитра
                {
                    traceObjectsList[i].position_x=zero_arbiter_x;
                    traceObjectsList[i].position_y=zero_arbiter_y;
                    zero_arbiter_y+=5;
                }
                if(strcmp(traceObjectsList[i].arbiter_id,arbitersList[j].arbiter_id)==0)
                {
                    traceObjectsList[i].position_x=arbitersList[j].position_x;
                    traceObjectsList[i].position_y=arbitersList[j].position_y+z;
                    z+=25;
                }
            }
        }
    }
}
