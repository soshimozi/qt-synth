#include "mainwindow.h"
#include "blackkey.h"
#include "definitions.h"
#include "whitekey.h"
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

    knob->setSpriteSheet(spritesheet_);  // Set the sprite sheet
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
        knob_tooltip_.hide();
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
    waveformCombo->setCurrentIndex(static_cast<int>(mod_waveform_));

    connect(waveformCombo, &QComboBox::currentIndexChanged, this, [=](int index){
        mod_waveform_ = static_cast<wave_shape>(index);
        updateVoices();
    });

    waveformVerticalLayout->addWidget(waveformLabel);
    waveformVerticalLayout->addWidget(waveformCombo);
    waveformLabel->setContentsMargins(0, 0, 0, 0);
    waveformCombo->setContentsMargins(0, 0, 0, 0);
    waveformVerticalLayout->setAlignment(Qt::AlignVCenter);

    vbModGroup->addLayout(waveformVerticalLayout);

    QVBoxLayout* frequencyKnobLayout;
    auto modFrequencyKnob = createKnob(&frequencyKnobLayout, 0, 10, mod_frequency_, tr("FREQ"), 1);
    connectKnobToMember(modFrequencyKnob, mod_frequency_, this, std::bind(&MainWindow::updateVoices, this));
    vbModGroup->addLayout(frequencyKnobLayout);

    QVBoxLayout *modMixLayout;
    auto modMixKnob = createKnob(&modMixLayout, 0, 100, osc_1_mod_mix_, tr("OSC1 TREMOLO"));
    connectKnobToMember(modMixKnob, osc_1_mod_mix_, this, std::bind(&MainWindow::updateVoices, this));
    vbModGroup->addLayout(modMixLayout);


    QVBoxLayout *modMix2Layout;
    auto modMix2Knob = createKnob(&modMix2Layout, 0, 100, osc_2_mod_mix_, tr("OSC2 TREMOLO"));
    connectKnobToMember(modMix2Knob, osc_2_mod_mix_, this, std::bind(&MainWindow::updateVoices, this));
    vbModGroup->addLayout(modMix2Layout);

    return vbModGroup;
}

void MainWindow::showKnobTooltip(const KnobControl* knob, int decimalPlaces, bool logarithmic, bool useFormatter, QString suffix) {

    auto value = logarithmic ? std::pow(2, knob->value()) : knob->value();
    auto tooltipText = QString("%1 %2").arg((useFormatter ? formatNumberPrefix(value) : QString::number(value, 'f', decimalPlaces)), suffix);
    auto tooltipPoint = knob->centerPoint() + QPoint(-5, -20);

    knob_tooltip_.showTooltip(tooltipText, tooltipPoint, 55000);

}

void MainWindow::showTooltip(const QPoint &point, QString toolTipText)
{
    knob_tooltip_.showTooltip(toolTipText, point, 25000);
}

