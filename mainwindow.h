#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "server.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_SendButton_clicked();

private:
    Ui::MainWindow *ui;

    _graphics *graphics;
    _server *server;
    _monitoring *monitoring;
    dispatcher_answer received_answer;
    dispatcher_answer answer;

    void main_serv_init();
    void main_serv_send();
};

#endif // MAINWINDOW_H
