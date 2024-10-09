#include "voicenode.h"
#include <cstring>


VoiceNode::VoiceNode(const VoiceParameters& parameters)
    : m_oscillator_gain1(0), m_oscillator_gain2(0),
    m_mod_oscillator_gain1(0), m_mod_oscillator_gain2(0),
    m_mixer_node(2) {

    setParameters(parameters);
    buildDeviceChain();
    // buildEffectChain(mod_waveform,
    //                  oscillator_1_waveform,
    //                  oscillator_2_waveform,
    //                  mod_frequency,
    //                  oscillator_1_mod_gain,
    //                  oscillator_2_mod_gain,
    //                  oscillator_1_frequency,
    //                  oscillator_2_frequency,
    //                  oscillator_1_gain,
    //                  oscillator_2_gain,
    //                  oscillator_1_detune,
    //                  oscillator_2_detune);
}

VoiceNode::VoiceNode() :  m_oscillator_gain1(0), m_oscillator_gain2(0),
    m_mod_oscillator_gain1(0), m_mod_oscillator_gain2(0),
    m_mixer_node(2) {

    buildDeviceChain();
}

void VoiceNode::setParameters(const VoiceParameters& parameters) {

    setModWaveform(parameters.mod_waveform);
    updateModFrequency(parameters.mod_frequency);

    setOscillator1Waveform(parameters.oscillator_1_waveform);
    setOscillator2Waveform(parameters.oscillator_2_waveform);

    updateModOscillator1Gain(parameters.oscillator_1_mod_gain);
    updateModOscillator2Gain(parameters.oscillator_2_mod_gain);

    updateOscillator1Frequency(parameters.oscillator_1_frequency);
    updateOscillator2Frequency(parameters.oscillator_2_frequency);

    updateOscillator1Gain(parameters.oscillator_1_gain);
    updateOscillator2Gain(parameters.oscillator_1_gain);

    updateOscillator1Detune(parameters.oscillator_1_detune);
    updateOscillator2Detune(parameters.oscillator_2_detune);
}

void VoiceNode::buildDeviceChain() {

    m_mod_oscillator.connect(&m_mod_oscillator_gain1);
    m_mod_oscillator.connect(&m_mod_oscillator_gain2);

    m_mod_oscillator_gain1.automate(&m_oscillator_gain1, GainNode::Parameters::GainModulation);
    m_mod_oscillator_gain2.automate(&m_oscillator_gain2, GainNode::Parameters::GainModulation);

    m_oscillator1.connect(&m_oscillator_gain1);
    m_oscillator2.connect(&m_oscillator_gain2);

    m_mixer_node.addMix(&m_oscillator_gain1, 0);
    m_mixer_node.addMix(&m_oscillator_gain2, 1);

    // mixer is 100% for each channel, the oscillator gain will set the
    // the actual signal going into the output node.
    m_mixer_node.setGain(0, 1.0);
    m_mixer_node.setGain(1, 1.0);
}

void VoiceNode::updateOscillator1Detune(float detune) {
    m_oscillator1.setDetune(detune);
}

void VoiceNode::updateOscillator2Detune(float detune) {
    m_oscillator2.setDetune(detune);
}


void VoiceNode::setModWaveform(wave_shape waveform) {
    m_mod_oscillator.setWaveform(waveform);
}

void VoiceNode::setOscillator1Waveform(wave_shape waveform) {
    m_oscillator1.setWaveform(waveform);
}

void VoiceNode::setOscillator2Waveform(wave_shape waveform) {
    m_oscillator2.setWaveform(waveform);
}

void VoiceNode::updateModFrequency(float frequency) {
    m_mod_oscillator.setFrequency(frequency);
}

void VoiceNode::updateModOscillator1Gain(float gain) {
    m_mod_oscillator_gain1.setGain(gain);
}

void VoiceNode::updateModOscillator2Gain(float gain) {
    m_mod_oscillator_gain2.setGain(gain);
}

void VoiceNode::updateOscillator1Frequency(float frequency) {
    m_oscillator1.setFrequency(frequency);
}

void VoiceNode::updateOscillator2Frequency(float frequency) {
    m_oscillator2.setFrequency(frequency);
}

void VoiceNode::updateOscillator1Gain(float gain) {
    m_oscillator_gain1.setGain(gain);
}

void VoiceNode::updateOscillator2Gain(float gain) {
    m_oscillator_gain2.setGain(gain);
}


void VoiceNode::processInternal(const unsigned frames) {
    ensureBufferSize(frames);

    m_mixer_node.process(frames, m_last_processing_id);

    const float* input_buffer = m_mixer_node.buffer();

    for(unsigned int i = 0; i < frames; i++) {
        m_buffer[i] = input_buffer[i];
    }
}

