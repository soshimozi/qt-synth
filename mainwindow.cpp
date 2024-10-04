#include "mainwindow.h"
#include "blackkey.h"
#include "pianokey.h"
#include "spritesheet.h"
#include "knobcontrol.h"
#include "helpers.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QRadioButton>
#include <QComboBox>

static int last_processing_id = 0;

KnobControl * MainWindow::createKnob(QVBoxLayout ** layout, double minimum, double maximum, double initialValue, QString labelText) {

    KnobControl *knob = new KnobControl;
    QLabel *lblTitle = createLabel(labelText, true);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->setSizeConstraint(QLayout::SetFixedSize);
    vbox->setContentsMargins(1, 1, 1, 1);
    vbox->setSpacing(1);

    knob->setSpriteSheet(m_spritesheet);  // Set the sprite sheet
    knob->setMinimum(minimum);  // Set minimum value
    knob->setMaximum(maximum);  // Set maximum value
    knob->setValue(initialValue);  // Initial value

    knob->setFixedSize(64, 64);

    lblTitle->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    vbox->addWidget(knob, 0, Qt::AlignHCenter);
    vbox->addWidget(lblTitle, 0, Qt::AlignHCenter);

    *layout = vbox;
    return knob;
}

QLabel * MainWindow::createLabel(QString text, bool bold, int pointSize) {
    QLabel * label = new QLabel(text);

    QPalette pal = label->palette();
    pal.setColor(QPalette::WindowText, Qt::white);
    label->setPalette(pal);

    QFont font("Syncopate", pointSize);
    label->setFont(font);

    if (bold) {
        QFont font = label->font();
        font.setBold(true);
        label->setFont(font);
    }

    return label;
}

QGroupBox * MainWindow::createGroupBox(QString title, int pointSize) {
    QGroupBox *gb = new QGroupBox(title);

    QFont font = QFont("Syncopate", pointSize, 700);
    gb->setFont(font);

    gb->setStyleSheet(tr("QGroupBox { color: white;  }"));
    return gb;
}

QComboBox * MainWindow::createComboBox(QStringList items) {
    QComboBox *cbo = new QComboBox;

    cbo->addItems(items);

    QFont font("Syncopate", 10);
    cbo->setFont(font);

    return cbo;
}

QVBoxLayout *MainWindow::createModLayout() {
    QVBoxLayout *vbModGroup = new QVBoxLayout;

    QVBoxLayout * waveformVerticalLayout = new QVBoxLayout;
    QLabel *waveformLabel = createLabel(tr("SHAPE"), true);

    waveformVerticalLayout->setSpacing(0);

    QStringList waveforms = {"Sine", "Square", "Saw", "Triangle"};
    auto waveformCombo = createComboBox(waveforms);
    waveformCombo->setCurrentIndex(m_modShapeIndex);

    connect(waveformCombo, &QComboBox::currentIndexChanged, this, [=](int index){
        m_modShapeIndex = index;
        updateVoices();
    });

    waveformVerticalLayout->addWidget(waveformLabel);
    waveformVerticalLayout->addWidget(waveformCombo);

    waveformLabel->setContentsMargins(0, 0, 0, 0);
    waveformCombo->setContentsMargins(0, 0, 0, 0);

    waveformVerticalLayout->setAlignment(Qt::AlignVCenter);

    vbModGroup->addLayout(waveformVerticalLayout);

    QVBoxLayout* frequencyKnobLayout;

    auto modFrequencyKnob = createKnob(&frequencyKnobLayout, 0, 10, m_modFrequency, tr("FREQ"));

    QObject::connect(modFrequencyKnob, &KnobControl::knobChanged, this, [=](double oldValue, double newValue) {

        auto tooltipPoint = modFrequencyKnob->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(newValue, 'f', 1)), tooltipPoint, 25000);

        m_modFrequency = newValue;
        updateVoices();
    });

    QObject::connect(modFrequencyKnob, &KnobControl::hoverEntered, this, [=]() {
        auto value = modFrequencyKnob->value();

        auto tooltipPoint = modFrequencyKnob->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(value, 'f', 1)), tooltipPoint, 25000);
    });

    QObject::connect(modFrequencyKnob, &KnobControl::hoverLeft, this, [=]() {
        m_tooltip.hide();
    });

    vbModGroup->addLayout(frequencyKnobLayout);

    QVBoxLayout *modMixLayout;
    auto modMixKnob = createKnob(&modMixLayout, 0, 100, m_osc1ModMix, tr("OSC1 TREMOLO"));
    vbModGroup->addLayout(modMixLayout);

    QObject::connect(modMixKnob, &KnobControl::knobChanged, this, [=](double oldValue, double newValue) {

        auto tooltipPoint = modMixKnob->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(newValue, 'f', 0)), tooltipPoint, 25000);

        m_osc1ModMix = newValue;
        updateVoices();
    });

    QObject::connect(modMixKnob, &KnobControl::hoverEntered, this, [=]() {
        auto value = modMixKnob->value();

        auto tooltipPoint = modMixKnob->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(value, 'f', 0)), tooltipPoint, 25000);
    });

    QObject::connect(modMixKnob, &KnobControl::hoverLeft, this, [=]() {
        m_tooltip.hide();
    });


    QVBoxLayout *modMix2Layout;
    auto modMix2Knob = createKnob(&modMix2Layout, 0, 100, m_osc2ModMix, tr("OSC2 TREMOLO"));
    vbModGroup->addLayout(modMix2Layout);

    QObject::connect(modMix2Knob, &KnobControl::knobChanged, this, [=](double oldValue, double newValue) {

        auto tooltipPoint = modMix2Knob->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(newValue, 'f', 0)), tooltipPoint, 25000);

        m_osc2ModMix = newValue;
        updateVoices();
    });

    QObject::connect(modMix2Knob, &KnobControl::hoverEntered, this, [=]() {
        auto value = modMix2Knob->value();

        auto tooltipPoint = modMix2Knob->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(value, 'f', 0)), tooltipPoint, 25000);
    });

    QObject::connect(modMix2Knob, &KnobControl::hoverLeft, this, [=]() {
        m_tooltip.hide();
    });

    return vbModGroup;
}

