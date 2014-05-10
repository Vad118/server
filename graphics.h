#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <QtWidgets>
#include <QtWidgets/QTextEdit>
#include <QPainter>
#include <string>

enum{CLIENT_RECT_WIDTH=50,CLIENT_RECT_HEIGHT=20,ACTOR_CIRCLE_RADIUS=25};

class _graphics
{
    QTextEdit *textEdit;
    int width,height;
public:
    QGraphicsScene *PalletScene;
    _graphics(QTextEdit *edit,int width, int height);
    ~_graphics();
    void TextEditAppend(const char *str);
    void paintClient(int x, int y, char* text);
    void paintArbiter(int x, int y, char* text);
    int getHeight(){return height;}
    int getWidth(){return width;}
    void clear();

};

#endif // GRAPHICS_H
