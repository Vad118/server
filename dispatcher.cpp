#include "dispatcher.h"

char *ltrim(char *s)
{
    while(isspace(*s)) s++;
    return s;
}

char *rtrim(char *s)
{
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0';
    return s;
}

char *trim(char *s)
{
    return rtrim(ltrim(s));
}

dispatcher::dispatcher(void)
{
//	table = new dispatcher_table[];
    nclients=0;
}

dispatcher::~dispatcher(void)
{
//	delete[] table;
}
void dispatcher::start()
{
    cout<<"start";
}

dispatcher_answer dispatcher::processMessage(dispatcher_answer received_answer)
{
    dispatcher_answer answer;
    if(received_answer.command==1)  //create
    {
        answer=addArbiter(received_answer);
    }
    else if(received_answer.command==2) //send
    {
        answer=sendMessage(received_answer,received_answer.command);
    }
    else
    {
        //cout<<"nothing";
        answer.worker_id=0;
    }
    TEST_FUNC();
    return answer;
}

dispatcher_answer dispatcher::addWorker(int worker_id, SOCKET clientSocket)
{
    dispatcher_answer answer;
    //table надо сделать динамическим массивом(с удалением в деструкторе), где worker_id - индекс.
    //*Добавление
    table[nclients].worker_addr=worker_id;
    table[nclients].clientSocket=clientSocket;
    table[nclients].clientMonitoringSocket=NULL;
    strcpy(table[nclients].arbiters,"|");
    table[nclients].arbiters_count=0;
    nclients++;
    //return createAnswer();

    //Заполняем структуру ответа
    answer.worker_id=-1;
    answer.command=0;
    //	strcpy(answer.arbiter_id,"");
    //answer.actor_create_msg=;
    //answer.parameter=NULL;
    return answer;
}
dispatcher_answer dispatcher::addArbiter(dispatcher_answer receiver_answer)
{
    dispatcher_answer answer;
    int workers[TOTAL_CLIENTS];
    //ищем работника с минимальным количеством арбитров на нем
    int id=0;
    int min=table[id].arbiters_count;
    for(int i=1;i<nclients;i++)
    {
        if(table[i].arbiters_count<min)
        {
            min=table[i].arbiters_count;
            id=i;
        }
    }
    //Добавляем нового арбитра к работнику
    table[id].arbiters_count++;
    char tmp[STR_SIZE];
    char arbiter_id[STR_SIZE];
    strcpy(arbiter_id,receiver_answer.arbiter_id);
    strcpy(tmp,"|");
    strcat(table[id].arbiters,strcat(trim(arbiter_id),tmp));

    //Заполняем структуру ответа
    answer.worker_id=id;
    answer.command=1;
    //answer.actor_behavior=receiver_answer.actor_behavior;
    answer.actor_par_count=receiver_answer.actor_par_count;
    for(int i=0;i<answer.actor_par_count;i++)
        strcpy(answer.actor_parameters[i],receiver_answer.actor_parameters[i]);
    strcpy(answer.arbiter_id,receiver_answer.arbiter_id);
    strcpy(answer.arbiter_parent,receiver_answer.arbiter_parent);
    strcpy(answer.actor_behavior,receiver_answer.actor_behavior);

    return answer;
}
dispatcher_answer dispatcher::sendMessage(dispatcher_answer receiver_answer,int command)
{
    dispatcher_answer answer;
    char message_text[STR_SIZE];
    char message_obr[STR_SIZE];
    char arbiter_id[ARBITER_ID_SIZE];
    //Ищем worker_id которому принадлежит арбитр
    char arbiter_id_to_search[STR_SIZE];
    //Делаем строку вида |arbiter_id|
    strcpy(arbiter_id_to_search,"|");
    strcat(arbiter_id_to_search,receiver_answer.arbiter_id);
    strcat(arbiter_id_to_search,"|");
    int worker_id=-1;
    int id=0;
    for(int i=0;i<nclients;i++)
    {
        if(table[i].arbiters_count>0)
        {
            if(strstr(table[i].arbiters,arbiter_id_to_search)!=NULL)
            {
                worker_id=i;
                break;
            }
        }
    }
    //Заполняем структуру ответа
    answer.worker_id=worker_id;
    answer.command=command;
    strcpy(answer.arbiter_id,receiver_answer.arbiter_id);
    strcpy(answer.arbiter_parent,receiver_answer.arbiter_parent);
    strcpy(answer.actor_behavior,receiver_answer.actor_behavior);
    answer.actor_par_count=receiver_answer.actor_par_count;
    for(int i=0;i<answer.actor_par_count;i++)
        strcpy(answer.actor_parameters[i],receiver_answer.actor_parameters[i]);
    return answer;
}


void dispatcher::TEST_FUNC()
{
    for(int i=0;i<nclients;i++)
    {
        cout<<"worker_addr:"<<table[i].worker_addr<<endl;
        cout<<"arbiters_count:"<<table[i].arbiters_count<<endl;
        cout<<"arbiters:"<<table[i].arbiters<<endl;
        cout<<endl<<endl;
    }
}

void dispatcher::clearArbiters()
{
    for(int i=0;i<nclients;i++)
    {
        strcpy(table[i].arbiters,"|");
        table[i].arbiters_count=0;
    }
}