QVBoxLayout* MainWindow::createOsc1Layout() {
    QVBoxLayout *vbOsc1Group = new QVBoxLayout;

    QGridLayout *osc1Grid = new QGridLayout;
    vbOsc1Group->addLayout(osc1Grid);

    QVBoxLayout * waveformVerticalLayout = new QVBoxLayout;
    QLabel *osc1WaveformLabel = createLabel(tr("WAVEFORM"), true);

    auto waveformCombo = createComboBox({"Sine", "Square", "Saw", "Triange"});
    waveformCombo->setCurrentIndex(m_osc1WaveformIndex);

    connect(waveformCombo, &QComboBox::currentIndexChanged, this, [=](int index){
        m_osc1WaveformIndex = index;
        updateVoices();
    });

    waveformVerticalLayout->addWidget(osc1WaveformLabel);
    waveformVerticalLayout->addWidget(waveformCombo);

    osc1WaveformLabel->setContentsMargins(0, 0, 0, 0);
    waveformCombo->setContentsMargins(0, 0, 0, 0);

    osc1Grid->addLayout(waveformVerticalLayout, 0, 0, Qt::AlignHCenter | Qt::AlignTop);

    QVBoxLayout * detuneKnobLayout;
    auto detuneKnob = createKnob(&detuneKnobLayout, -1200, 1200, m_osc1Detune, "DETUNE");
    osc1Grid->addLayout(detuneKnobLayout, 1, 0, Qt::AlignHCenter | Qt::AlignTop);

    QObject::connect(detuneKnob, &KnobControl::knobChanged, this, [=](double oldValue, double newValue) {
        auto tooltipPoint = detuneKnob->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(newValue, 'f', 0)), tooltipPoint, 25000);

        m_osc1Detune = newValue;
        updateVoices();
    });

    QObject::connect(detuneKnob, &KnobControl::hoverEntered, this, [=]() {
        auto value = detuneKnob->value();

        auto tooltipPoint = detuneKnob->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(value, 'f', 0)), tooltipPoint, 25000);
    });

    QObject::connect(detuneKnob, &KnobControl::hoverLeft, this, [=]() {
        m_tooltip.hide();
    });


    // // /////////////////////////////////////////////////
    // // /

    waveformVerticalLayout = new QVBoxLayout;
    QLabel *osc1IntervalLabel = createLabel(tr("INTERVAL"), true);

    auto intervalCombo = createComboBox({"32'", "16'", "8'"});
    intervalCombo->setCurrentIndex(m_osc1IntervalIndex);

    connect(intervalCombo, &QComboBox::currentIndexChanged, this, [=](int index){
        // Handle the selected index
        m_osc1IntervalIndex = index;
        updateVoices();
    });

    waveformVerticalLayout->addWidget(osc1IntervalLabel);
    waveformVerticalLayout->addWidget(intervalCombo);

    osc1IntervalLabel->setContentsMargins(0, 0, 0, 0);
    intervalCombo->setContentsMargins(0, 0, 0, 0);
    osc1Grid->addLayout(waveformVerticalLayout, 0, 1, Qt::AlignHCenter | Qt::AlignTop);

    QVBoxLayout * mixKnobLayout;
    auto mixKnob = createKnob(&mixKnobLayout, 0, 100, m_osc1Mix, "Mix");
    osc1Grid->addLayout(mixKnobLayout, 1, 1, Qt::AlignHCenter | Qt::AlignTop);

    QObject::connect(mixKnob, &KnobControl::knobChanged, this, [=](double oldValue, double newValue) {
        auto tooltipPoint = mixKnob->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString::number(newValue, 'f', 0), tooltipPoint, 25000);

        m_osc1Mix = newValue;
        updateVoices();
    });


    QObject::connect(mixKnob, &KnobControl::hoverEntered, this, [=]() {
        auto value = mixKnob->value();

        auto tooltipPoint = mixKnob->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString::number(value, 'f', 0), tooltipPoint, 25000);
    });

    QObject::connect(mixKnob, &KnobControl::hoverLeft, this, [=]() {
        m_tooltip.hide();
    });

    vbOsc1Group->setAlignment(Qt::AlignTop);

    return vbOsc1Group;
}

