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

template <typename T>
void connectKnobToMember(KnobControl* knob, T& memberVar, QObject* parent, std::function<void()> onUpdate = nullptr) {
    QObject::connect(knob, &KnobControl::knobChanged, parent, [&memberVar, onUpdate](double oldValue, double newValue) {
        memberVar = newValue;  // Update the member variable

        if (onUpdate) {
            onUpdate();  // Call optional onUpdate function if provided
        }
    });
}

int calculateNoteIndex(int keyboardOctaveIndex, int noteIndex) {
    auto octaveIndex = (7 - keyboardOctaveIndex) + 1;
    auto activeIndex = noteIndex + (octaveIndex * 12);

    return activeIndex;
}

KnobControl * MainWindow::createKnob(QVBoxLayout ** layout,
                                    double minimum,
                                    double maximum,
                                    double initialValue,
                                    QString labelText,
                                    int decimalPlaces,
                                    bool log,
                                    bool useFormatter,
                                    QString suffixText) {

    KnobControl *knob = new KnobControl;
    QLabel *lblTitle = createLabel(labelText, true, 12);

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


    connect(knob, &KnobControl::knobChanged, this, [=](double oldValue, double newValue) {
        showKnobTooltip(knob, decimalPlaces, log, useFormatter, suffixText);
    });

    connect(knob, &KnobControl::hoverEntered, this, [=]() {
        showKnobTooltip(knob, decimalPlaces, log, useFormatter, suffixText);
    });

    connect(knob, &KnobControl::hoverLeft, this, [=]() {
        m_tooltip.hide();
    });

    return knob;
}

QLabel * MainWindow::createLabel(QString text, bool bold, int pointSize) {
    QLabel * label = new QLabel(text);

    QPalette pal = label->palette();
    pal.setColor(QPalette::WindowText, Qt::white);
    label->setPalette(pal);

    QFont font("Afacad Flux", pointSize);
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

    QFont font = QFont("Afacad Flux", pointSize, 700);
    gb->setFont(font);

    gb->setStyleSheet(tr("QGroupBox { color: white;  }"));
    return gb;
}

QComboBox * MainWindow::createComboBox(QStringList items) {
    QComboBox *cbo = new QComboBox;

    cbo->addItems(items);

    QFont font("Afacad Flux", 10);
    cbo->setFont(font);

    return cbo;
}

QVBoxLayout *MainWindow::createModLayout() {
    QVBoxLayout *vbModGroup = new QVBoxLayout;

    QVBoxLayout * waveformVerticalLayout = new QVBoxLayout;
    QLabel *waveformLabel = createLabel(tr("SHAPE"), true, 11);

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
    auto modFrequencyKnob = createKnob(&frequencyKnobLayout, 0, 10, m_modFrequency, tr("FREQ"), 1);
    connectKnobToMember(modFrequencyKnob, m_modFrequency, this, std::bind(&MainWindow::updateVoices, this));
    vbModGroup->addLayout(frequencyKnobLayout);

    QVBoxLayout *modMixLayout;
    auto modMixKnob = createKnob(&modMixLayout, 0, 100, m_osc1ModMix, tr("OSC1 TREMOLO"));
    connectKnobToMember(modMixKnob, m_osc1ModMix, this, std::bind(&MainWindow::updateVoices, this));
    vbModGroup->addLayout(modMixLayout);


    QVBoxLayout *modMix2Layout;
    auto modMix2Knob = createKnob(&modMix2Layout, 0, 100, m_osc2ModMix, tr("OSC2 TREMOLO"));
    connectKnobToMember(modMix2Knob, m_osc2ModMix, this, std::bind(&MainWindow::updateVoices, this));
    vbModGroup->addLayout(modMix2Layout);

    return vbModGroup;
}

void MainWindow::showKnobTooltip(const KnobControl* knob, int decimalPlaces, bool logarithmic, bool useFormatter, QString suffix) {

    auto value = logarithmic ? std::pow(2, knob->value()) : knob->value();
    auto tooltipText = QString("%1 %2").arg((useFormatter ? formatNumberPrefix(value) : QString::number(value, 'f', decimalPlaces)), suffix);
    auto tooltipPoint = knob->centerPoint() + QPoint(-5, -20);

    m_tooltip.showTooltip(tooltipText, tooltipPoint, 55000);

}

