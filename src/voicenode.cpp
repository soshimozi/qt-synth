#include "voicenode.h"
#include <cstring>


VoiceNode::VoiceNode(const VoiceParameters& parameters)
    : oscillator_gain_1_(0), oscillator_gain_2_(0),
    mod_oscillator_gain_1_(0), mod_oscillator_gain_2_(0),
    mixer_node_(2) {

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

VoiceNode::VoiceNode() :  oscillator_gain_1_(0), oscillator_gain_2_(0),
    mod_oscillator_gain_1_(0), mod_oscillator_gain_2_(0),
    mixer_node_(2) {

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

    mod_oscillator_.connect(&mod_oscillator_gain_1_);
    mod_oscillator_.connect(&mod_oscillator_gain_2_);

    mod_oscillator_gain_1_.automate(&oscillator_gain_1_, GainNode::Parameters::GainModulation);
    mod_oscillator_gain_2_.automate(&oscillator_gain_2_, GainNode::Parameters::GainModulation);

    oscillator_1_.connect(&oscillator_gain_1_);
    oscillator_2_.connect(&oscillator_gain_2_);

    mixer_node_.addMix(&oscillator_gain_1_, 0);
    mixer_node_.addMix(&oscillator_gain_2_, 1);

    // mixer is 100% for each channel, the oscillator gain will set the
    // the actual signal going into the output node.
    mixer_node_.setGain(0, 1.0);
    mixer_node_.setGain(1, 1.0);
}

void VoiceNode::updateOscillator1Detune(float detune) {
    oscillator_1_.setDetune(detune);
}

void VoiceNode::updateOscillator2Detune(float detune) {
    oscillator_2_.setDetune(detune);
}


void VoiceNode::setModWaveform(wave_shape waveform) {
    mod_oscillator_.setWaveform(waveform);
}

void VoiceNode::setOscillator1Waveform(wave_shape waveform) {
    oscillator_1_.setWaveform(waveform);
}

void VoiceNode::setOscillator2Waveform(wave_shape waveform) {
    oscillator_2_.setWaveform(waveform);
}

void VoiceNode::updateModFrequency(float frequency) {
    mod_oscillator_.setFrequency(frequency);
}

void VoiceNode::updateModOscillator1Gain(float gain) {
    mod_oscillator_gain_1_.setGain(gain);
}

void VoiceNode::updateModOscillator2Gain(float gain) {
    mod_oscillator_gain_2_.setGain(gain);
}

void VoiceNode::updateOscillator1Frequency(float frequency) {
    oscillator_1_.setFrequency(frequency);
}

void VoiceNode::updateOscillator2Frequency(float frequency) {
    oscillator_2_.setFrequency(frequency);
}

void VoiceNode::updateOscillator1Gain(float gain) {
    oscillator_gain_1_.setGain(gain);
}

void VoiceNode::updateOscillator2Gain(float gain) {
    oscillator_gain_2_.setGain(gain);
}


void VoiceNode::processInternal(const unsigned frames) {
    ensureBufferSize(frames);

    mixer_node_.process(frames, last_processing_id_);

    const float* input_buffer = mixer_node_.buffer();

    for(unsigned int i = 0; i < frames; i++) {
        buffer_[i] = input_buffer[i];
    }
}