QVBoxLayout* MainWindow::createOsc1Layout() {
    QVBoxLayout *vbOsc1Group = new QVBoxLayout;

    QGridLayout *osc1Grid = new QGridLayout;
    vbOsc1Group->addLayout(osc1Grid);

    QVBoxLayout * waveformVerticalLayout = new QVBoxLayout;
    QLabel *osc1WaveformLabel = createLabel(tr("WAVEFORM"), true, 11);

    auto waveformCombo = createComboBox({"Sine", "Square", "Saw", "Triangle"});
    waveformCombo->setCurrentIndex(static_cast<int>(osc_1_waveform_));

    connect(waveformCombo, &QComboBox::currentIndexChanged, this, [=](int index){
        osc_1_waveform_ = static_cast<wave_shape>(index);
        updateVoices();
    });

    waveformVerticalLayout->addWidget(osc1WaveformLabel);
    waveformVerticalLayout->addWidget(waveformCombo);

    osc1WaveformLabel->setContentsMargins(0, 0, 0, 0);
    waveformCombo->setContentsMargins(0, 0, 0, 0);

    osc1Grid->addLayout(waveformVerticalLayout, 0, 0, Qt::AlignHCenter | Qt::AlignTop);

    QVBoxLayout * detuneKnobLayout;
    auto detuneKnob = createKnob(&detuneKnobLayout, -1200, 1200, osc_1_detune_, "DETUNE");
    connectKnobToMember(detuneKnob, osc_1_detune_, this, std::bind(&MainWindow::updateVoices, this));
    osc1Grid->addLayout(detuneKnobLayout, 1, 0, Qt::AlignHCenter | Qt::AlignTop);


    // // /////////////////////////////////////////////////
    // // /

    waveformVerticalLayout = new QVBoxLayout;
    QLabel *osc1IntervalLabel = createLabel(tr("INTERVAL"), true, 11);

    auto intervalCombo = createComboBox({"32'", "16'", "8'"});
    intervalCombo->setCurrentIndex(osc_1_interval_);

    connect(intervalCombo, &QComboBox::currentIndexChanged, this, [=](int index){
        // Handle the selected index
        osc_1_interval_ = index;
        updateVoices();
    });

    waveformVerticalLayout->addWidget(osc1IntervalLabel);
    waveformVerticalLayout->addWidget(intervalCombo);

    osc1IntervalLabel->setContentsMargins(0, 0, 0, 0);
    intervalCombo->setContentsMargins(0, 0, 0, 0);
    osc1Grid->addLayout(waveformVerticalLayout, 0, 1, Qt::AlignHCenter | Qt::AlignTop);

    QVBoxLayout * mixKnobLayout;
    auto mixKnob = createKnob(&mixKnobLayout, 0, 100, osc_1_mix_, "MIX");
    connectKnobToMember(mixKnob, osc_1_mix_, this, std::bind(&MainWindow::updateVoices, this));
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
    waveformCombo->setCurrentIndex(static_cast<int>(osc_2_waveform_));

    connect(waveformCombo, &QComboBox::currentIndexChanged, this, [=](int index){
        // Handle the selected index
        osc_2_waveform_ = static_cast<wave_shape>(index);
        updateVoices();
    });


    waveformVerticalLayout->addWidget(oscWaveformLabel);
    waveformVerticalLayout->addWidget(waveformCombo);

    oscWaveformLabel->setContentsMargins(0, 0, 0, 0);
    waveformCombo->setContentsMargins(0, 0, 0, 0);

    gridOscillator->addLayout(waveformVerticalLayout, 0, 0, Qt::AlignHCenter | Qt::AlignTop);

    QVBoxLayout * detuneKnobLayout;
    auto detuneKnob = createKnob(&detuneKnobLayout, -1200, 1200, osc_2_detune_, "DETUNE");
    connectKnobToMember(detuneKnob, osc_2_detune_, this, std::bind(&MainWindow::updateVoices, this));
    gridOscillator->addLayout(detuneKnobLayout, 1, 0, Qt::AlignHCenter | Qt::AlignTop);

    // // /////////////////////////////////////////////////
    // // /

    QVBoxLayout *intervalVerticalLayout = new QVBoxLayout;
    QLabel *labelInterval = createLabel(tr("INTERVAL"), true, 11);


    auto intervalCombo = createComboBox({"16'", "8'", "4'"});
    intervalCombo->setCurrentIndex(osc_2_interval_);


    connect(intervalCombo, &QComboBox::currentIndexChanged, this, [=](int index){
        // Handle the selected index
        osc_2_interval_ = index;
        updateVoices();
    });


    intervalVerticalLayout->addWidget(labelInterval);
    intervalVerticalLayout->addWidget(intervalCombo);

    labelInterval->setContentsMargins(0, 0, 0, 0);
    intervalCombo->setContentsMargins(0, 0, 0, 0);

    gridOscillator->addLayout(intervalVerticalLayout, 0, 1, Qt::AlignHCenter | Qt::AlignTop);

    QVBoxLayout * mixKnobLayout;
    auto mixKnob= createKnob(&mixKnobLayout, 0, 100, osc_2_mix_, "MIX");
    connectKnobToMember(mixKnob, osc_2_mix_, this, std::bind(&MainWindow::updateVoices, this));
    gridOscillator->addLayout(mixKnobLayout, 1, 1, Qt::AlignHCenter | Qt::AlignTop);

    vbOscillator->setAlignment(Qt::AlignTop);

    return vbOscillator;
}

QVBoxLayout * MainWindow::createFilterLayout() {

    QVBoxLayout *vbFilterGroup = new QVBoxLayout;

    QVBoxLayout* knobLayout;
    auto cutoffKnob = createKnob(&knobLayout, log2(20), log2(20000), filter_cutoff_, tr("CUTOFF"), 0, true, true, "hz");
    connectKnobToMember(cutoffKnob, filter_cutoff_, this,  std::bind(&MainWindow::updateVoices, this));
    vbFilterGroup->addLayout(knobLayout);

    auto knobResonance = createKnob(&knobLayout, 0, 20, filter_resonance_, tr("Q"), 1);
    connectKnobToMember(knobResonance, filter_resonance_, this, std::bind(&MainWindow::updateVoices, this));
    vbFilterGroup->addLayout(knobLayout);

    auto knobFilterMod = createKnob(&knobLayout, 0, 100, filter_mod_, tr("MOD"));
    connectKnobToMember(knobFilterMod, filter_mod_, this, std::bind(&MainWindow::updateVoices, this));
    vbFilterGroup->addLayout(knobLayout);

    auto knobFilterEnv = createKnob(&knobLayout, 0, 100, filter_envelope_, tr("ENV"));
    connectKnobToMember(knobFilterEnv, filter_envelope_, this, std::bind(&MainWindow::updateVoices, this));
    vbFilterGroup->addLayout(knobLayout);

    return vbFilterGroup;
}

