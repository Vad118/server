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
public slots:
    void paintConfigurator();

private slots:
    void on_SendButton_clicked();

    void on_checkBox_2_clicked();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_checkBox_clicked();

   // void on_pushButton_2_clicked();

    void on_action_3_triggered();

    void on_MainWindow_destroyed();

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

private:
    Ui::MainWindow *ui;

    dispatcher *dsp;
    _graphics *graphics;
    _server *server;
    Monitoring *monitoring;
    MonitoringSocket *monitoringSocketObj;
    MonitoringCheckNewMultithread *monitoringCheckNewMultithread;
    //MultiThreadServerPart *multiThreadServPart;
    dispatcher_answer received_answer;
    dispatcher_answer answer;

    void main_serv_init();
    void monitoring_serv_init();
    void main_serv_send();
    void setMonitoringType();
    void clickExit();
    void closeEvent(QCloseEvent *event);

    void TEST_GENERATE_DSP_TABLE();

    int configuratorChoosenId;
};



#endif // MAINWINDOW_H