QVBoxLayout* MainWindow::createOsc2Layout() {
    QVBoxLayout *vbOscillator = new QVBoxLayout;

    QGridLayout *gridOscillator= new QGridLayout;
    vbOscillator->addLayout(gridOscillator);

    QVBoxLayout * waveformVerticalLayout = new QVBoxLayout;
    QLabel *oscWaveformLabel = createLabel(tr("WAVEFORM"), false);

    auto waveformCombo = createComboBox({"Sine", "Square", "Saw", "Triangle"});
    waveformCombo->setCurrentIndex(m_osc2WaveformIndex);

    connect(waveformCombo, &QComboBox::currentIndexChanged, this, [=](int index){
        // Handle the selected index
        m_osc2WaveformIndex = index;
        updateVoices();
    });


    waveformVerticalLayout->addWidget(oscWaveformLabel);
    waveformVerticalLayout->addWidget(waveformCombo);

    oscWaveformLabel->setContentsMargins(0, 0, 0, 0);
    waveformCombo->setContentsMargins(0, 0, 0, 0);

    gridOscillator->addLayout(waveformVerticalLayout, 0, 0, Qt::AlignHCenter | Qt::AlignTop);

    QVBoxLayout * detuneKnobLayout;

    auto detuneKnob = createKnob(&detuneKnobLayout, -1200, 1200, m_osc2Detune, "DETUNE");

    QObject::connect(detuneKnob, &KnobControl::knobChanged, this, [=](double oldValue, double newValue) {
        auto tooltipPoint = detuneKnob->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(newValue, 'f', 0)), tooltipPoint, 25000);

        m_osc2Detune = newValue;
        updateVoices();
    });

    QObject::connect(detuneKnob, &KnobControl::hoverEntered, this, [=]() {
        auto value = detuneKnob->value();

        auto tooltipPoint = detuneKnob->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(value, 'f', 0)), tooltipPoint, 25000);
    });

    QObject::connect(detuneKnob, &KnobControl::hoverLeft, this, [=]() {
        m_tooltip.hide();
    });



    gridOscillator->addLayout(detuneKnobLayout, 1, 0, Qt::AlignHCenter | Qt::AlignTop);

    // // /////////////////////////////////////////////////
    // // /

    QVBoxLayout *intervalVerticalLayout = new QVBoxLayout;
    QLabel *labelInterval = createLabel(tr("INTERVAL"), false);


    auto intervalCombo = createComboBox({"16'", "8'", "4'"});
    intervalCombo->setCurrentIndex(m_osc2IntervalIndex);


    connect(intervalCombo, &QComboBox::currentIndexChanged, this, [=](int index){
        // Handle the selected index
        m_osc2IntervalIndex = index;
    });


    intervalVerticalLayout->addWidget(labelInterval);
    intervalVerticalLayout->addWidget(intervalCombo);

    labelInterval->setContentsMargins(0, 0, 0, 0);
    intervalCombo->setContentsMargins(0, 0, 0, 0);

    gridOscillator->addLayout(intervalVerticalLayout, 0, 1, Qt::AlignHCenter | Qt::AlignTop);

    QVBoxLayout * mixKnobLayout;
    auto mixKnob= createKnob(&mixKnobLayout, 0, 100, m_osc2Mix, "Mix");

    QObject::connect(mixKnob, &KnobControl::knobChanged, this, [=](double oldValue, double newValue) {
        auto tooltipPoint = mixKnob->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(newValue, 'f', 0)), tooltipPoint, 25000);

        m_osc2Mix = newValue;
        updateVoices();
    });

    QObject::connect(mixKnob, &KnobControl::hoverEntered, this, [=]() {
        auto value = mixKnob->value();

        auto tooltipPoint = mixKnob->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(value, 'f', 0)), tooltipPoint, 25000);
    });

    QObject::connect(mixKnob, &KnobControl::hoverLeft, this, [=]() {
        m_tooltip.hide();
    });


    gridOscillator->addLayout(mixKnobLayout, 1, 1, Qt::AlignHCenter | Qt::AlignTop);

    vbOscillator->setAlignment(Qt::AlignTop);

    return vbOscillator;
}