QHBoxLayout *MainWindow::createFilterEnvelopeLayout() {
    QHBoxLayout *qhbFilterEnvelope = new QHBoxLayout;


    QVBoxLayout* knobLayout;
    auto knobFilterEnvelopeA = createKnob(&knobLayout, 0, 100, filter_envelope_a_, tr("ATTACK"));
    connectKnobToMember(knobFilterEnvelopeA, filter_envelope_a_, this, std::bind(&MainWindow::updateVoices, this));
    qhbFilterEnvelope->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);

    auto knobFilterEnvelopeD = createKnob(&knobLayout, 0, 100, filter_envelope_d_, tr("DECAY"));
    connectKnobToMember(knobFilterEnvelopeD, filter_envelope_d_, this, std::bind(&MainWindow::updateVoices, this));
    qhbFilterEnvelope->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);

    auto knobFilterEnvelopeS = createKnob(&knobLayout, 0, 100, filter_envelope_s_ , tr("SUSTAIN"));
    connectKnobToMember(knobFilterEnvelopeS, filter_envelope_s_, this, std::bind(&MainWindow::updateVoices, this));
    qhbFilterEnvelope->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);

    auto knobFilterEnvelopeR = createKnob(&knobLayout, 0, 100, filter_envelope_r_ , tr("RELEASE"));
    connectKnobToMember(knobFilterEnvelopeR, filter_envelope_r_, this, std::bind(&MainWindow::updateVoices, this));
    qhbFilterEnvelope->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);
    return qhbFilterEnvelope;
}

QHBoxLayout *MainWindow::createVolumeEnvelopeLayout() {
    QHBoxLayout *qhbVolumeEnvelope = new QHBoxLayout;
    QVBoxLayout* knobLayout;

    auto knobVolumeEnvelopeA = createKnob(&knobLayout, 0, 100, volume_envelope_a_, tr("ATTACK"));
    connectKnobToMember(knobVolumeEnvelopeA, volume_envelope_a_, this, std::bind(&MainWindow::updateVoices, this));
    qhbVolumeEnvelope->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);

    auto knobVolumeEnvelopeD = createKnob(&knobLayout, 0, 100, volume_envelope_d_, tr("DECAY"));
    connectKnobToMember(knobVolumeEnvelopeD, volume_envelope_d_, this, std::bind(&MainWindow::updateVoices, this));
    qhbVolumeEnvelope->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);

    auto knobVolumeEnvelopeS = createKnob(&knobLayout, 0, 100, volume_envelope_s_, tr("SUSTAIN"));
    connectKnobToMember(knobVolumeEnvelopeS, volume_envelope_s_, this, std::bind(&MainWindow::updateVoices, this));
    qhbVolumeEnvelope->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);

    auto knobVolumeEnvelopeR = createKnob(&knobLayout, 0, 100, volume_envelope_r_, tr("RELEASE"));
    connectKnobToMember(knobVolumeEnvelopeR, volume_envelope_r_, this, std::bind(&MainWindow::updateVoices, this));
    qhbVolumeEnvelope->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);

    return qhbVolumeEnvelope;
}