void MainWindow::showTooltip(const QPoint &point, QString toolTipText)
{
    m_tooltip.showTooltip(toolTipText, point, 25000);
}

QVBoxLayout* MainWindow::createOsc1Layout() {
    QVBoxLayout *vbOsc1Group = new QVBoxLayout;

    QGridLayout *osc1Grid = new QGridLayout;
    vbOsc1Group->addLayout(osc1Grid);

    QVBoxLayout * waveformVerticalLayout = new QVBoxLayout;
    QLabel *osc1WaveformLabel = createLabel(tr("WAVEFORM"), true, 11);

    auto waveformCombo = createComboBox({"Sine", "Square", "Saw", "Triangle"});
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
    connectKnobToMember(detuneKnob, m_osc1Detune, this, std::bind(&MainWindow::updateVoices, this));
    osc1Grid->addLayout(detuneKnobLayout, 1, 0, Qt::AlignHCenter | Qt::AlignTop);


    // // /////////////////////////////////////////////////
    // // /

    waveformVerticalLayout = new QVBoxLayout;
    QLabel *osc1IntervalLabel = createLabel(tr("INTERVAL"), true, 11);

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
    auto mixKnob = createKnob(&mixKnobLayout, 0, 100, m_osc1Mix, "MIX");
    connectKnobToMember(mixKnob, m_osc1Mix, this, std::bind(&MainWindow::updateVoices, this));
    osc1Grid->addLayout(mixKnobLayout, 1, 1, Qt::AlignHCenter | Qt::AlignTop);

    vbOsc1Group->setAlignment(Qt::AlignTop);

    return vbOsc1Group;
}

QVBoxLayout* MainWindow::createOsc2Layout() {
    QVBoxLayout *vbOscillator = new QVBoxLayout;

    QGridLayout *gridOscillator= new QGridLayout;
    vbOscillator->addLayout(gridOscillator);

    QVBoxLayout * waveformVerticalLayout = new QVBoxLayout;
    QLabel *oscWaveformLabel = createLabel(tr("WAVEFORM"), true, 11);

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
    connectKnobToMember(detuneKnob, m_osc2Detune, this, std::bind(&MainWindow::updateVoices, this));
    gridOscillator->addLayout(detuneKnobLayout, 1, 0, Qt::AlignHCenter | Qt::AlignTop);

    // // /////////////////////////////////////////////////
    // // /

    QVBoxLayout *intervalVerticalLayout = new QVBoxLayout;
    QLabel *labelInterval = createLabel(tr("INTERVAL"), true, 11);


    auto intervalCombo = createComboBox({"16'", "8'", "4'"});
    intervalCombo->setCurrentIndex(m_osc2IntervalIndex);


    connect(intervalCombo, &QComboBox::currentIndexChanged, this, [=](int index){
        // Handle the selected index
        m_osc2IntervalIndex = index;
        updateVoices();
    });


    intervalVerticalLayout->addWidget(labelInterval);
    intervalVerticalLayout->addWidget(intervalCombo);

    labelInterval->setContentsMargins(0, 0, 0, 0);
    intervalCombo->setContentsMargins(0, 0, 0, 0);

    gridOscillator->addLayout(intervalVerticalLayout, 0, 1, Qt::AlignHCenter | Qt::AlignTop);

    QVBoxLayout * mixKnobLayout;
    auto mixKnob= createKnob(&mixKnobLayout, 0, 100, m_osc2Mix, "MIX");
    connectKnobToMember(mixKnob, m_osc2Mix, this, std::bind(&MainWindow::updateVoices, this));
    gridOscillator->addLayout(mixKnobLayout, 1, 1, Qt::AlignHCenter | Qt::AlignTop);

    vbOscillator->setAlignment(Qt::AlignTop);

    return vbOscillator;
}