QVBoxLayout * MainWindow::createFilterLayout() {

    QVBoxLayout *vbFilterGroup = new QVBoxLayout;

    QVBoxLayout* knobLayout;
    auto cutoffKnob = createKnob(&knobLayout, log2(20), log2(20000), m_filterCutoff, tr("Cutoff"));
    vbFilterGroup->addLayout(knobLayout);

    QObject::connect(cutoffKnob, &KnobControl::knobChanged, this, [=](double oldValue, double newValue) {

        auto toolTipText = QString("%1 hz").arg(formatNumberPrefix(std::pow(2, newValue)));
                               //.arg(QString::number(std::pow(2, newValue), 'f', 0));

        auto tooltipPoint = cutoffKnob->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(toolTipText, tooltipPoint, 25000);

        m_filterCutoff = newValue;
        updateVoices();
    });

    QObject::connect(cutoffKnob, &KnobControl::hoverEntered, this, [=]() {
        auto value = cutoffKnob->value();
        auto tooltipText = QString("%1 hz").arg(formatNumberPrefix(std::pow(2, value)));

        auto tooltipPoint = cutoffKnob->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(tooltipText, tooltipPoint, 25000);
    });

    QObject::connect(cutoffKnob, &KnobControl::hoverLeft, this, [=]() {
        m_tooltip.hide();
    });


    auto knobResonance = createKnob(&knobLayout, 0, 20, m_filterQ, tr("Q"));
    vbFilterGroup->addLayout(knobLayout);

    QObject::connect(knobResonance, &KnobControl::knobChanged, this, [=](double oldValue, double newValue) {
        auto tooltipPoint = knobResonance->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(newValue, 'f', 0)), tooltipPoint, 25000);

        m_filterQ = newValue;
        updateVoices();
    });

    QObject::connect(knobResonance, &KnobControl::hoverEntered, this, [=]() {
        auto value = knobResonance->value();

        auto tooltipPoint = knobResonance->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(value, 'f', 0)), tooltipPoint, 25000);
    });

    QObject::connect(knobResonance, &KnobControl::hoverLeft, this, [=]() {
        m_tooltip.hide();
    });

    auto knobFilterMod = createKnob(&knobLayout, 0, 100, m_filterMod, tr("Mod"));
    vbFilterGroup->addLayout(knobLayout);

    QObject::connect(knobFilterMod, &KnobControl::knobChanged, this, [=](double oldValue, double newValue) {
        auto tooltipPoint = knobFilterMod->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(newValue, 'f', 0)), tooltipPoint, 25000);

        m_filterMod = newValue;
        updateVoices();
    });

    QObject::connect(knobFilterMod, &KnobControl::hoverEntered, this, [=]() {
        auto value = knobFilterMod->value();

        auto tooltipPoint = knobFilterMod->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(value, 'f', 0)), tooltipPoint, 25000);
    });

    QObject::connect(knobFilterMod, &KnobControl::hoverLeft, this, [=]() {
        m_tooltip.hide();
    });

    auto knobFilterEnv = createKnob(&knobLayout, 0, 100, m_filterEnv, tr("Env"));
    vbFilterGroup->addLayout(knobLayout);

    QObject::connect(knobFilterEnv, &KnobControl::knobChanged, this, [=](double oldValue, double newValue) {
        auto tooltipPoint = knobFilterEnv->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(newValue, 'f', 0)), tooltipPoint, 25000);

        m_filterQ = newValue;
        updateVoices();
    });


    QObject::connect(knobFilterEnv, &KnobControl::hoverEntered, this, [=]() {
        auto value = knobFilterEnv->value();

        auto tooltipPoint = knobFilterEnv->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(value, 'f', 0)), tooltipPoint, 25000);
    });

    QObject::connect(knobFilterEnv, &KnobControl::hoverLeft, this, [=]() {
        m_tooltip.hide();
    });

    return vbFilterGroup;
}

