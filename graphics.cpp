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

void _graphics::paintClient(int x, int y, string text)
{
    QBrush br(Qt::green);
    QPen pen(Qt::black);
    PalletScene->addRect(x,y,CLIENT_RECT_WIDTH,CLIENT_RECT_HEIGHT,pen, br);
}

void _graphics::paintArbiter(int x, int y, string text)
{
    QBrush br(Qt::red);
    QPen pen(Qt::black);
    PalletScene->addEllipse(x,y,ACTOR_CIRCLE_RADIUS,ACTOR_CIRCLE_RADIUS,pen,br);
}

void _graphics::clear()
{
    PalletScene->clear();
}
