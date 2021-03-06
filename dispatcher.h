#pragma once

#include <iostream>
#include <string>
#include <winsock2.h>

using namespace std;
//COMMAND_SIZE МОЖНО ИЗБЕЖАТЬ, СМ strspn
enum{STR_SIZE=256,COMMAND_SIZE=3,ARBITER_ID_SIZE=10,TOTAL_CLIENTS=100,MY_PORT=7500};


struct dispatcher_answer
{
    int command;   //Команда, интерпретируется на стороне клиента.
    // 1 - create
    // 2 - send
    // 4 - print
    // 5 - finish print
    // 6 - Первоначальная рассылка скрипта(всем работникам)
    // 61- идет процесс пересылки скрипта
    // 62- процесс пересылки завершен
    // 7 - Запуск скрипта(одному работнику)
    int worker_id; //кому отправляем
    char arbiter_id[STR_SIZE]; //Арбитр, которому отправляем
    //actor actor_create_msg; //Параметры для передачи в create\become

    //Параметры actor
    char actor_behavior[STR_SIZE];
    char actor_parameters[5][50]; //Параметр для передачи в send
    /*char actor_addressat[STR_SIZE];
    int  actor_val;
    bool actor_isValSet;*/
    //string actor_behavior;
    //string actor_parameters[100];
    int actor_par_count;
    char script[1024];
};

class dispatcher
{
public:
    struct dispatcher_table
    {
        int worker_addr;
        SOCKET clientSocket;
        char arbiters[STR_SIZE];
        int arbiters_count;
    }table[TOTAL_CLIENTS];
    int nclients;
///functions
private:
    dispatcher_answer addArbiter(dispatcher_answer received_answer);
    dispatcher_answer sendMessage(dispatcher_answer received_answer,int command);
public:
    dispatcher(void);
    ~dispatcher(void);
    void start();
    dispatcher_answer processMessage(dispatcher_answer received_answer);
    dispatcher_answer addWorker(int worker_id, SOCKET clientSocket);
    void clearArbiters();

    void TEST_FUNC();
    //void

};
