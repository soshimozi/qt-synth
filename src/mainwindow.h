#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>

//#include "audioplayer.h"
#include "audioplayer.h"
#include "definitions.h"
#include "gainnode.h"
#include "knobcontrol.h"
#include "spritesheet.h"
#include "tooltip.h"
#include "voicenode.h"

typedef std::function<void(double, double)> FnChange;
typedef std::function<QString(double)> FnFormat;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() { audio_player_.stop(); }


protected:
    void buildLayout();

    QLabel * createLabel(QString text, bool bold, int pointSize = -1);
    KnobControl* createKnob(QVBoxLayout ** layout,
                            double minimum,
                            double maximum,
                            double initialValue,
                            QString labelText,
                            int decimalPlaces = 0,
                            bool log = false,
                            bool useFormatter = false,
                            QString suffixText = ""
                            );

    QGroupBox * createGroupBox(QString title, int pointSize = -1);

    QComboBox * createComboBox(QStringList items);

    QVBoxLayout * createFilterLayout();

    QVBoxLayout * createModLayout();

    QVBoxLayout * createOsc1Layout();
    QVBoxLayout * createOsc2Layout();

    QHBoxLayout  * createFilterEnvelopeLayout();
    QHBoxLayout  * createVolumeEnvelopeLayout();
    QHBoxLayout  * createMasterLayout();
    QWidget  * createKeyboardWidget();

private slots:

protected:
    void updateVoices();

    void noteOn(int nodeIndex);
    void noteOff(int nodeIndex);

    void showTooltip(const QPoint &point, QString toolTipText);
    void showKnobTooltip(const KnobControl* knob, int decimalPlaces = 0, bool logarithmic = false, bool useFormatter = false, QString suffix = "");

private:
    std::shared_ptr<SpriteSheet> spritesheet_;

    //std::unique_ptr<AudioPlayer> audio_player_;
    AudioPlayer audio_player_;
    GainNode output_node_;
    bool playing_;

    // values for new voices
    double mod_frequency_ = 2.1;
    wave_shape mod_waveform_ = wave_shape::sine;

    double osc_1_mod_mix_ = 15;
    double osc_2_mod_mix_ = 17;

    wave_shape osc_1_waveform_ = wave_shape::sine;
    wave_shape osc_2_waveform_ = wave_shape::sine;

    double osc_1_detune_ = 0;
    double osc_2_detune_ = 0;

    int osc_1_interval_ = 0;
    int osc_2_interval_ = 0;

    double osc_1_mix_ = 50;
    double osc_2_mix_ = 50;

    double filter_cutoff_ = 8;
    double filter_resonance_ = 7.0;
    double filter_mod_ = 21;
    double filter_envelope_ = 56;

    double filter_envelope_a_ = 5;
    double filter_envelope_d_ = 6;
    double filter_envelope_s_ = 5;
    double filter_envelope_r_ = 7;

    double volume_envelope_a_ = 2;
    double volume_envelope_d_ = 15;
    double volume_envelope_s_ = 68;
    double volume_envelope_r_ = 5;

    double overdrive_ = 38;
    double reverb_ = 32;
    double volume_ = 75;

    int keyboard_octave_offset_ = 3;

    ToolTip knob_tooltip_;

    //VoiceNode m_voice[16] { VoiceNode };

    std::vector<VoiceNode *> voices_;

    // volume sets masterGain in real time.
    // since there is only one masterGain
    // at any time we don't need to cache the value

    AudioContext audio_context_;

};
#endif // MAINWINDOW_H
