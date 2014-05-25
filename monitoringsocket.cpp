#include "monitoringsocket.h"

MonitoringSocket::MonitoringSocket(dispatcher *disp_par, Monitoring *monitoring)
{
    disp=disp_par;
    this->monitoring=monitoring;
}

int MonitoringSocket::initialize()
{
    int ret=0;
    //++++++++++Создание сокета++++++
    if (WSAStartup(0x0202, &WsaData))
    {
        return 1;
    }
    // SOCK_STREAM - потоковый сокет (с установкой соединения)
    // 0 - по умолчанию выбирается TCP протокол
    if ((monitorSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
       return 2;
    // вызываем bind для связывания
    // Привязыкаем адрес интерфейса и номер порта к прослушивающему сокету
    sockaddr_in local;
    local.sin_family=AF_INET;      // AF_INET - сокет Интернета
    local.sin_port=htons(MONITORING_PORT);    //Номер порта
    local.sin_addr.s_addr=htonl(INADDR_ANY); //IP Адрес сервера

    if(bind(monitorSocket, (sockaddr *)&local, sizeof(local)))
       ret=3;
    //Помечаем сокет как прослушивающий. 5- стандатрное значение очереди
    if(listen(monitorSocket,5))
       ret=4;
    return ret;
}

void MonitoringSocket::stop()
{
    closesocket(monitorSocket);
    WSACleanup();
}

void MonitoringSocket::sendCommand(int command)
{
    sendStruct send_struct;
    send_struct.command=command;
    for(int i=0;i<disp->nclients;i++)
    {
        sendMessage(i,send_struct);
    }
}

receiveStruct MonitoringSocket::receiveMessage(int client_id)
{
    receiveStruct answer;
    char *p;
    fd_set readfds;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    SOCKET clientSocket=disp->table[client_id].clientMonitoringSocket;
    //Очищаем readfds
    FD_ZERO(&readfds);
    //Заносим дескриптор сокета в readfds
    FD_SET(clientSocket,&readfds);
    //Последний параметр - время ожидания. Выставляем нули чтобы
    //Select не блокировал выполнение программы до смены состояния сокета
    select(NULL,&readfds,NULL,NULL,&tv);
    //Если пришли данные на чтение то читаем
    if(FD_ISSET(clientSocket,&readfds))
    {
        int bytes_recv;
        //Если клиент не отключился и мы получили сообщение
        char *pBuff = new char[STR_SIZE];
        if((bytes_recv=recv(clientSocket,pBuff,sizeof( receiveStruct),0)) &&(bytes_recv!=SOCKET_ERROR) && (bytes_recv!=-1))
        {
            memcpy( &answer, pBuff, sizeof( receiveStruct));
        }
        //else //Клиент отключился
        //{
        //    answer.command=-1;
        //    clientDisconnected(client_id);
        //}
        delete[] pBuff;
    }
    /*else
        answer.command=-1;*/
    return answer;
}


void MonitoringSocket::sendMessage(int client_id, sendStruct send_struct)
{
     SOCKET clientSocket=disp->table[client_id].clientMonitoringSocket;
     char *pBuff = new char[sizeof(sendStruct)];
     memcpy(pBuff,&send_struct,sizeof(sendStruct));
     send(clientSocket,pBuff, sizeof(sendStruct), 0);
}

//--------------------------------------------------------

void MonitoringCheckNewMultithread::init(MonitoringSocket *monitoringSocket)
{
    this->monitoringSocketObj=monitoringSocket;
}

void MonitoringCheckNewMultithread::run()
{
    //для select
    fd_set readfds;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    //+++++++++++++++++++++++++++++++
    //Проверяем подключение нового клиента
    //Очищаем readfds
    while(1)
    {
        FD_ZERO(&readfds);
        //Заносим дескриптор сокета в readfds
        FD_SET(monitoringSocketObj->monitorSocket,&readfds);
        //Последний параметр - время ожидания. Выставляем нули чтобы
        //Select не блокировал выполнение программы до смены состояния сокета
        select(NULL,&readfds,NULL,NULL,&tv);
        //Если пришли данные на чтение то читаем
        SOCKET clientSocket;
        char buf[STR_SIZE];
        if(FD_ISSET(monitoringSocketObj->monitorSocket,&readfds))
        {
            if(clientSocket = accept(monitoringSocketObj->monitorSocket, 0,0))
            {
                // Клиент первым запросом на этот сокет шлет client_id
                int b=recv(clientSocket,buf,STR_SIZE,0);
                if(b!=-1)
                {
                    char srv_resp[STR_SIZE];
                    int client_id=atoi(buf);
                    if(client_id>=0 && client_id<=TOTAL_CLIENTS)
                    {
                        monitoringSocketObj->disp->table[client_id].clientMonitoringSocket=clientSocket;
                        send(clientSocket, "done", STR_SIZE, 0);
                    }
                }
            }
        }
    }
}

//------------------------------------------------------
void MonitoringReceiveMultithread::init(MonitoringSocket *monitoringSocket)
{
    this->monitoringSocketObj=monitoringSocket;
}

void MonitoringReceiveMultithread::run()
{
    while(1)
    {
        receiveStruct msg;
        msg.command=-1;
        strcpy(msg.text,"");
        bool changed=false;
        for(int i=0;i<monitoringSocketObj->disp->nclients;i++)
        {
            msg=monitoringSocketObj->receiveMessage(i);
            if(msg.command>=0 && strcmp(msg.text,"")!=0)
            {
                monitoringSocketObj->monitoring->traceObjectsList[i].type=msg.command;
                strcpy(monitoringSocketObj->monitoring->traceObjectsList[i].text,msg.text);
                changed=true;
            }
        }
        if(changed)
        {
            draw();
        }

    }
}

void MonitoringReceiveMultithread::draw()
{
    // Одинаковые: _server::showClients(), MultithreadServerPart::showClients(), MonitoringReceiveMultithread::draw();
    QMutex mutex;
    mutex.lock();
    monitoringSocketObj->monitoring->getClientsArray();
    emit graphicsClear();
    for(int i=0;i<monitoringSocketObj->disp->nclients;i++)
    {
        emit showClientSignal(monitoringSocketObj->monitoring->clientsList[i].position_x,monitoringSocketObj->monitoring->clientsList[i].position_y,monitoringSocketObj->monitoring->clientsList[i].worker_addr);
    }
    for(int i=0;i<monitoringSocketObj->monitoring->arbitersListCount;i++)
    {
        emit paintArbiterSignal(monitoringSocketObj->monitoring->arbitersList[i].position_x,
                               monitoringSocketObj->monitoring->arbitersList[i].position_y,
                               monitoringSocketObj->monitoring->clientsList[monitoringSocketObj->monitoring->arbitersList[i].clientsListId].position_x,
                               monitoringSocketObj->monitoring->clientsList[monitoringSocketObj->monitoring->arbitersList[i].clientsListId].position_y,
                               monitoringSocketObj->monitoring->arbitersList[i].arbiter_id);
        if(monitoringSocketObj->monitoring->traceObjectsList[i].type!=-1)
        {
            emit paintTraceObjectSignal(monitoringSocketObj->monitoring->traceObjectsList[i].position_x,
                                  monitoringSocketObj->monitoring->traceObjectsList[i].position_y,
                                  monitoringSocketObj->monitoring->arbitersList[i].position_x,
                                  monitoringSocketObj->monitoring->arbitersList[i].position_y,
                                  monitoringSocketObj->monitoring->traceObjectsList[i].text,
                                  monitoringSocketObj->monitoring->traceObjectsList[i].type);
        }
    }
    mutex.unlock();
}
