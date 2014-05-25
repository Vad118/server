#include "monitoringsocket.h"

MonitoringSocket::MonitoringSocket(dispatcher *disp_par)
{
    disp=disp_par;
    sendMessage(0);
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

string MonitoringSocket::echo(int client_id)
{
    sendMessage(client_id);
    string msg="";
    msg=receiveMessage(client_id);
    while(msg=="")
        msg=receiveMessage(client_id);
    return msg;
}

string MonitoringSocket::receiveMessage(int client_id)
{
    char message[STR_SIZE];
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
    string msg="";
    if(FD_ISSET(clientSocket,&readfds))
    {
        int bytes_recv;
        //Если клиент не отключился и мы получили сообщение
        char *pBuff = new char[STR_SIZE];
        if((bytes_recv=recv(clientSocket,pBuff,STR_SIZE,0)) &&(bytes_recv!=SOCKET_ERROR))
        {
            msg.assign(pBuff);
            //memcpy( &answer, pBuff, sizeof( dispatcher_answer));
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



    return msg;
}


void MonitoringSocket::sendMessage(int client_id)
{
     /*char msg[STR_SIZE];
     sprintf(msg,"%d",i);
     strcat(msg,": ");
     strcat(msg,buf);*/
    /*
     SOCKET clientSocket=disp->table[client_id].clientSocket;
     char *pBuff = new char[sizeof(dispatcher_answer)];
     memcpy(pBuff,&answer,sizeof(dispatcher_answer));
     send(clientSocket,pBuff, sizeof(dispatcher_answer), 0);*/
    SOCKET clientSocket=disp->table[client_id].clientMonitoringSocket;
    char msg[STR_SIZE];
    strcpy(msg,"test message");
    send(clientSocket,msg, STR_SIZE, 0);
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