QHBoxLayout *MainWindow::createFilterEnvelopeLayout() {
    QHBoxLayout *qhbFilterEnvelope = new QHBoxLayout;


    QVBoxLayout* knobLayout;
    auto knobFilterEnvelopeA = createKnob(&knobLayout, 0, 100, m_filterEnvelopeA, tr("ATTACK"));
    qhbFilterEnvelope->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);

    QObject::connect(knobFilterEnvelopeA, &KnobControl::knobChanged, this, [=](double oldValue, double newValue) {
        auto tooltipPoint = knobFilterEnvelopeA->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(newValue, 'f', 0)), tooltipPoint, 25000);

        m_filterEnvelopeA = newValue;
        updateVoices();
    });

    QObject::connect(knobFilterEnvelopeA, &KnobControl::hoverEntered, this, [=]() {
        auto value = knobFilterEnvelopeA->value();

        auto tooltipPoint = knobFilterEnvelopeA->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(value, 'f', 0)), tooltipPoint, 25000);
    });

    QObject::connect(knobFilterEnvelopeA, &KnobControl::hoverLeft, this, [=]() {
        m_tooltip.hide();
    });

    auto knobFilterEnvelopeD = createKnob(&knobLayout, 0, 100, m_filterEnvelopeD, tr("DECAY"));
    qhbFilterEnvelope->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);

    QObject::connect(knobFilterEnvelopeD, &KnobControl::knobChanged, this, [=](double oldValue, double newValue) {
        auto tooltipPoint = knobFilterEnvelopeD->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(newValue, 'f', 0)), tooltipPoint, 25000);

        m_filterEnvelopeD = newValue;
        updateVoices();
    });

    QObject::connect(knobFilterEnvelopeD, &KnobControl::hoverEntered, this, [=]() {
        auto value = knobFilterEnvelopeD->value();

        auto tooltipPoint = knobFilterEnvelopeD->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(value, 'f', 0)), tooltipPoint, 25000);
    });

    QObject::connect(knobFilterEnvelopeD, &KnobControl::hoverLeft, this, [=]() {
        m_tooltip.hide();
    });


    auto knobFilterEnvelopeS = createKnob(&knobLayout, 0, 100, m_filterEnvelopeS , tr("SUSTAIN"));
    qhbFilterEnvelope->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);

    QObject::connect(knobFilterEnvelopeS, &KnobControl::knobChanged, this, [=](double oldValue, double newValue) {
        auto tooltipPoint = knobFilterEnvelopeS->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(newValue, 'f', 0)), tooltipPoint, 25000);

        m_filterEnvelopeS = newValue;
        updateVoices();
    });

    QObject::connect(knobFilterEnvelopeS, &KnobControl::hoverEntered, this, [=]() {
        auto value = knobFilterEnvelopeS->value();

        auto tooltipPoint = knobFilterEnvelopeS->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(value, 'f', 0)), tooltipPoint, 25000);
    });

    QObject::connect(knobFilterEnvelopeS, &KnobControl::hoverLeft, this, [=]() {
        m_tooltip.hide();
    });

    auto knobFilterEnvelopeR = createKnob(&knobLayout, 0, 100, m_filterEnvelopeR , tr("Release"));
    qhbFilterEnvelope->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);

    QObject::connect(knobFilterEnvelopeR, &KnobControl::knobChanged, this, [=](double oldValue, double newValue) {
        auto tooltipPoint = knobFilterEnvelopeR->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(newValue, 'f', 0)), tooltipPoint, 25000);

        m_filterEnvelopeR = newValue;
        updateVoices();
    });

    QObject::connect(knobFilterEnvelopeR, &KnobControl::hoverEntered, this, [=]() {
        auto value = knobFilterEnvelopeR->value();

        auto tooltipPoint = knobFilterEnvelopeR->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(value, 'f', 0)), tooltipPoint, 25000);
    });

    QObject::connect(knobFilterEnvelopeR, &KnobControl::hoverLeft, this, [=]() {
        m_tooltip.hide();
    });

    return qhbFilterEnvelope;
}

