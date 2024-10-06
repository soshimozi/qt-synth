#ifndef BLACKKEY_H
#define BLACKKEY_H

#include <QPushButton>

class BlackKey : public QPushButton
{
public:
    BlackKey(const QString& style, QWidget* parent = nullptr)
        : QPushButton(parent) {
        setStyleSheet(style);
        setFixedSize(40, 125); // Size for black keys
    }
};

#endif // BLACKKEY_H
