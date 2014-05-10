// Клиенты хранятся в dispatcher->dispatcher_table.
// При подключении нового клиента, если включен мониторинг - ему в
// handshake отправляется сообщение с client_id из dispatcher_table на подключение второго сокета по другому порту, для мониторинга
// В ответ клиент отправляет client_id уже на этот порт и сохраняется второй клиентский сокет
// В этом классе только создается такой сокет.

#ifndef MONITORINGSOCKET_H
#define MONITORINGSOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include "dispatcher.h"

class MonitoringSocket
{
    dispatcher *disp;
    SOCKET monitorSocket;
    WSADATA WsaData;
public:
    MonitoringSocket(dispatcher *disp_par);
    void checkForNewClients(); // ПАРСИНГ HANDSHAKE не доделан
    int initialize(int port);
    //receiveMessage // для трассировки
    void stop();
};

#endif // MONITORINGSOCKET_H