QHBoxLayout *MainWindow::createMasterLayout() {
    QHBoxLayout *qhbMaster = new QHBoxLayout;

    QVBoxLayout* knobLayout;
    auto knobDrive = createKnob(&knobLayout, 0, 100, overdrive_ , tr("DRIVE"));
    connectKnobToMember(knobDrive, overdrive_, this, std::bind(&MainWindow::updateVoices, this));
    qhbMaster->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);


    auto knobReverb = createKnob(&knobLayout, 0, 100, reverb_, tr("REVERB"));
    connectKnobToMember(knobReverb, reverb_, this, std::bind(&MainWindow::updateVoices, this));
    qhbMaster->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);


    auto knobVolume = createKnob(&knobLayout, 0, 100, volume_, tr("VOLUME"));
    connectKnobToMember(knobVolume, volume_, this, std::bind(&MainWindow::updateVoices, this));
    qhbMaster->addLayout(knobLayout);
    knobLayout->setAlignment(Qt::AlignVCenter);

    QVBoxLayout *dropdownLayout = new QVBoxLayout;
    QComboBox *midiInCombo = new QComboBox;
    QComboBox *octaveCombo = createComboBox({"+3", "+2", "+1", "NORMAL", "-1", "-2", "-3"});

    octaveCombo->setCurrentIndex(keyboard_octave_offset_);

    connect(octaveCombo, &QComboBox::currentIndexChanged, this, [=](int index){
        // Handle the selected index
        keyboard_octave_offset_ = index;
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
            noteOn(calculateNoteIndex(keyboard_octave_offset_, whiteNoteIndexes[i]));
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
            noteOn(calculateNoteIndex(keyboard_octave_offset_, blackNoteIndexes[i]));
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
    qDebug() << "m_filterCutoff" << filter_cutoff_;
    qDebug() << "m_modFrequency" << mod_frequency_;
    qDebug() << "m_osc1ModMix" << osc_1_mod_mix_;
    qDebug() << "m_osc2ModMix" << osc_2_mod_mix_;
    qDebug() << "m_filterEnv" << filter_envelope_;
}

void MainWindow::noteOn(int noteIndex) {

    qDebug() << "NoteOn => Note Index: " << noteIndex;
    qDebug() << "Frequency: " << 440.0 * std::pow(2.0, (noteIndex - 69) / 12.0);

    // TODO: find an empty voice slot
    // initialize that voice with current parameters
    // call noteOn for that voice
    // if there are no empty slots, note steal from first slot
}

void MainWindow::noteOff(int noteIndex) {

}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , audio_player_(nullptr, SAMPLE_RATE, FRAMES)
    , audio_context_(SAMPLE_RATE, FRAMES)
    , output_node_(audio_context_, 0.5)
    , playing_(false) /*, m_voice(wave_shape::sine, wave_shape::sine, wave_shape::sine, 4, .1, .1, 440, 232.24, .5, .5, 0, 0)*/
{

    for(uint i = 0; i < 32; i++) {
        voices_.push_back(new VoiceNode(audio_context_));
    }

    //m_voice.connect(&m_masterGain);

    spritesheet_ = std::make_shared<SpriteSheet>();
    spritesheet_->setOrientation(Qt::Vertical);
    spritesheet_->setCells(101);  // Number of cells (frames)
    spritesheet_->setSource(":/images/LittlePhatty.png");  // Make sure to have a valid image here

    try {
        buildLayout();
    } catch(const std::exception& ex) {

    }

    auto voice = voices_.begin();

    (*voice)->setParameters(VoiceNode::Builder(audio_context_)
                              .setModFrequency(5)
                              .setOscillator1Frequency(440)
                              .setOscillator2Frequency(440 * std::pow(2, -1))
                              .setOscillator1Gain(.9)
                              .setOscillator2Gain(.6)
                              .setOscillator1ModGain(.1)
                              .setOscillator2ModGain(.1)
                              .setVolumeEnvelopeA(1.1)
                              .setVolumeEnvelopeD(.1)
                              .setVolumeEnvelopeS(1.0)
                              .setVolumeEnvelopeR(1.5)
                              .parameters());

    (*voice)->noteOn();

    // m_voice.setParameters(VoiceNode::Builder()
    //                           .setModFrequency(5)
    //                           .setOscillator1Frequency(440)
    //                           .setOscillator2Frequency(440 * std::pow(2, -1))
    //                           .setOscillator1Gain(.6)
    //                           .setOscillator2Gain(.4)
    //                           .setOscillator1ModGain(.4)
    //                           .setOscillator2ModGain(.1).parameters());

    (*voice)->connect(&output_node_);

    static int sample_count = 0;

    // Set the callback for the audio player
    audio_player_.setCallback([this](const void* user_data, float* output, unsigned long frames_per_buffer) {

        sample_count += frames_per_buffer;

        output_node_.process(frames_per_buffer, last_processing_id++);  // Process the signal chain
        float* gainBuffer = output_node_.buffer();  // Get the processed buffer

        // Copy the buffer to the output
        for (unsigned long i = 0; i < frames_per_buffer; ++i) {
            output[i * 2] = gainBuffer[i];      // Left channel
            output[i * 2 + 1] = gainBuffer[i];  // Right channel (duplicate for stereo)
        }

        if(sample_count > 88200) {
             auto voice = voices_.begin();
             //(*voice)->setParameters(VoiceNode::Builder().parameters());
             (*voice)->noteOff();
         }
    });

    // Initialize and start the audio player
    if (!audio_player_.initializeStream()) {
        qDebug() << "Failed to initialize audio stream!";
    }

    if (!audio_player_.start()) {
        qDebug() << "Failed to start audio stream!";
    }

    // auto osc = std::make_shared<OscillatorNode>();

    // osc->setFrequency(440);
    // osc->connect(m_masterGain.get());
}
