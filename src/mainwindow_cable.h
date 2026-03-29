#pragma once

#include "patchpanelwidget.h"
#include <QMainWindow>

class MainWindow_Cable : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow_Cable(QWidget* parent = nullptr);

private:
    PatchPanelWidget* patch_panel_ = nullptr;

    OutputJack* keyboard_cv_out_ = nullptr;
    InputJack* vco_mod_in_ = nullptr;
    OutputJack* vco_out_ = nullptr;
    InputJack* output_in_ = nullptr;

private:
    QWidget* createModularPanel();
    bool isPatched(const OutputJack* out, const InputJack* in) const;
    double noteIndexToFrequency(int noteIndex) const;
    void noteOn(int noteIndex);
    void noteOff(int noteIndex);

};
