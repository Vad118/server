#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <QtWidgets/QTextEdit>

class _graphics
{
    QTextEdit *textEdit;
public:
    _graphics(QTextEdit *edit);
    void TextEditAppend(const char *str);
};

#endif // GRAPHICS_H
