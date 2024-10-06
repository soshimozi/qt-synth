#include "voice.h"
#include <cmath>


// Helper function to convert note numbers to frequency.
float frequencyFromNoteNumber(int note) {
    // Example formula, replace with actual conversion logic.
    return 440.0 * std::pow(2.0, (note - 69) / 12.0);
}

Voice::Voice(int noteIndex, float velocity) {
    m_originalFrequency = frequencyFromNoteNumber(noteIndex);

    // build audio chain
    buildAudioChain();
}


void Voice::setOsc1Frequency(float frequency) {}
void Voice::setOsc2Frequency(float frequency) {}
void Voice::setModOscFrequency(float frequency) {}

void Voice::setOsc1Detune(float detune) {}
void Voice::setOsc2Detune(float detune) {}

void Voice::setOsc1Waveform(int waveform) {}
void Voice::setOsc2Waveform(int waveform) {}
void Voice::setModOscWaveform(int waveform) {}

void Voice::setOscillatorMix(float oscillator1Gain, float oscillator2Gain) {}
void Voice::setModMix(float oscillator1Gain, float oscillator2Gain) {}

void Voice::setFilterCutoff(float frequency) {}
void Voice::setFilterDetune(float detune) {}
void Voice::setFilterEnvelopeGain(float mix) {}
void Voice::setFilterModGain(float mix) {}

void Voice::buildAudioChain() {
    m_oscillatorMod = std::make_shared<OscillatorNode>();

    m_oscillator1 = std::make_shared<OscillatorNode>();
    m_oscillator2 = std::make_shared<OscillatorNode>();

}
