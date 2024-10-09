
#include "audioplayer.h"
#include "gainnode.h"
#include "mainwindow.h"
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
    AudioPlayer *m_audioPlayer = new AudioPlayer;
    GainNode *m_masterGain = new GainNode(.25);

    auto voiceNode = VoiceNode::Builder()
                        .setModFrequency(5)
                        .setOscillator1Frequency(440)
                        .setOscillator2Frequency(440 * std::pow(2, -1))
                        .setOscillator1Gain(.6)
                        .setOscillator2Gain(.4)
                        .setOscillator1ModGain(.4)
                         .setOscillator2ModGain(.1).build();

    voiceNode.connect(m_masterGain);

    static int last_processing_id = 0;

    // Set the callback for the audio player
    m_audioPlayer->setCallback([m_masterGain](const void* user_data, float* output, unsigned long frames_per_buffer) {
        m_masterGain->process(frames_per_buffer, last_processing_id++);  // Process the signal chain
        float* gainBuffer = m_masterGain->buffer();  // Get the processed buffer

        // Copy the buffer to the output
        for (unsigned long i = 0; i < frames_per_buffer; ++i) {
            output[i * 2] = gainBuffer[i];      // Left channel
            output[i * 2 + 1] = gainBuffer[i];  // Right channel (duplicate for stereo)
        }
    });

    // Initialize and start the audio player
    if (!m_audioPlayer->initializeStream()) {
        std::cerr << "Failed to initialize audio stream!";
    }

    if (!m_audioPlayer->start()) {
        std::cerr << "Failed to start audio stream!";
    }

    std::cout << "Note released. Press Enter to quit.\n";
    std::cin.get();

    return 0;

}

#define USE_WINDOW

int main(int argc, char *argv[])
{

#ifdef USE_WINDOW
    return showMainWindow(argc, argv);
#else
    return showConsole(argc, argv);
#endif

}
