#include "monitoringsocket.h"

MonitoringSocket::MonitoringSocket(dispatcher *disp_par)
{
    disp=disp_par;
}

int MonitoringSocket::init(int port)
{
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
    local.sin_port=htons(port);    //Номер порта
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

void MonitoringSocket::checkForNewClients()
{
    //для select
    fd_set readfds;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    //+++++++++++++++++++++++++++++++
    //Проверяем подключение нового клиента
    //Очищаем readfds
    bool quit=false;
    while(!quit)
    {
        FD_ZERO(&readfds);
        //Заносим дескриптор сокета в readfds
        FD_SET(ServerSocket,&readfds);
        //Последний параметр - время ожидания. Выставляем нули чтобы
        //Select не блокировал выполнение программы до смены состояния сокета
        select(NULL,&readfds,NULL,NULL,&tv);
        //Если пришли данные на чтение то читаем
        SOCKET clientSocket;
        char buf[STR_SIZE];
        if(FD_ISSET(ServerSocket,&readfds))
        {
            if(clientSocket = accept(ServerSocket, 0,0))
            {
                int b=recv(clientSocket,buf,STR_SIZE,0);
                char srv_resp[STR_SIZE];
                int client_id;
                // ТУТ ПАРСИНГ HANDSHAKE от клиента
                disp->dispatcher_table[client_id].clietnMonitoringSocket=clientSocket;

                /* Тут ответ клиенту. Ниже пример из сервера
                _itoa(client_id,srv_resp,10);
                srv_resp[strlen(srv_resp)]='\0';
                //sprintf(srv_resp,"%d",idclient);
                send(clientSocket, srv_resp, STR_SIZE, 0);*/
            }
        }
        else
            quit=true;
    }
}