QHBoxLayout *MainWindow::createVolumeEnvelopeLayout() {
    QHBoxLayout *qhbVolumeEnvelope = new QHBoxLayout;
    QVBoxLayout* knobLayout;

    auto knobVolumeEnvelopeA = createKnob(&knobLayout, 0, 100, m_volumeEnvelopeA, tr("ATTACK"));
    qhbVolumeEnvelope->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);

    QObject::connect(knobVolumeEnvelopeA, &KnobControl::knobChanged, this, [=](double oldValue, double newValue) {
        auto tooltipPoint = knobVolumeEnvelopeA->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(newValue, 'f', 0)), tooltipPoint, 25000);

        m_volumeEnvelopeA = newValue;
        updateVoices();
    });

    QObject::connect(knobVolumeEnvelopeA, &KnobControl::hoverEntered, this, [=]() {
        auto value = knobVolumeEnvelopeA->value();

        auto tooltipPoint = knobVolumeEnvelopeA->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(value, 'f', 0)), tooltipPoint, 25000);
    });

    QObject::connect(knobVolumeEnvelopeA, &KnobControl::hoverLeft, this, [=]() {
        m_tooltip.hide();
    });

    auto knobVolumeEnvelopeD = createKnob(&knobLayout, 0, 100, m_volumeEnvelopeD, tr("Decay"));
    qhbVolumeEnvelope->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);

    QObject::connect(knobVolumeEnvelopeD, &KnobControl::knobChanged, this, [=](double oldValue, double newValue) {
        auto tooltipPoint = knobVolumeEnvelopeD->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(newValue, 'f', 0)), tooltipPoint, 25000);

        m_volumeEnvelopeD = newValue;
        updateVoices();
    });

    QObject::connect(knobVolumeEnvelopeD, &KnobControl::hoverEntered, this, [=]() {
        auto value = knobVolumeEnvelopeD->value();

        auto tooltipPoint = knobVolumeEnvelopeD->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(value, 'f', 0)), tooltipPoint, 25000);
    });

    QObject::connect(knobVolumeEnvelopeD, &KnobControl::hoverLeft, this, [=]() {
        m_tooltip.hide();
    });

    auto knobVolumeEnvelopeS = createKnob(&knobLayout, 0, 100, m_volumeEnvelopeS, tr("Sustain"));
    qhbVolumeEnvelope->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);

    QObject::connect(knobVolumeEnvelopeS, &KnobControl::knobChanged, this, [=](double oldValue, double newValue) {
        auto tooltipPoint = knobVolumeEnvelopeS->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(newValue, 'f', 0)), tooltipPoint, 25000);

        m_volumeEnvelopeS = newValue;
        updateVoices();
    });

    QObject::connect(knobVolumeEnvelopeS, &KnobControl::hoverEntered, this, [=]() {
        auto value = knobVolumeEnvelopeS->value();

        auto tooltipPoint = knobVolumeEnvelopeS->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(value, 'f', 0)), tooltipPoint, 25000);
    });

    QObject::connect(knobVolumeEnvelopeS, &KnobControl::hoverLeft, this, [=]() {
        m_tooltip.hide();
    });

    auto knobVolumeEnvelopeR = createKnob(&knobLayout, 0, 100, m_volumeEnvelopeR, tr("Release"));
    qhbVolumeEnvelope->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);

    QObject::connect(knobVolumeEnvelopeR, &KnobControl::knobChanged, this, [=](double oldValue, double newValue) {
        auto tooltipPoint = knobVolumeEnvelopeR->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(newValue, 'f', 0)), tooltipPoint, 25000);

        m_volumeEnvelopeR = newValue;
        updateVoices();
    });



    QObject::connect(knobVolumeEnvelopeR, &KnobControl::hoverEntered, this, [=]() {
        auto value = knobVolumeEnvelopeR->value();

        auto tooltipPoint = knobVolumeEnvelopeR->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(value, 'f', 0)), tooltipPoint, 25000);
    });

    QObject::connect(knobVolumeEnvelopeR, &KnobControl::hoverLeft, this, [=]() {
        m_tooltip.hide();
    });


    return qhbVolumeEnvelope;
}

