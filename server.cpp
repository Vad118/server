#include "server.h"

_server::_server(_graphics *graphic)
{
    graphics=graphic;
}

int _server::initialize()
{
    int ret=0;
    disp=new dispatcher();
    disp->nclients=0;
    idclient=0;
    //++++++++++Создание сокета++++++
    if (WSAStartup(0x0202, &WsaData))
        {
           return 1;
        }
    // SOCK_STREAM - потоковый сокет (с установкой соединения)
    // 0 - по умолчанию выбирается TCP протокол
    if ((ServerSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
       return 2;
    // вызываем bind для связывания
    // Привязыкаем адрес интерфейса и номер порта к прослушивающему сокету
    sockaddr_in local;
    local.sin_family=AF_INET;      // AF_INET - сокет Интернета
    local.sin_port=htons(MY_PORT);    //Номер порта
    local.sin_addr.s_addr=htonl(INADDR_ANY); //IP Адрес сервера

    if(bind(ServerSocket, (sockaddr *)&local, sizeof(local)))
       ret=3;
    //Помечаем сокет как прослушивающий. 5- стандатрное значение очереди
    if(listen(ServerSocket,5))
       ret=4;
    return ret;


    //++++++++++++++++


    //1) Получаем всех клиентов
    //2) Для всех клиентов делаем
    //disp->addWorker(worker_id);
    //2) Рассылаем всем клиентам программу
    //3) Запускаем на диспетчере процесс старта
    //disp->start();
}

void _server::stop()
{
    closesocket(ServerSocket);
    WSACleanup();
}

void _server::checkForNewClients()
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
                //ЗДЕСЬ МОЖЕТ БЫТЬ КОПИРОВАНИЕ ip КЛИЕНТА
                char srv_resp[STR_SIZE];
                _itoa(idclient,srv_resp,10);
                srv_resp[strlen(srv_resp)]='\0';
                //sprintf(srv_resp,"%d",idclient);
                send(clientSocket, srv_resp, STR_SIZE, 0);
                disp->addWorker(idclient,clientSocket);
                idclient++;
                showClients();
            }
        }
        else
            quit=true;
    }
}

void _server::showClients()
{
    char *str;
    itoa(getTotalClients(),str,10);
    graphics->TextEditAppend(str);
    delete str;
}

dispatcher_answer _server::receiveMessage(int client_id)
{
    char message[STR_SIZE];
    char *p;
    fd_set readfds;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    SOCKET clientSocket=disp->table[client_id].clientSocket;
    //Очищаем readfds
    FD_ZERO(&readfds);
    //Заносим дескриптор сокета в readfds
    FD_SET(clientSocket,&readfds);
    //Последний параметр - время ожидания. Выставляем нули чтобы
    //Select не блокировал выполнение программы до смены состояния сокета
    select(NULL,&readfds,NULL,NULL,&tv);
    //Если пришли данные на чтение то читаем
    dispatcher_answer answer;
    if(FD_ISSET(clientSocket,&readfds))
    {
        int bytes_recv;
        //Если клиент не отключился и мы получили сообщение
        char *pBuff = new char[sizeof(dispatcher_answer)];
        if((bytes_recv=recv(clientSocket,pBuff,sizeof(dispatcher_answer),0)) &&(bytes_recv!=SOCKET_ERROR))
        {
            memcpy( &answer, pBuff, sizeof( dispatcher_answer));
        }
        else //Клиент отключился
        {
            answer.command=-1;
            clientDisconnected(client_id);
        }
        delete[] pBuff;
    }
    else
        answer.command=-1;

    return answer;
}

void _server::clientDisconnected(int client_id)
{
    SOCKET clientSocket=disp->table[client_id].clientSocket;
    closesocket(clientSocket);
    //После удаления если был удалён не последний клиент то т.к.
    //Число эл-в в массиве уменьшается на 1, берём последний эл-т из массива и заносим
    //На позицию удалённого
    //nclients-1 т.к. в nclients текущее кол-во клиентов если считать с 1
    if (client_id<disp->nclients-1)
    {
        disp->table[client_id]=disp->table[disp->nclients-1];
    }
    disp->nclients--;
    //showClients();
}


