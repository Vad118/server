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
    QMutex mutex;
    mutex.lock();
    QBrush br(Qt::white);
    QPen pen(Qt::black);
    PalletScene->addLine(server_x+SERVER_RECT_WIDTH/2,server_y+SERVER_RECT_HEIGHT,x+CLIENT_RECT_WIDTH/2,y,pen);
    PalletScene->addRect(x,y,CLIENT_RECT_WIDTH,CLIENT_RECT_HEIGHT,pen, br);

    QGraphicsTextItem* io = new QGraphicsTextItem;
    io->setPos(x,y);
    io->setPlainText(str);
    //QMetaObject::invokeMethod(PalletScene,"addRect",Q_ARG(QGraphicsItem*,io));
    //QMetaObject::invokeMethod(PalletScene,"addItem",Q_ARG(QGraphicsItem*,io));
    PalletScene->addItem(io);
    mutex.unlock();
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
