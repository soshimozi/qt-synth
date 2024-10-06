#ifndef PIANOKEY_H
#define PIANOKEY_H

#include <QPushButton>

class PianoKey : public QPushButton
{
public:
    PianoKey(const QString& style, QWidget* parent = nullptr)
        : QPushButton(parent) {
        setStyleSheet(style);

        setFixedSize(60, 208); // Size for white keys
        setFocusPolicy(Qt::StrongFocus);
    }
};

#endif // PIANOKEY_H
