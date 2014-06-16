#include "graphics.h"



_graphics::_graphics(QTextEdit *edit, int width, int height)
{
    textEdit=edit;
    PalletScene=new QGraphicsScene(0,0,width,height);
    this->width=width;
    this->height=height;

}

_graphics::~_graphics()
{
    delete PalletScene;
}

void _graphics::TextEditAppend(const char *str)
{
    textEdit->append(str);
}

void _graphics::paintClient(int x, int y, char* str)
{
    QBrush br(Qt::white);
    QPen pen(Qt::black);
    PalletScene->addLine(server_x+SERVER_RECT_WIDTH/2,server_y+SERVER_RECT_HEIGHT,x+CLIENT_RECT_WIDTH/2,y,pen);
    PalletScene->addRect(x,y,CLIENT_RECT_WIDTH,CLIENT_RECT_HEIGHT,pen, br);

    QGraphicsTextItem* io = new QGraphicsTextItem;
    io->setPos(x,y);
    io->setPlainText(str);
    PalletScene->addItem(io);
}

void _graphics::paintArbiter(int x, int y, int client_x, int client_y, char* text)
{
    QBrush br(Qt::white);
    QPen pen(Qt::black);
    PalletScene->addLine(client_x+CLIENT_RECT_WIDTH/2,client_y+CLIENT_RECT_HEIGHT,x+ACTOR_CIRCLE_RADIUS/2,y,pen);

    PalletScene->addEllipse(x,y,ACTOR_CIRCLE_RADIUS,ACTOR_CIRCLE_RADIUS,pen,br);
    QGraphicsTextItem * io = new QGraphicsTextItem;
    io->setPos(x,y);
    io->setPlainText(text);
    PalletScene->addItem(io);
}

void _graphics::clear()
{
    PalletScene->clear();
    paintServer();
}

void _graphics::paintServer()
{
    server_x=PalletScene->width()/2-SERVER_RECT_WIDTH/2;
    server_y=SERVER_RECT_HEIGHT;
    QBrush br(Qt::white);
    QPen pen(Qt::black);
    PalletScene->addRect(server_x,server_y,SERVER_RECT_WIDTH,SERVER_RECT_HEIGHT,pen,br);
    QGraphicsTextItem* io = new QGraphicsTextItem;
    io->setPos(server_x,server_y);
    io->setPlainText("SERVER");
    PalletScene->addItem(io);
}

void _graphics::paintTraceObject(int x, int y, int arbiter_x, int arbiter_y, char *text, int type)
{
    QBrush br(Qt::white);
    QPen pen;
    //PalletScene->addLine(server_x+SERVER_RECT_WIDTH/2,server_y+SERVER_RECT_HEIGHT,x+CLIENT_RECT_WIDTH/2,y,pen);

    switch(type)
    {
        case 0:
            pen.setColor(Qt::red);
            break;
        case 1:
            pen.setColor(Qt::green);
            break;
        case 2:
            pen.setColor(Qt::blue);
            break;
    }
    if(x!=0 && y!=0)
    {
        PalletScene->addRect(x,y,CLIENT_RECT_WIDTH*2,CLIENT_RECT_HEIGHT,pen, br);
        QGraphicsTextItem* io = new QGraphicsTextItem;
        io->setPos(x,y);
        io->setPlainText(text);
        PalletScene->addItem(io);
    }
    else   // Нет арбитра - когда инициализация
    {
        int message_x=x;
        int message_y=y;
        int init_x=0;
        int init_y=20;

        PalletScene->addRect(message_x,message_y,CLIENT_RECT_WIDTH*2,CLIENT_RECT_HEIGHT,pen, br);
        QGraphicsTextItem* io2 = new QGraphicsTextItem;
        io2->setPos(message_x,message_y);
        io2->setPlainText(text);
        PalletScene->addItem(io2);

        pen.setColor(Qt::black);
        PalletScene->addRect(init_x,init_y,CLIENT_RECT_WIDTH*2,CLIENT_RECT_HEIGHT,pen, br);
        QGraphicsTextItem* io3 = new QGraphicsTextItem;
        io3->setPos(init_x,init_y);
        io3->setPlainText("createAndInitActors");
        PalletScene->addItem(io3);

        //PalletScene->addLine(init_x+CLIENT_RECT_WIDTH/2,init_y+CLIENT_RECT_HEIGHT,message_x+CLIENT_RECT_WIDTH/2,message_y,pen);
    }
}

void _graphics::showClientDisconnectedError()
{
    textEdit->append("ERROR: Client disconnected");
}
