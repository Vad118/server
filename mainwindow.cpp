#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtConcurrent/QtConcurrent>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
    {
        ui->setupUi(this);
        graphics=new _graphics(ui->textEdit,ui->graphicsView->width()-10,ui->graphicsView->height()-10);
        ui->graphicsView->setScene(graphics->PalletScene);
        dispatcher *dsp=new dispatcher(); // TEST
        monitoring=new Monitoring(dsp,graphics);
        server=new _server(graphics, monitoring);
        main_serv_init();

        QThread workerThread;
        MultiThreadServerPart *multiThreadServPart=new MultiThreadServerPart();
        multiThreadServPart->init(server);
        QObject::connect(multiThreadServPart, SIGNAL(graphicsClear()), graphics, SLOT(clear()),Qt::DirectConnection);
        QObject::connect(multiThreadServPart, SIGNAL(showClientSignal(int,int,char*)), graphics, SLOT(paintClient(int,int,char*)),Qt::DirectConnection);
        multiThreadServPart->start();
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
void MainWindow::main_serv_send()
{
    server->clearArbiters();
    server->sendScriptToClients();
    server->work_cycle();
}

void MainWindow::on_SendButton_clicked()
{
    main_serv_send();
}

void MainWindow::on_pushButton_clicked()
{
    //monitoring->show();
}