QHBoxLayout *MainWindow::createMasterLayout() {
    QHBoxLayout *qhbMaster = new QHBoxLayout;

    QVBoxLayout* knobLayout;
    auto knobDrive = createKnob(&knobLayout, 0, 100, m_overdrive , tr("Drive"));
    qhbMaster->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);

    QObject::connect(knobDrive, &KnobControl::knobChanged, this, [=](double oldValue, double newValue) {

        auto tooltipPoint = knobDrive->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(newValue, 'f', 0)), tooltipPoint, 25000);

        m_overdrive = newValue;
        updateVoices();
    });

    QObject::connect(knobDrive, &KnobControl::hoverEntered, this, [=]() {
        auto value = knobDrive->value();

        auto tooltipPoint = knobDrive->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(value, 'f', 0)), tooltipPoint, 25000);
    });

    QObject::connect(knobDrive, &KnobControl::hoverLeft, this, [=]() {
        m_tooltip.hide();
    });

    auto knobReverb = createKnob(&knobLayout, 0, 100, m_reverb, tr("Reverb"));
    qhbMaster->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);

    QObject::connect(knobReverb, &KnobControl::knobChanged, this, [=](double oldValue, double newValue) {
        auto tooltipPoint = knobReverb->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(newValue, 'f', 0)), tooltipPoint, 25000);

        m_reverb = newValue;
        updateVoices();
    });

    QObject::connect(knobReverb, &KnobControl::hoverEntered, this, [=]() {
        auto value = knobReverb->value();

        auto tooltipPoint = knobReverb->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(value, 'f', 0)), tooltipPoint, 25000);
    });

    QObject::connect(knobReverb, &KnobControl::hoverLeft, this, [=]() {
        m_tooltip.hide();
    });


    auto knobVolume = createKnob(&knobLayout, 0, 100, m_volume, tr("Volume"));
    qhbMaster->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);

    QObject::connect(knobVolume, &KnobControl::knobChanged, this, [=](double oldValue, double newValue) {

        auto tooltipPoint = knobVolume->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(newValue, 'f', 0)), tooltipPoint, 25000);

        m_volume = newValue;
        updateVoices();
    });

    QObject::connect(knobVolume, &KnobControl::hoverEntered, this, [=]() {
        auto value = knobVolume->value();

        auto tooltipPoint = knobVolume->centerPoint() + QPoint(-5, -20);
        m_tooltip.showTooltip(QString("%1").arg(QString::number(value, 'f', 0)), tooltipPoint, 25000);
    });

    QObject::connect(knobVolume, &KnobControl::hoverLeft, this, [=]() {
        m_tooltip.hide();
    });

    QVBoxLayout *dropdownLayout = new QVBoxLayout;

    QComboBox *midiInCombo = new QComboBox;

    QComboBox *octaveCombo = new QComboBox;
    octaveCombo->addItem("+3");
    octaveCombo->addItem("+2");
    octaveCombo->addItem("+1");
    octaveCombo->addItem("NORMAL");
    octaveCombo->addItem("-1");
    octaveCombo->addItem("-2");
    octaveCombo->addItem("-3");

    octaveCombo->setCurrentIndex(m_keyboardOctaveIndex);

    connect(octaveCombo, &QComboBox::currentIndexChanged, this, [=](int index){
        // Handle the selected index
        m_keyboardOctaveIndex = index;
        updateVoices();
    });

    auto midiLabel = createLabel(tr("MIDI IN"), true);
    midiLabel->setAlignment(Qt::AlignTop);
    dropdownLayout->addWidget(midiLabel);
    dropdownLayout->addWidget(midiInCombo);

    dropdownLayout->addItem(new QSpacerItem(0, 15));

    auto octaveLabel = createLabel(tr("OCTAVE"), true);
    octaveLabel->setAlignment(Qt::AlignTop);
    dropdownLayout->addWidget(octaveLabel);
    dropdownLayout->addWidget(octaveCombo);

    dropdownLayout->setAlignment(Qt::AlignTop);

    qhbMaster->addLayout(dropdownLayout);

    return qhbMaster;
}

QWidget *MainWindow::createKeyboardWidget() {
    QWidget* keyBox = new QWidget;
    keyBox->setFixedSize(910, 260);
    keyBox->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout* whiteKeysLayout = new QHBoxLayout;
    whiteKeysLayout->setSpacing(0);
    whiteKeysLayout->setContentsMargins(0, 0, 0, 0);
    whiteKeysLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    QString whiteKeyStyle = "background: white; border: 1px solid black; "
                            "border-bottom-right-radius: 5px; "
                            "border-bottom-left-radius: 5px;"
                            "margin: 0px; padding: 0px;";

    for (int i = 0; i < 14; ++i) {
        auto key = new PianoKey(whiteKeyStyle, keyBox);
        whiteKeysLayout->addWidget(key);
    }

    QWidget* blackKeysWidget = new QWidget(keyBox);
    blackKeysWidget->setFixedSize(910, 150);  // Black keys only take up 150px height
    blackKeysWidget->move(0, 10);  // Positioned absolutely on top of the white keys

    QString blackKeyStyle = "background: black; border-bottom-right-radius: 5px; "
                            "border-bottom-left-radius: 5px; "
                            "margin: 0px; padding: 0px;";

    // List of X positions for each black key relative to the keyBox
    const int spacing = 60;
    const int left = 75;
    QVector<int> blackKeyPositions = {left, left + spacing, left + spacing * 3, left + spacing * 4, left + spacing * 5, left + spacing * 7, left + spacing * 8, left + spacing * 10, left + spacing * 11, left + spacing * 12};

    // Manually position black keys at precise locations
    for (int i = 0; i < blackKeyPositions.size(); ++i) {
        BlackKey* blackKey = new BlackKey(blackKeyStyle, blackKeysWidget);
        blackKey->setFixedSize(40, 150);  // Same size as before
        blackKey->move(blackKeyPositions[i], 0);  // Set the exact X position
    }

    QVBoxLayout* pianoLayout = new QVBoxLayout(keyBox);
    pianoLayout->addLayout(whiteKeysLayout);

    return keyBox;
}

