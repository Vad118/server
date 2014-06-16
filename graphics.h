#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <QtWidgets>
#include <QObject>
#include <QtWidgets/QTextEdit>
#include <QPainter>
#include <string>

enum{CLIENT_RECT_WIDTH=55,
     CLIENT_RECT_HEIGHT=20,
     SERVER_RECT_WIDTH=50,
     SERVER_RECT_HEIGHT=20,
     ACTOR_CIRCLE_RADIUS=25,
     SERVER_CLIENTS_HEIGHT=60,
     CLIENTS_ARBITERS_HEIGHT=60,
     ARBITERS_Y_STEP};

class _graphics:public QObject
{
    Q_OBJECT
    QTextEdit *textEdit;
    int width,height;
    void paintServer(); // вызывается в clear
    int server_x;
    int server_y;
public:
    QGraphicsScene *PalletScene;
    _graphics(QTextEdit *edit,int width, int height);
    ~_graphics();
    void TextEditAppend(const char *str);
    int getHeight(){return height;}
    int getWidth(){return width;}
public slots:
    void paintClient(int x, int y, char* text);
    void paintArbiter(int x, int y, int client_x, int client_y, char* text);
    void paintTraceObject(int x, int y, int arbiter_x, int arbiter_y, char* text, int type);
    void clear();
    void showClientDisconnectedError();
};

#endif // GRAPHICS_H
