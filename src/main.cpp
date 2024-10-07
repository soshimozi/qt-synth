
#include "audioplayer.h"
#include "gainnode.h"
#include "mainwindow.h"
#include "oscillatornode.h"
//#include <QApplication>
#include <iostream>
#include <limits>
#include <ostream>
#include <qapplication.h>


int main(int argc, char *argv[])
{
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

    // AudioPlayer *m_audioPlayer = new AudioPlayer;
    // GainNode *m_masterGain = new GainNode(.25);
    // OscillatorNode *m_Oscillator = new OscillatorNode();
    // m_Oscillator->setFrequency(440);
    // m_Oscillator->connect(m_masterGain);

    // static int last_processing_id = 0;

    // // Set the callback for the audio player
    // m_audioPlayer->setCallback([m_masterGain](const void* user_data, float* output, unsigned long frames_per_buffer) {
    //     m_masterGain->process(frames_per_buffer, last_processing_id++);  // Process the signal chain
    //     float* gainBuffer = m_masterGain->buffer();  // Get the processed buffer

    //     // Copy the buffer to the output
    //     for (unsigned long i = 0; i < frames_per_buffer; ++i) {
    //         output[i * 2] = gainBuffer[i];      // Left channel
    //         output[i * 2 + 1] = gainBuffer[i];  // Right channel (duplicate for stereo)
    //     }
    // });

    // // Initialize and start the audio player
    // if (!m_audioPlayer->initializeStream()) {
    //     std::cerr << "Failed to initialize audio stream!";
    // }

    // if (!m_audioPlayer->start()) {
    //     std::cerr << "Failed to start audio stream!";
    // }

    // //PressEnterToContinue();

    // double foo;
    // std::cout << "Enter a double value: ";
    // if (std::cin >> foo) {
    //     std::cout << "You entered: " << foo << std::endl;
    // } else {
    //     std::cout << "Invalid input." << std::endl;
    //     std::cin.clear(); // Clear error state
    //     std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer
    // }
    // std::cout << "Done!" << std::endl;

    // return 0;
}
