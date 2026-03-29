#include "mainwindow_cable.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

MainWindow_Cable::MainWindow_Cable(QWidget* parent)
    : QMainWindow(parent)
{
    auto* panel = new PatchPanelWidget(this);
    setCentralWidget(panel);

    panel->addOutputJack("Keyboard CV Out", QRect(40, 40, 150, 36), QColor(180, 70, 70));
    panel->addInputJack("VCO 1V/Oct", QRect(330, 40, 150, 36), QColor(70, 120, 180));
    panel->addInputJack("VCF Cutoff CV", QRect(330, 100, 150, 36), QColor(70, 160, 100));

    panel->addOutputJack("LFO Out", QRect(40, 160, 150, 36), QColor(180, 140, 60));
    panel->addInputJack("PWM In", QRect(330, 160, 150, 36), QColor(140, 90, 180));

    resize(560, 280);
    setWindowTitle("Modular Patch Panel UI Test");
}

QWidget* MainWindow_Cable::createModularPanel()
{
    auto* holder = new QWidget;
    auto* layout = new QVBoxLayout(holder);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    //auto* title = createLabel("PATCH PANEL", true, 12);
    //layout->addWidget(title);

    patch_panel_ = new PatchPanelWidget;
    patch_panel_->setMinimumHeight(260);

    const QColor cvColor(70, 140, 200);
    const QColor audioColor(180, 70, 70);

    keyboard_cv_out_ = patch_panel_->addOutputJack(
        "CV Out",
        QRect(20, 30, 120, 28),
        cvColor);

    vco_mod_in_ = patch_panel_->addInputJack(
        "Mod",
        QRect(220, 30, 120, 28),
        cvColor);

    vco_out_ = patch_panel_->addOutputJack(
        "Out",
        QRect(220, 90, 120, 28),
        audioColor);

    output_in_ = patch_panel_->addInputJack(
        "In",
        QRect(420, 60, 120, 28),
        audioColor);

    layout->addWidget(patch_panel_);

    return holder;
}

void MainWindow_Cable::noteOn(int noteIndex)
{
    qDebug() << "NoteOn => Note Index:" << noteIndex;

    if (!isPatched(keyboard_cv_out_, vco_mod_in_)) {
        qDebug() << "Keyboard CV not patched to VCO Mod";
        return;
    }

    if (!isPatched(vco_out_, output_in_)) {
        qDebug() << "VCO Out not patched to Output In";
        return;
    }

    const double frequency = noteIndexToFrequency(noteIndex);
    qDebug() << "Frequency:" << frequency;

    //auto voice = voices_.begin();
    //if (voice == voices_.end()) {
    //    return;
    //}

    //(*voice)->updateOscillator1Frequency(static_cast<float>(frequency));
    //(*voice)->noteOn();
}

bool MainWindow_Cable::isPatched(const OutputJack* out, const InputJack* in) const
{
    return out != nullptr && in != nullptr && in->source() == out;
}

double MainWindow_Cable::noteIndexToFrequency(int noteIndex) const
{
    return 440.0 * std::pow(2.0, (noteIndex - 69) / 12.0);
}
