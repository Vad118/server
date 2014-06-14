#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtConcurrent/QtConcurrent>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
    {
        ui->setupUi(this);
        graphics=new _graphics(ui->textEdit,ui->graphicsView->width()-10,ui->graphicsView->height()-10);
        graphics->clear();
        ui->graphicsView->setScene(graphics->PalletScene);
        dsp=new dispatcher(); // TEST
        monitoring=new Monitoring(dsp,graphics);
        monitoringSocketObj=new MonitoringSocket(dsp,monitoring);
        QObject::connect(monitoringSocketObj, SIGNAL(graphicsClear()), graphics, SLOT(clear()));
        QObject::connect(monitoringSocketObj, SIGNAL(showClientSignal(int,int,char*)), graphics, SLOT(paintClient(int,int,char*)));
        QObject::connect(monitoringSocketObj, SIGNAL(paintArbiterSignal(int,int,int,int,char*)), graphics, SLOT(paintArbiter(int,int,int,int,char*)));
        QObject::connect(monitoringSocketObj, SIGNAL(paintTraceObjectSignal(int,int,int,int,char*,int)), graphics, SLOT(paintTraceObject(int,int,int,int,char*,int)));

        server=new _server(graphics, monitoring,dsp,monitoringSocketObj);
        QObject::connect(server, SIGNAL(graphicsClear()), graphics, SLOT(clear()));
        QObject::connect(server, SIGNAL(showClientSignal(int,int,char*)), graphics, SLOT(paintClient(int,int,char*)));
        QObject::connect(server, SIGNAL(paintArbiterSignal(int,int,int,int,char*)), graphics, SLOT(paintArbiter(int,int,int,int,char*)));
        QObject::connect(server, SIGNAL(paintTraceObjectSignal(int,int,int,int,char*,int)), graphics, SLOT(paintTraceObject(int,int,int,int,char*,int)));

        monitoring_serv_init();
        main_serv_init();

        // Запуск многопоточных классов
        // Поток 2 - CheckNewClients на основной сокет
        MultiThreadServerPart *multiThreadServPart=new MultiThreadServerPart();
        multiThreadServPart->init(server);
        QObject::connect(multiThreadServPart, SIGNAL(graphicsClear()), graphics, SLOT(clear()));
        QObject::connect(multiThreadServPart, SIGNAL(showClientSignal(int,int,char*)), graphics, SLOT(paintClient(int,int,char*)));
        QObject::connect(multiThreadServPart, SIGNAL(paintArbiterSignal(int,int,int,int,char*)), graphics, SLOT(paintArbiter(int,int,int,int,char*)));
        QObject::connect(multiThreadServPart, SIGNAL(paintTraceObjectSignal(int,int,int,int,char*,int)), graphics, SLOT(paintTraceObject(int,int,int,int,char*,int)));
        multiThreadServPart->start();

        // Поток 3 - CheckNewClients на второй сокет
        monitoringCheckNewMultithread=new MonitoringCheckNewMultithread();
        monitoringCheckNewMultithread->init(monitoringSocketObj);
        monitoringCheckNewMultithread->start();

        //TEST_GENERATE_DSP_TABLE();

    }

MainWindow::~MainWindow()
{
    delete server;
    delete ui;
}

void MainWindow::main_serv_init()
{
    int result=server->initialize();
    if(result==1)
        ui->textEdit->setText("WSAStratup error");
    if(result==2)
        ui->textEdit->setText("Error with socket");
    if(result==3)
        ui->textEdit->setText("Error with bind");
    else if(result==4)
        ui->textEdit->setText("Error with listen");
    else if(result==0)
    {
        ui->textEdit->setText("Server started.\nWaiting for connections...");
    }
}
void MainWindow::monitoring_serv_init()
{
    int result= monitoringSocketObj->initialize();
    if(result==1)
        ui->textEdit->setText("WSAStratup error");
    if(result==2)
        ui->textEdit->setText("Error with socket");
    if(result==3)
        ui->textEdit->setText("Error with bind");
    else if(result==4)
        ui->textEdit->setText("Error with listen");
    else if(result==0)
    {
        ui->textEdit->setText("Server started.\nWaiting for connections...");
    }
}

void MainWindow::main_serv_send()
{
    server->clearArbiters();
    server->sendScriptToClients();
    server->start();
}

void MainWindow::on_SendButton_clicked()
{
    monitoringSocketObj->sendCommand(2);
    monitoringSocketObj->monitoringType=2;
    main_serv_send();
}
/*
void MainWindow::on_pushButton_clicked()
{

}

void MainWindow::on_pushButton_3_clicked()
{
    // Paint
    monitoring->getClientsArray();
    graphics->clear();
    for(int i=0;i<dsp->nclients;i++)
        graphics->paintClient(monitoring->clientsList[i].position_x,monitoring->clientsList[i].position_y,monitoring->clientsList[i].worker_addr);
    for(int i=0;i<monitoring->arbitersListCount;i++)
        graphics->paintArbiter(monitoring->arbitersList[i].position_x,
                               monitoring->arbitersList[i].position_y,
                               monitoring->clientsList[monitoring->arbitersList[i].clientsListId].position_x,
                               monitoring->clientsList[monitoring->arbitersList[i].clientsListId].position_y,
                               monitoring->arbitersList[i].arbiter_id);
}
*/

void MainWindow::TEST_GENERATE_DSP_TABLE()
{
    dsp->nclients=6;
    dsp->table[0].arbiters_count=0;
    strcpy(dsp->table[0].arbiters,"|");
    dsp->table[0].worker_addr=0;

    dsp->table[1].arbiters_count=1;
    strcpy(dsp->table[1].arbiters,"|1;0|");
    dsp->table[1].worker_addr=1;

    dsp->table[2].arbiters_count=2;
    strcpy(dsp->table[2].arbiters,"|2;0|2;1|");
    dsp->table[2].worker_addr=2;

    dsp->table[3].arbiters_count=3;
    strcpy(dsp->table[3].arbiters,"|3;0|3;1|3;2|");
    dsp->table[3].worker_addr=3;

    dsp->table[4].arbiters_count=4;
    strcpy(dsp->table[4].arbiters,"|4;0|4;1|4;2|4;3|");
    dsp->table[4].worker_addr=4;

    dsp->table[5].arbiters_count=5;
    strcpy(dsp->table[5].arbiters,"|5;0|5;1|5;2|5;3|5;4|");
    dsp->table[5].worker_addr=5;

}

void MainWindow::on_checkBox_2_clicked()
{
    if(ui->checkBox_2->isChecked())
        ui->pushButton->setEnabled(true);
    else
        ui->pushButton->setEnabled(false);
}

void MainWindow::on_pushButton_clicked()
{
    monitoringSocketObj->sendCommand(3);
    monitoringSocketObj->monitoringType=2;
}

void MainWindow::on_pushButton_3_clicked()
{
    monitoringSocketObj->sendCommand(4);
    monitoringSocketObj->monitoringType=4;
}

void MainWindow::on_pushButton_4_clicked()
{
    server->clearArbiters();
    monitoringSocketObj->loadFile();
    server->sendScriptToClients(true);
    server->loadCreateActors();
    server->loadSendOutputMessages();
    server->loadInputMessages();
    server->start();
}
