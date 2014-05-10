#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
    {
        ui->setupUi(this);
        graphics=new _graphics(ui->textEdit,ui->graphicsView->width()-10,ui->graphicsView->height()-10);
        ui->graphicsView->setScene(graphics->PalletScene);
        server=new _server(graphics);
        dispatcher *dsp=new dispatcher(); // TEST
        monitoring=new Monitoring(dsp,graphics);
        main_serv_init();
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
    server->work_cycle(received_answer,answer);
}

void MainWindow::on_SendButton_clicked()
{
    main_serv_send();
}

void MainWindow::on_pushButton_clicked()
{
    monitoring->show();
}