QVBoxLayout * MainWindow::createFilterLayout() {

    QVBoxLayout *vbFilterGroup = new QVBoxLayout;

    QVBoxLayout* knobLayout;
    auto cutoffKnob = createKnob(&knobLayout, log2(20), log2(20000), m_filterCutoff, tr("CUTOFF"), 0, true, true, "hz");
    connectKnobToMember(cutoffKnob, m_filterCutoff, this,  std::bind(&MainWindow::updateVoices, this));
    vbFilterGroup->addLayout(knobLayout);

    auto knobResonance = createKnob(&knobLayout, 0, 20, m_filterQ, tr("Q"), 1);
    connectKnobToMember(knobResonance, m_filterQ, this, std::bind(&MainWindow::updateVoices, this));
    vbFilterGroup->addLayout(knobLayout);

    auto knobFilterMod = createKnob(&knobLayout, 0, 100, m_filterMod, tr("MOD"));
    connectKnobToMember(knobFilterMod, m_filterMod, this, std::bind(&MainWindow::updateVoices, this));
    vbFilterGroup->addLayout(knobLayout);

    auto knobFilterEnv = createKnob(&knobLayout, 0, 100, m_filterEnv, tr("ENV"));
    connectKnobToMember(knobFilterEnv, m_filterEnv, this, std::bind(&MainWindow::updateVoices, this));
    vbFilterGroup->addLayout(knobLayout);

    return vbFilterGroup;
}

QHBoxLayout *MainWindow::createFilterEnvelopeLayout() {
    QHBoxLayout *qhbFilterEnvelope = new QHBoxLayout;


    QVBoxLayout* knobLayout;
    auto knobFilterEnvelopeA = createKnob(&knobLayout, 0, 100, m_filterEnvelopeA, tr("ATTACK"));
    connectKnobToMember(knobFilterEnvelopeA, m_filterEnvelopeA, this, std::bind(&MainWindow::updateVoices, this));
    qhbFilterEnvelope->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);

    auto knobFilterEnvelopeD = createKnob(&knobLayout, 0, 100, m_filterEnvelopeD, tr("DECAY"));
    connectKnobToMember(knobFilterEnvelopeD, m_filterEnvelopeD, this, std::bind(&MainWindow::updateVoices, this));
    qhbFilterEnvelope->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);

    auto knobFilterEnvelopeS = createKnob(&knobLayout, 0, 100, m_filterEnvelopeS , tr("SUSTAIN"));
    connectKnobToMember(knobFilterEnvelopeS, m_filterEnvelopeS, this, std::bind(&MainWindow::updateVoices, this));
    qhbFilterEnvelope->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);

    auto knobFilterEnvelopeR = createKnob(&knobLayout, 0, 100, m_filterEnvelopeR , tr("RELEASE"));
    connectKnobToMember(knobFilterEnvelopeR, m_filterEnvelopeR, this, std::bind(&MainWindow::updateVoices, this));
    qhbFilterEnvelope->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);
    return qhbFilterEnvelope;
}

QHBoxLayout *MainWindow::createVolumeEnvelopeLayout() {
    QHBoxLayout *qhbVolumeEnvelope = new QHBoxLayout;
    QVBoxLayout* knobLayout;

    auto knobVolumeEnvelopeA = createKnob(&knobLayout, 0, 100, m_volumeEnvelopeA, tr("ATTACK"));
    connectKnobToMember(knobVolumeEnvelopeA, m_volumeEnvelopeA, this, std::bind(&MainWindow::updateVoices, this));
    qhbVolumeEnvelope->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);

    auto knobVolumeEnvelopeD = createKnob(&knobLayout, 0, 100, m_volumeEnvelopeD, tr("DECAY"));
    connectKnobToMember(knobVolumeEnvelopeD, m_volumeEnvelopeD, this, std::bind(&MainWindow::updateVoices, this));
    qhbVolumeEnvelope->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);

    auto knobVolumeEnvelopeS = createKnob(&knobLayout, 0, 100, m_volumeEnvelopeS, tr("SUSTAIN"));
    connectKnobToMember(knobVolumeEnvelopeS, m_volumeEnvelopeS, this, std::bind(&MainWindow::updateVoices, this));
    qhbVolumeEnvelope->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);

    auto knobVolumeEnvelopeR = createKnob(&knobLayout, 0, 100, m_volumeEnvelopeR, tr("RELEASE"));
    connectKnobToMember(knobVolumeEnvelopeR, m_volumeEnvelopeR, this, std::bind(&MainWindow::updateVoices, this));
    qhbVolumeEnvelope->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);

    return qhbVolumeEnvelope;
}

