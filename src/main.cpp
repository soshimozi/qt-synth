
#include "audioplayer.h"
#include "gainnode.h"
#include "lp12filternode.h"
#include "mainwindow.h"
#include "muladdnode.h"
#include "oscillatornode.h"
//#include <QApplication>
#include <iostream>
#include <limits>
#include <ostream>
#include <qapplication.h>


int showMainWindow(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;

    w.setFixedSize(1024, 768);

    QPalette pal = QPalette();

    // set black background
    // Qt::black / "#000000" / "black"
    pal.setColor(QPalette::Window, Qt::black);
    w.setAutoFillBackground(true);
    w.setPalette(pal);

    w.show();
    return a.exec();
}

int showConsole(int argc, char* argv[]) {
    AudioContext context(SAMPLE_RATE, FRAMES);

    AudioPlayer m_audioPlayer(nullptr, SAMPLE_RATE, FRAMES);

    GainNode modOscillatorGain1(context, .7);
    GainNode modOscillatorGain2(context, .3);

    GainNode filterModGain(context, .9);
    OscillatorNode oscillator1(context, wave_shape::sawtooth, 165);
    OscillatorNode oscillator2(context, wave_shape::sawtooth, 110);

    OscillatorNode lfo(context, wave_shape::sine, 2);
    MulAddNode lfoMA(context, 10, 100);
    MulAddNode filterMA(context, 900, 1000);

    LP12FilterNode filter(context, 440);
    LP12FilterNode filter2(context, 440);
    GainNode gainVolume(context, .5);
    MixerNode filter_mixer(context);

    GainNode oscillator1Gain(context, .4);
    GainNode oscillator2Gain(context, .4);

    GainNode envelopeVolume(context, .1);

    ADSRNode adsr(context, .2, .1, .8, .1);

    lfo.connect(&lfoMA);
    lfoMA.connect(&modOscillatorGain1);
    lfoMA.connect(&modOscillatorGain2);
    lfo.connect(&filterMA);

    filterMA.connect(&filterModGain);

    oscillator1.connect(&oscillator1Gain);
    oscillator2.connect(&oscillator2Gain);

    filter_mixer.addInput(&oscillator1Gain, 1);
    filter_mixer.addInput(&oscillator2Gain, 1);
    filter_mixer.connect(&filter);
    filter.connect(&filter2);
    filter2.connect(&gainVolume);

    modOscillatorGain1.automate(&oscillator1, OscillatorNode::Parameters::Frequency);
    modOscillatorGain2.automate(&oscillator2, OscillatorNode::Parameters::Frequency);

    filterModGain.automate(&filter, LP12FilterNode::Parameters::Cutoff);
    filterModGain.automate(&filter2, LP12FilterNode::Parameters::Cutoff);

    adsr.automate(&gainVolume, GainNode::Parameters::Gain);

    adsr.setGate(true);

    //gainVolume.gain()->linearRampValueAtTime(1, 1.2);

    //adsr.automate(&gainVolume, GainNode::Parameters::Gain);
    // auto voiceNode = VoiceNode::Builder(context)
    //                     .setModFrequency(5)
    //                     .setOscillator1Frequency(440)
    //                     .setOscillator2Frequency(440 * std::pow(2, -1))
    //                     .setOscillator1Gain(.6)
    //                     .setOscillator2Gain(.4)
    //                     .setOscillator1ModGain(.4)
    //                     .setOscillator2ModGain(.1).build();

    //voiceNode.connect(m_masterGain);

    static int last_processing_id = 0;

    // Set the callback for the audio player
    m_audioPlayer.setCallback([&gainVolume, &context, &adsr](const void* user_data, float* output, unsigned long frames_per_buffer) {

        gainVolume.process(frames_per_buffer, last_processing_id++ /*context.lastBatch()*/);  // Process the signal chain

        float* buffer = gainVolume.buffer();  // Get the processed buffer

        // Copy the buffer to the output
        for (unsigned long i = 0; i < frames_per_buffer; ++i) {
            output[i * 2] = buffer[i];      // Left channel
            output[i * 2 + 1] = buffer[i];  // Right channel (duplicate for stereo)
        }

        if(last_processing_id > 100) {
            adsr.setGate(false);
        }

        if(last_processing_id > 300) {
            adsr.setGate(true);
        }

        if(last_processing_id > 350) {
            adsr.setGate(false);
        }

        //context.updateBatch();
    });

    // Initialize and start the audio player
    if (!m_audioPlayer.initializeStream()) {
        std::cerr << "Failed to initialize audio stream!";
    }

    if (!m_audioPlayer.start()) {
        std::cerr << "Failed to start audio stream!";
    }

    std::cout << "Note released. Press Enter to quit.\n";
    std::cin.get();

    return 0;

}

//#define USE_WINDOW

int main(int argc, char *argv[])
{

#ifdef USE_WINDOW
    return showMainWindow(argc, argv);
#else
    return showConsole(argc, argv);
#endif

}
