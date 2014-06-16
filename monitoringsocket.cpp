#include "monitoringsocket.h"

MonitoringSocket::MonitoringSocket(dispatcher *disp_par, Monitoring *monitoring)
{
    disp=disp_par;
    this->monitoring=monitoring;
    save_file="save.txt";
    script_file=
    total_saved_actors=0;
    total_saved_pull_messages=0;
    total_visible_arbiters=0;
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
    for(int i=0;i<disp->nclients;i++)
        closesocket(disp->table[i].clientMonitoringSocket);
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
    answer.command=-1;
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
        char pBuff[STR_SIZE];
        if((bytes_recv=recv(clientSocket,pBuff,sizeof( receiveStruct),0)) &&(bytes_recv!=SOCKET_ERROR) && (bytes_recv!=-1))
        {
            memcpy( &answer, pBuff, sizeof( receiveStruct));
        }
        else
        {
            answer.command=-2;
        }
    }
    else
        answer.command=-1;
    return answer;
}


void MonitoringSocket::sendMessage(int client_id, sendStruct send_struct)
{
     SOCKET clientSocket=disp->table[client_id].clientMonitoringSocket;
     char *pBuff = new char[sizeof(sendStruct)];
     memcpy(pBuff,&send_struct,sizeof(sendStruct));
     send(clientSocket,pBuff, sizeof(sendStruct), 0);
}

void MonitoringSocket::getMonitoringMessage()
{
    bool changed=false;
    receiveStruct msg;
    msg.command=-1;
    strcpy(msg.text,"");
    for(int i=0;i<disp->nclients;i++)
    {
        QMutex mutex;
        mutex.lock();
        do  // Каждый раз считываем все сообщения пришедшие на этот сокет
        {
            msg=receiveMessage(i);
            if(msg.command>=0 && msg.command<=100 && strcmp(msg.text,"")!=0)
            {
                monitoring->traceObjectsList[i].type=msg.command;
                strcpy(monitoring->traceObjectsList[i].text,msg.text);
                strcpy(monitoring->traceObjectsList[i].arbiter_id,msg.arbiter_parent);
                if(msg.command==0)
                {
                    strcpy(visible_arbiters[total_visible_arbiters],msg.arbiter_id);
                    total_visible_arbiters++;
                }
                changed=true;
            }
        }while(msg.command!=-1);
        mutex.unlock();
    }
    if(changed)
    {
         draw();
    }

}

void MonitoringSocket::collectActorsAndTheirMessages()
{
    for(int i=0;i<disp->nclients;i++)
    {
        bool finish=false;
        int count=0;
        // Собираем актеров
        int count_client_messages=0;
        while(!finish)
        {
            fd_set readfds;
            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 0;
            SOCKET clientSocket=disp->table[i].clientMonitoringSocket;
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
                char *pBuff = new char[sizeof(saveActor)];
                if((bytes_recv=recv(clientSocket,pBuff,sizeof( saveActor),0)) &&(bytes_recv!=SOCKET_ERROR) && (bytes_recv!=-1))
                {
                    memcpy( &saveActorsStruct[total_saved_actors], pBuff, sizeof( saveActor));
                    count++;
                    count_client_messages=saveActorsStruct[total_saved_actors].totalUnreadMessages;
                    if(count>=saveActorsStruct[total_saved_actors].totalSaveCount)  // Останваливаемся, когда считали всех актеров для сохранения с 1-го клиента
                        finish=true;
                    total_saved_actors++;
                }
                delete[] pBuff;
            }
            else
                finish=true;
        }
        // Собираем пул сообщений
        count=0;
        finish=false;
        if(count_client_messages>0)
        {
            // Собираем сообщения
            while(!finish)
            {
                fd_set readfds;
                struct timeval tv;
                tv.tv_sec = 0;
                tv.tv_usec = 0;
                SOCKET clientSocket=disp->table[i].clientMonitoringSocket;
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
                    char *pBuff = new char[sizeof(dispatcher_answer)];
                    if((bytes_recv=recv(clientSocket,pBuff,sizeof( dispatcher_answer),0)) &&(bytes_recv!=SOCKET_ERROR) && (bytes_recv!=-1))
                    {
                        memcpy( &clientsMessagesPull[total_saved_pull_messages], pBuff, sizeof( dispatcher_answer));
                        count++;
                        if(count>=count_client_messages)
                            finish=true;
                        total_saved_pull_messages++;
                    }
                    delete[] pBuff;
                }
                else
                    finish=true;
            }
        }
    }
    saveSizesStruct.size_saveActorsStruct=total_saved_actors;
    saveSizesStruct.size_clientsMessagesPull=total_saved_pull_messages;
}