void MainWindow::buildLayout() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(0);

    // Create a QGridLayout
    QGridLayout *gridLayout = new QGridLayout;
    QWidget* gridWidget = new QWidget;
    gridWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    gridWidget->setLayout(gridLayout);

    gridWidget->setObjectName("GridWidget");
    gridWidget->setStyleSheet("#GridWidget { border: 4px solid blue; border-radius: 8px; }");

    gridLayout->setContentsMargins(50, 50, 50, 50);

    mainLayout->addWidget(gridWidget);
    //mainLayout->addLayout(gridLayout);

    // auto uiSpacer = new QSpacerItem(0, 1);
    // mainLayout->addItem(uiSpacer);

    // Add widgets and layouts to the grid
    QGroupBox *gbMod = createGroupBox(tr("Mod"));
    QGroupBox *gbOsc1 = createGroupBox(tr("Osc1"));
    QGroupBox *gbOsc2 = createGroupBox(tr("Osc2"));
    QGroupBox *gbFilter = createGroupBox(tr("Filter"));
    QGroupBox *gbFilterEnv = createGroupBox(tr("Filter Envelope"));
    QGroupBox *gbVolumeEnv = createGroupBox(tr("Volume Envelope"));
    QGroupBox *gbMaster = createGroupBox(tr("Master"));

    gbMod->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    gbOsc1->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    gbOsc2->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    gbFilter->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    gbFilterEnv->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    gbVolumeEnv->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    gbMaster->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    // Column 1 (First column, spanning all 3 rows)
    gridLayout->addWidget(gbMod, 0, 0, 3, 1);
    gbMod->setLayout(createModLayout());

    gridLayout->addWidget(gbOsc1, 0, 1);
    gbOsc1->setLayout(createOsc1Layout());

    auto spacer = new QSpacerItem(0, 1, QSizePolicy::Minimum, QSizePolicy::Minimum);
    gridLayout->addItem(spacer, 1, 1);

    gridLayout->addWidget(gbOsc2, 2, 1);
    gbOsc2->setLayout(createOsc2Layout());

    gridLayout->addWidget(gbFilter, 0, 2, 3, 1);
    gbFilter->setLayout(createFilterLayout());

    gridLayout->addWidget(gbFilterEnv, 0, 3);
    gbFilterEnv->setLayout(createFilterEnvelopeLayout());

    gridLayout->addWidget(gbVolumeEnv, 1, 3);
    gbVolumeEnv->setLayout(createVolumeEnvelopeLayout());

    gridLayout->addWidget(gbMaster, 2, 3);
    gbMaster->setLayout(createMasterLayout());

    gridLayout->setContentsMargins(10, 10, 10, 10);
    gridLayout->setAlignment(Qt::AlignTop);

    QWidget* pianoWidget = createKeyboardWidget();  // This will be the function where you create your piano

    QVBoxLayout *pianoLayout = new QVBoxLayout;
    QWidget* pianoHolderWidget = new QWidget;

    pianoLayout->addWidget(pianoWidget, 0, Qt::AlignBottom);


    pianoHolderWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pianoHolderWidget->setLayout(pianoLayout);

    pianoHolderWidget->setObjectName("pianoHolder");
    pianoHolderWidget->setStyleSheet("#pianoHolder { background-color: #ccc; padding: 0; }");

    mainLayout->addWidget(pianoHolderWidget, 0, Qt::AlignHCenter);

}


void MainWindow::updateVoices() {

}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_audioPlayer(std::make_unique<AudioPlayer>(nullptr))
    , m_masterGain(std::make_unique<GainNode>(0))
    , m_playing(false)
{

    m_spritesheet = std::make_shared<SpriteSheet>();
    m_spritesheet->setOrientation(Qt::Vertical);
    m_spritesheet->setCells(101);  // Number of cells (frames)
    m_spritesheet->setSource(":/images/LittlePhatty.png");  // Make sure to have a valid image here

    buildLayout();

    // Set the callback for the audio player
    m_audioPlayer->setCallback([this](const void* user_data, float* output, unsigned long frames_per_buffer) {
        m_masterGain->process(frames_per_buffer, last_processing_id++);  // Process the signal chain
        float* gainBuffer = m_masterGain->getBuffer();  // Get the processed buffer

        // Copy the buffer to the output
        for (unsigned long i = 0; i < frames_per_buffer; ++i) {
            output[i * 2] = gainBuffer[i];      // Left channel
            output[i * 2 + 1] = gainBuffer[i];  // Right channel (duplicate for stereo)
        }
    });

    // Initialize and start the audio player
    if (!m_audioPlayer->initializeStream()) {
        qDebug() << "Failed to initialize audio stream!";
    }

    if (!m_audioPlayer->start()) {
        qDebug() << "Failed to start audio stream!";
    }
}