QHBoxLayout *MainWindow::createMasterLayout() {
    QHBoxLayout *qhbMaster = new QHBoxLayout;

    QVBoxLayout* knobLayout;
    auto knobDrive = createKnob(&knobLayout, 0, 100, m_overdrive , tr("DRIVE"));
    connectKnobToMember(knobDrive, m_overdrive, this, std::bind(&MainWindow::updateVoices, this));
    qhbMaster->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);


    auto knobReverb = createKnob(&knobLayout, 0, 100, m_reverb, tr("REVERB"));
    connectKnobToMember(knobReverb, m_reverb, this, std::bind(&MainWindow::updateVoices, this));
    qhbMaster->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);


    auto knobVolume = createKnob(&knobLayout, 0, 100, m_volume, tr("VOLUME"));
    connectKnobToMember(knobVolume, m_volume, this, std::bind(&MainWindow::updateVoices, this));
    qhbMaster->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);

    QVBoxLayout *dropdownLayout = new QVBoxLayout;
    QComboBox *midiInCombo = new QComboBox;
    QComboBox *octaveCombo = createComboBox({"+3", "+2", "+1", "NORMAL", "-1", "-2", "-3"});

    octaveCombo->setCurrentIndex(m_keyboardOctaveIndex);

    connect(octaveCombo, &QComboBox::currentIndexChanged, this, [=](int index){
        // Handle the selected index
        m_keyboardOctaveIndex = index;
        updateVoices();
    });

    auto midiLabel = createLabel(tr("MIDI IN"), true, 11);
    midiLabel->setAlignment(Qt::AlignTop);
    dropdownLayout->addWidget(midiLabel);
    dropdownLayout->addWidget(midiInCombo);

    dropdownLayout->addItem(new QSpacerItem(0, 15));

    auto octaveLabel = createLabel(tr("OCTAVE"), true, 11);
    octaveLabel->setAlignment(Qt::AlignTop);
    dropdownLayout->addWidget(octaveLabel);
    dropdownLayout->addWidget(octaveCombo);

    dropdownLayout->setAlignment(Qt::AlignTop);

    qhbMaster->addLayout(dropdownLayout);

    return qhbMaster;
}

