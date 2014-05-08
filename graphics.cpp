#include "graphics.h"

_graphics::_graphics(QTextEdit *edit)
{
    textEdit=edit;
}

void _graphics::TextEditAppend(const char *str)
{
    textEdit->append(str);
}
