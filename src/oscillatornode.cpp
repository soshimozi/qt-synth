#include "oscillatornode.h"

#include <cmath>
#include "definitions.h"


OscillatorNode::OscillatorNode(AudioContext& context, const wave_shape waveform, const float frequency, const float detune, const float pulse_width)
    : AudioNode(context), waveform_(waveform), phase_(0.0f), detune_cents_(detune),
    frequency_automation_(context, frequency), pulse_width_automation_(context, pulse_width) {
}

void OscillatorNode::setFrequency(const float frequency) {
    frequency_automation_.setBaseValue(frequency);
}

void OscillatorNode::setWaveform(const wave_shape waveform) {
    waveform_ = waveform;
}

void OscillatorNode::setPulseWidth(const float pulse_width) {
    pulse_width_automation_.setBaseValue(pulse_width);
}

void OscillatorNode::setDetune(const float detune) {
    detune_cents_ = detune;
}
void OscillatorNode::processInternal(const unsigned int frames) {

    frequency_automation_.process(frames, last_processing_id_);
    pulse_width_automation_.process(frames, last_processing_id_);

    const auto frequency_buffer = std::make_unique<float[]>(frames);
    memcpy(frequency_buffer.get(), frequency_automation_.buffer(), frames * sizeof(float));

    const auto pulse_width_buffer = std::make_unique<float[]>(frames);
    memcpy(pulse_width_buffer.get(), pulse_width_automation_.buffer(), frames * sizeof(float));

    // Generate waveform samples
    for (unsigned int i = 0; i < frames; ++i) {
        const float current_freq = (frequency_automation_.baseValue() + frequency_buffer[i]) * std::pow(2.0f, detune_cents_ / 1200.0f);
        const float current_pulse_width = pulse_width_buffer[i];

        buffer_[i] = generateSample(phase_, current_pulse_width);

        // Update phase
        const float phase_increment = TWO_PI * current_freq / context_.sampleRate();
        phase_ += phase_increment;
        if (phase_ >= TWO_PI) {
            phase_ -= TWO_PI;
        }
    }
}

//float OscillatorNode::generateSample(const float phase, const float current_pulse_width) const
// {
//     switch (waveform_) {
//     case wave_shape::sine:
//         return sine(phase);
//     case wave_shape::triangle:
//         return triangle(phase);
//     case wave_shape::square:
//         return square(phase);
//     case wave_shape::sawtooth:
//         return sawtooth(phase);
//     case wave_shape::inv_sawtooth:
//         return invSawtooth(phase);
//     case wave_shape::pulse:
//         return pulse(phase, current_pulse_width);
//     }

//     return 0.0;
// }

//float OscillatorNode::sine(const float phase) const {
//     return sinf(phase);  // sineTable.getValue(phase);
// }

// float OscillatorNode::triangle(const float phase) {
//     // Triangle wave between -1 and 1
//     const float value = 2.0f * fabsf(2.0f * (phase / TWO_PI - floorf(phase / TWO_PI + 0.5f))) - 1.0f;
//     return value;
// }

// float OscillatorNode::square(const float phase) {
//     return sine(phase) >= 0.0f ? 1.0f : -1.0f;
// }

// float OscillatorNode::sawtooth(const float phase) {
//     // Sawtooth wave between -1 and 1
//     const float value = 2.0f * (phase / TWO_PI - floorf(phase / TWO_PI + 0.5f));
//     return value;
// }

// float OscillatorNode::invSawtooth(const float phase) {
//     // Inverse sawtooth wave
//     return -sawtooth(phase);
// }

// float OscillatorNode::pulse(const float phase, const float current_pulse_width) {
//     // Pulse wave between -1 and 1
//     // float pw = current_pulse_width;
//     // pw = fmodf(current_pulse_width, 1.0f);
//     return fmodf(phase / TWO_PI, 1.0f) < fmodf(current_pulse_width, 1.0f) ? 1.0f : -1.0f;
// }

void OscillatorNode::addAutomation(AudioNode* node, const unsigned port) {

    switch(static_cast<Parameters>(port))
    {
    case Parameters::Frequency:
        frequency_automation_.setInput(node);
        break;
    case Parameters::PulseWidth:
        pulse_width_automation_.setInput(node);
        break;
    }
}

AudioNode* OscillatorNode::removeAutomation(const unsigned port) {

    AudioNode* node = nullptr;
    switch(static_cast<Parameters>(port)) {
    case Parameters::Frequency:
        node = frequency_automation_.input();
        frequency_automation_.setInput(nullptr);
        break;
    case Parameters::PulseWidth:
        node = pulse_width_automation_.input();
        pulse_width_automation_.setInput(nullptr);
        break;
    }

    return node;
}