QWidget *MainWindow::createKeyboardWidget() {
    QWidget* keyBox = new QWidget;
    keyBox->setFixedSize(910, 220);
    keyBox->setContentsMargins(0, 0, 0, 0);

    std::vector<QString> noteNames = {"C", "C#", "D", "Eb", "E", "F", "F#", "G", "G#", "A", "Bb", "B"};

    QString whiteStyleSheet = QString(R"(
                QPushButton {
                    background-color: white; border: 1px solid black;
                    border-bottom-right-radius: 5px; border-bottom-left-radius: 5px;
                }
                QPushButton:hover { background: lightgray; }
                QPushButton:pressed {
                    background: #777777;
                    padding-top: 2px;
                }
    )");

    keyBox->setStyleSheet(whiteStyleSheet);

    QHBoxLayout* whiteKeysLayout = new QHBoxLayout;
    whiteKeysLayout->setSpacing(0);
    whiteKeysLayout->setContentsMargins(0, 0, 0, 0);

    const std::vector<int> whiteNoteIndexes = {0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 17, 19, 21, 23};
    const std::vector<int> blackNoteIndexes = {1, 3, 6, 8, 10, 13, 15, 18, 20, 22};

    for (int i = 0; i < 14; ++i) {

        auto key = new PianoKey("", keyBox);
        whiteKeysLayout->addWidget(key);

        QLabel *label = new QLabel(noteNames[whiteNoteIndexes[i] % 12]);
        label->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);

        QVBoxLayout *layout = new QVBoxLayout(key);
        layout->addWidget(label);

        QFont font("Afacad Flux");
        font.setBold(true);
        label->setFont(font);

        connect(key, &QPushButton::pressed, this, [=]() {
            noteOn(calculateNoteIndex(m_keyboardOctaveIndex, whiteNoteIndexes[i]));
        });
    }

    QWidget* blackKeysWidget = new QWidget(keyBox);
    blackKeysWidget->setFixedSize(910, 130);  // Black keys only take up 150px height
    blackKeysWidget->move(0, 10);  // Positioned absolutely on top of the white keys

    QString blackStyleSheet = QString(R"(
            QPushButton {
                background-color: black;
                border-bottom-right-radius: 5px;
                border-bottom-left-radius: 5px;
            }

            QPushButton:hover {
                background-color: #555;
            }

            QPushButton:pressed {
                background: #777777;
                padding-top: 2px;
            }

    )");

    blackKeysWidget->setStyleSheet(blackStyleSheet);
    QString blackKeyStyle = "margin: 0px; padding: 0px; color: white;";

    // List of X positions for each black key relative to the keyBox
    const int spacing = 60;
    const int left = 75;
    QVector<int> blackKeyPositions = {left, left + spacing, left + spacing * 3, left + spacing * 4, left + spacing * 5, left + spacing * 7, left + spacing * 8, left + spacing * 10, left + spacing * 11, left + spacing * 12};

    // Manually position black keys at precise locations
    for (int i = 0; i < blackKeyPositions.size(); ++i) {

        BlackKey* blackKey = new BlackKey(blackKeyStyle, blackKeysWidget);
        blackKey->setFixedSize(40, 130);  // Same size as before
        blackKey->move(blackKeyPositions[i], 0);  // Set the exact X position

        QLabel *label = new QLabel(noteNames[blackNoteIndexes[i] % 12]);
        label->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);

        QFont font("Afacad Flux");
        font.setBold(true);
        label->setFont(font);

        QVBoxLayout *layout = new QVBoxLayout(blackKey);
        layout->addWidget(label);

        connect(blackKey, &QPushButton::pressed, this, [=]() {
            noteOn(calculateNoteIndex(m_keyboardOctaveIndex, blackNoteIndexes[i]));
        });

    }

    QVBoxLayout* pianoLayout = new QVBoxLayout(keyBox);
    pianoLayout->addLayout(whiteKeysLayout);
    pianoLayout->setAlignment(Qt::AlignHCenter);

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

    gridWidget->setObjectName("gridWidget");
    gridWidget->setStyleSheet("#gridWidget { background-color: black; }");

    mainLayout->addWidget(gridWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Add widgets and layouts to the grid
    QGroupBox *gbMod = createGroupBox(tr("MOD"));
    QGroupBox *gbOsc1 = createGroupBox(tr("OSC1"));
    QGroupBox *gbOsc2 = createGroupBox(tr("OSC2"));
    QGroupBox *gbFilter = createGroupBox(tr("FILTER"));
    QGroupBox *gbFilterEnv = createGroupBox(tr("FILTER ENVELOPE"));
    QGroupBox *gbVolumeEnv = createGroupBox(tr("VOLUME ENVELOPE"));
    QGroupBox *gbMaster = createGroupBox(tr("MASTER"));

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

    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->setAlignment(Qt::AlignTop);
    gridLayout->setContentsMargins(15, 15, 15, 15);

    QWidget* pianoWidget = createKeyboardWidget();  // This will be the function where you create your piano

    QVBoxLayout *pianoLayout = new QVBoxLayout;
    QWidget* pianoHolderWidget = new QWidget;
    pianoHolderWidget->setContentsMargins(0, 0, 0, 0);
    pianoHolderWidget->setFixedWidth(1024);

    pianoLayout->setAlignment(Qt::AlignHCenter);

    pianoLayout->addWidget(pianoWidget);

    pianoHolderWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    pianoHolderWidget->setLayout(pianoLayout);

    pianoHolderWidget->setObjectName("pianoHolder");
    pianoHolderWidget->setStyleSheet("#pianoHolder { background-color: #333; }");

    mainLayout->addWidget(pianoHolderWidget);
}


void MainWindow::updateVoices() {
    qDebug() << "m_filterCutoff" << m_filterCutoff;
    qDebug() << "m_modFrequency" << m_modFrequency;
    qDebug() << "m_osc1ModMix" << m_osc1ModMix;
    qDebug() << "m_osc2ModMix" << m_osc2ModMix;
    qDebug() << "m_filterEnv" << m_filterEnv;
}

void MainWindow::noteOn(int noteIndex) {

    qDebug() << "NoteOn => Note Index: " << noteIndex;
    qDebug() << "Frequency: " << 440.0 * std::pow(2.0, (noteIndex - 69) / 12.0);
}

void MainWindow::noteOff(int noteIndex) {

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