void MonitoringSocket::save(string file_script)
{
    // Непосредственно сохранение

    char cfile_script[STR_SIZE];
    strcpy(cfile_script,file_script.c_str());

    ofstream f(this->save_file.c_str(),ios::in | ios::binary | ios::trunc);
    f.write((char *)&saveSizesStruct,sizeof(saveSizesStruct));
    f.write((char *)&all_received_answers,sizeof(all_received_answers));
    f.write((char *)&clientsMessagesPull,sizeof(clientsMessagesPull));
    f.write((char *)&saveActorsStruct,sizeof(saveActorsStruct));
    f.write(cfile_script,sizeof(cfile_script));    // Дописываем имя файла скрипта в конец
    f.close();

    /*ofstream f(this->save_file.c_str(),ios::out | ios::binary | ios::app);
    f.write((char *)&all_received_answers,sizeof(all_received_answers));
    f.close();
    ofstream f2("save2.txt",ios::out | ios::binary | ios::app);
    f2.write((char *)&clientsMessagesPull,sizeof(clientsMessagesPull));
    f2.close();
    ofstream f3("save3.txt",ios::out | ios::binary | ios::app);
    f3.write((char *)&saveActorsStruct,sizeof(saveActorsStruct));
    f3.close();*/




}

bool MonitoringSocket::isVisibleArbiter(char* arbiter_id)
{
    bool visible=false;
    if(monitoringType>0)
    {
        for(int i=0;i<total_visible_arbiters;i++)
            if(strcmp(arbiter_id,visible_arbiters[i])==0)
                visible=true;
    }
    else
        visible=true;

    return visible;
}

void MonitoringSocket::draw()
{
    // Одинаковые: _server::showClients(), MultithreadServerPart::showClients(), MonitoringSocket::draw();
    QMutex mutex;
    mutex.lock();
    monitoring->getClientsArray();
    emit graphicsClear();
    for(int i=0;i<disp->nclients;i++)
    {
        emit showClientSignal(monitoring->clientsList[i].position_x,monitoring->clientsList[i].position_y,monitoring->clientsList[i].worker_addr);
    }
    for(int i=0;i<monitoring->arbitersListCount;i++)
    {
        if(isVisibleArbiter(monitoring->arbitersList[i].arbiter_id))
        {
            emit paintArbiterSignal(monitoring->arbitersList[i].position_x,
                               monitoring->arbitersList[i].position_y,
                               monitoring->clientsList[monitoring->arbitersList[i].clientsListId].position_x,
                               monitoring->clientsList[monitoring->arbitersList[i].clientsListId].position_y,
                               monitoring->arbitersList[i].arbiter_id);
        }

        if(monitoring->traceObjectsList[i].type!=-1)
        {
            emit paintTraceObjectSignal(monitoring->traceObjectsList[i].position_x,
                                  monitoring->traceObjectsList[i].position_y,
                                  monitoring->arbitersList[i].position_x,
                                  monitoring->arbitersList[i].position_y,
                                  monitoring->traceObjectsList[i].text,
                                  monitoring->traceObjectsList[i].type);
        }
    }
    mutex.unlock();
}


void MonitoringSocket::loadFile(string &file_script)
{
    // Загрузка файла
    char cfile_script[STR_SIZE];

    ifstream f(this->save_file.c_str(),ios::in | ios::binary);
    f.read((char *)&saveSizesStruct,sizeof(saveSizesStruct));
    f.read((char *)&all_received_answers,sizeof(all_received_answers));
    f.read((char *)&clientsMessagesPull,sizeof(clientsMessagesPull));
    f.read((char *)&saveActorsStruct,sizeof(saveActorsStruct));
    f.read(cfile_script,sizeof(cfile_script));
    file_script=cfile_script;
    f.close();

    // Отсылаем

}

//--------------------------------------------------------

void MonitoringCheckNewMultithread::init(MonitoringSocket *monitoringSocket)
{
    this->monitoringSocketObj=monitoringSocket;
    global_quit=false;
}

void MonitoringCheckNewMultithread::run()
{
    check();
}

void MonitoringCheckNewMultithread::check()
{
    //для select
    fd_set readfds;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    //+++++++++++++++++++++++++++++++
    //Проверяем подключение нового клиента
    //Очищаем readfds
    //while(!global_quit)
    //{
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
                        monitoringSocketObj->disp->table[client_id].clientMonitoringSocketStatus=0;
                        send(clientSocket, "done", STR_SIZE, 0);
                    }
                }
            }
        }
    //}
}

void MonitoringCheckNewMultithread::globalQuit()
{
    global_quit=true;
}

//------------------------------------------------------
