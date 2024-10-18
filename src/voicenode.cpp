#include "voicenode.h"
#include "arithmeticnode.h"
#include <cstring>


VoiceNode::VoiceNode(AudioContext& context, const VoiceParameters& parameters) :
    AudioNode(context), mod_oscillator_(context), oscillator_1_(context), oscillator_2_(context),
    oscillator_gain_1_(context), oscillator_gain_2_(context), mod_oscillator_gain_1_(context), mod_oscillator_gain_2_(context),
    oscillator_1_volume_envelope_gain_(context), oscillator_2_volume_envelope_gain_(context), filter_envelope_(context), volume_envelope_(context),
    output_(context) {

    setParameters(parameters);
    buildDeviceChain();
}

VoiceNode::VoiceNode(AudioContext& context) :
    AudioNode(context), mod_oscillator_(context), oscillator_1_(context), oscillator_2_(context),
    oscillator_gain_1_(context), oscillator_gain_2_(context), mod_oscillator_gain_1_(context), mod_oscillator_gain_2_(context),
    oscillator_1_volume_envelope_gain_(context), oscillator_2_volume_envelope_gain_(context), filter_envelope_(context), volume_envelope_(context),
    output_(context) {

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

    updateVolumeEnvelopeA(parameters.volume_envelope_a_);
    updateVolumeEnvelopeD(parameters.volume_envelope_d_);
    updateVolumeEnvelopeS(parameters.volume_envelope_s_);
    updateVolumeEnvelopeR(parameters.volume_envelope_r_);

}

void VoiceNode::buildDeviceChain() {

    mod_oscillator_.connect(&mod_oscillator_gain_1_);
    mod_oscillator_.connect(&mod_oscillator_gain_2_);

    mod_oscillator_.automate(&oscillator_gain_1_, GainNode::Parameters::Gain);
    mod_oscillator_.automate(&oscillator_gain_2_, GainNode::Parameters::Gain);

    oscillator_1_.connect(&oscillator_gain_1_);
    oscillator_2_.connect(&oscillator_gain_2_);

    oscillator_gain_1_.connect(&oscillator_1_volume_envelope_gain_);
    oscillator_gain_2_.connect(&oscillator_2_volume_envelope_gain_);

    volume_envelope_.automate(&oscillator_1_volume_envelope_gain_, GainNode::Parameters::Gain);
    volume_envelope_.automate(&oscillator_2_volume_envelope_gain_, GainNode::Parameters::Gain);

    volume_envelope_.setGate(false);

    output_.addInput(&oscillator_1_volume_envelope_gain_, 1.0);
    output_.addInput(&oscillator_2_volume_envelope_gain_, 1.0);
}

void VoiceNode::updateVolumeEnvelopeA(float attack) {
    volume_envelope_.setAttack(attack);
}

void VoiceNode::updateVolumeEnvelopeD(float decay) {
    volume_envelope_.setDecay(decay);
}

void VoiceNode::updateVolumeEnvelopeS(float sustain) {
    volume_envelope_.setSustain(sustain);
}

void VoiceNode::updateVolumeEnvelopeR(float release) {
    volume_envelope_.setRelease(release);
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

    output_.process(frames, last_processing_id_);
    //mixer_node_.process(frames, last_processing_id_);

    const float* input_buffer = output_.buffer();

    for(unsigned int i = 0; i < frames; i++) {
        buffer_[i] = input_buffer[i];
    }
}

