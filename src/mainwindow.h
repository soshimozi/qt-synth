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
#include "gainnode.h"
#include "knobcontrol.h"
#include "spritesheet.h"
#include "tooltip.h"
#include "voice.h"

typedef std::function<void(double, double)> FnChange;
typedef std::function<QString(double)> FnFormat;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() { m_audioPlayer.stop(); }


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
    std::shared_ptr<SpriteSheet> m_spritesheet;

    std::unique_ptr<Voice> m_voices[255] = {};

    //std::unique_ptr<AudioPlayer> m_audioPlayer;
    AudioPlayer m_audioPlayer;
    GainNode m_masterGain;
    bool m_playing;

    // values for new voices
    double m_modFrequency = 2.1;
    int m_modShapeIndex = 0;

    double m_osc1ModMix = 15;
    double m_osc2ModMix = 17;

    int m_osc1WaveformIndex = 2;
    int m_osc2WaveformIndex = 2;

    double m_osc1Detune = 0;
    double m_osc2Detune = 0;

    int m_osc1IntervalIndex = 0;
    int m_osc2IntervalIndex = 0;

    double m_osc1Mix = 50;
    double m_osc2Mix = 50;

    double m_filterCutoff = 8;
    double m_filterQ = 7.0;
    double m_filterMod = 21;
    double m_filterEnv = 56;

    double m_filterEnvelopeA = 5;
    double m_filterEnvelopeD = 6;
    double m_filterEnvelopeS = 5;
    double m_filterEnvelopeR = 7;

    double m_volumeEnvelopeA = 2;
    double m_volumeEnvelopeD = 15;
    double m_volumeEnvelopeS = 68;
    double m_volumeEnvelopeR = 5;

    double m_overdrive = 38;
    double m_reverb = 32;
    double m_volume = 75;

    int m_keyboardOctaveIndex = 3;

    ToolTip m_tooltip;

    // volume sets masterGain in real time.
    // since there is only one masterGain
    // at any time we don't need to cache the value

};
#endif // MAINWINDOW_H