void _server::sendMessage(int client_id, dispatcher_answer answer)
{
     /*char msg[STR_SIZE];
     sprintf(msg,"%d",i);
     strcat(msg,": ");
     strcat(msg,buf);*/
     SOCKET clientSocket=disp->table[client_id].clientSocket;
     char *pBuff = new char[sizeof(dispatcher_answer)];
     memcpy(pBuff,&answer,sizeof(dispatcher_answer));
     send(clientSocket,pBuff, sizeof(dispatcher_answer), 0);
}

int _server::getClients()
{
    return disp->nclients;
}

dispatcher_answer _server::processMessage(dispatcher_answer received_answer)
{
    dispatcher_answer answer;
    answer=disp->processMessage(received_answer);
    return answer;
}

int _server::getTotalClients()
{
    return disp->nclients;
}

void _server::send_file(int client_id)
{
    //FILE *in = fopen("C:\\practise\\server\\Debug\\script.lua", "r");
    FILE *in = fopen("C:\\QTProjects\\diplom_server\\server\\script.lua", "r");
    SOCKET clientSocket=disp->table[client_id].clientSocket;
    char buffer[1024];
    size_t b=0;
    size_t size=0;
    dispatcher_answer answer;
    answer.command=61;
    strcpy(answer.actor_behavior,"");
    answer.actor_par_count=-1;
    strcpy(answer.arbiter_id,"");
    answer.worker_id=-1;
    while(!feof(in))
    {
        b=fread(buffer,1,sizeof(buffer),in);
        size=ftell(in);
        //printf("bytes read: %d, part:%d, pos: %ld \n",b,i,size);
        if(b!=0)
        {
            if(b<1024)
                buffer[b]='\0';
            strcpy(answer.script,buffer);
            sendMessage(client_id,answer);
        }
    }
    answer.command=62;
    strcpy(answer.script,buffer);
    sendMessage(client_id,answer);

    fclose(in);
}

void _server::clearArbiters()
{
    disp->clearArbiters();
}

void _server::showAnswer(dispatcher_answer received_answer, bool final)
{
    string str;
    if(final)
    {
        str="FINAL result: ";
        str.append(received_answer.actor_parameters[0]);
    }
    else
    {
        str="result: ";
        str.append(received_answer.actor_parameters[0]);
    }
    graphics->TextEditAppend(str.c_str());
}

void _server::sendScriptToClients()
{
    // Раньше назывался getInput И было считывание SEND и QUIT из консоли
    // Теперь это первоначальная рассылка скрипта
    checkForNewClients();
    if(getTotalClients()>0)
    {
        //Формируем структуру
        dispatcher_answer answer;
        answer.command=6;   //Рассылаем скрипт
        strcpy(answer.script,"");
        //Забиваем нулями оставшуюся часть структуры, чтобы не передавать мусор
        answer.actor_par_count=0;
        strcpy(answer.actor_behavior,"");
        strcpy(answer.arbiter_id,"");
        //Отправляем всем клиентам
        for(int i=0;i<getTotalClients();i++)
        {
            answer.worker_id=i; //Нужно для создания файла на клиенте
            sendMessage(i,answer);
            send_file(i);
        }
        answer.command=7;  //Стартуем скрипт
        strcpy(answer.script,"");
        sendMessage(0,answer); //Старт работы всегда на нулевом работнике
    }
}


void _server::work_cycle(dispatcher_answer received_answer, dispatcher_answer answer)
{
      bool finished=false;
      while (!finished)
      {
           checkForNewClients();
           //+++++++++
           //Читаем текущих клиентов
           for(int i=0;i<getTotalClients();i++)
           {
               received_answer=receiveMessage(i);
               if(received_answer.command!=-1)
               {
                   switch(received_answer.command)
                   {
                       case 4:
                           showAnswer(received_answer,false);
                           break;
                       case 5:
                           showAnswer(received_answer,true);
                           finished=true;
                           break;
                       default:
                           answer=processMessage(received_answer);
                           if(answer.worker_id!=-1)
                               sendMessage(answer.worker_id,answer);
                           break;
                   }
               }
           }
       }
stop();
}





//---------------------------------------------------------------------------


