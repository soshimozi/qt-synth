#include "oscillatornode.h"

#include <cmath>
#include "definitions.h"


OscillatorNode::OscillatorNode(const wave_shape waveform, const float frequency, const float detune, const float pulse_width, const float sample_rate)
    :m_waveform(waveform), m_phase(0.0f), m_detune_cents(detune),  m_sample_rate(sample_rate),
    frequency_parameter_node_(frequency), pulse_width_parameter_node_(pulse_width) {
}

void OscillatorNode::setFrequency(const float frequency) {
    frequency_parameter_node_.setStaticValue(frequency);
}

void OscillatorNode::setWaveform(const wave_shape waveform) {
    m_waveform = waveform;
}

void OscillatorNode::setPulseWidth(const float pulse_width) {
    pulse_width_parameter_node_.setStaticValue(pulse_width);
}

void OscillatorNode::setDetune(const float detune) {
    m_detune_cents = detune;
}
void OscillatorNode::processInternal(const unsigned int frames) {
    ensureBufferSize(frames);

    frequency_parameter_node_.process(frames, m_last_processing_id);
    pulse_width_parameter_node_.process(frames, m_last_processing_id);

    const auto frequency_buffer = std::make_unique<float[]>(frames);
    memcpy(frequency_buffer.get(), frequency_parameter_node_.buffer(), frames * sizeof(float));

    const auto pulse_width_buffer = std::make_unique<float[]>(frames);
    memcpy(pulse_width_buffer.get(), pulse_width_parameter_node_.buffer(), frames * sizeof(float));

    // Generate waveform samples
    for (unsigned int i = 0; i < frames; ++i) {
        const float current_freq = frequency_buffer[i] * std::pow(2.0f, m_detune_cents / 1200.0f);
        const float current_pulse_width = pulse_width_buffer[i];

        m_buffer[i] = generateSample(m_phase, current_pulse_width);

        // Update phase
        const float phase_increment = TWO_PI * current_freq / m_sample_rate;
        m_phase += phase_increment;
        if (m_phase >= TWO_PI) {
            m_phase -= TWO_PI;
        }
    }
}

float OscillatorNode::generateSample(const float phase, const float current_pulse_width) const
{
    switch (m_waveform) {
    case wave_shape::sine:
        return sine(phase);
    case wave_shape::triangle:
        return triangle(phase);
    case wave_shape::square:
        return square(phase);
    case wave_shape::sawtooth:
        return sawtooth(phase);
    case wave_shape::inv_sawtooth:
        return invSawtooth(phase);
    case wave_shape::pulse:
        return pulse(phase, current_pulse_width);
    }

    return 0.0;
}

float OscillatorNode::sine(const float phase) {
    return sinf(phase);  // sineTable.getValue(phase);
}

float OscillatorNode::triangle(const float phase) {
    // Triangle wave between -1 and 1
    const float value = 2.0f * fabsf(2.0f * (phase / TWO_PI - floorf(phase / TWO_PI + 0.5f))) - 1.0f;
    return value;
}

float OscillatorNode::square(const float phase) {
    return sine(phase) >= 0.0f ? 1.0f : -1.0f;
}

float OscillatorNode::sawtooth(const float phase) {
    // Sawtooth wave between -1 and 1
    const float value = 2.0f * (phase / TWO_PI - floorf(phase / TWO_PI + 0.5f));
    return value;
}

float OscillatorNode::invSawtooth(const float phase) {
    // Inverse sawtooth wave
    return -sawtooth(phase);
}

float OscillatorNode::pulse(const float phase, const float current_pulse_width) {
    // Pulse wave between -1 and 1
    float pw = current_pulse_width;
    pw = fmodf(pw, 1.0f);
    return fmodf(phase / TWO_PI, 1.0f) < pw ? 1.0f : -1.0f;
}

void OscillatorNode::addAutomation(AudioNode* node, const unsigned port) {

    switch(static_cast<Parameters>(port))
    {
    case Parameters::Frequency:
        frequency_parameter_node_.setInput(node);
        break;
    case Parameters::PulseWidth:
        pulse_width_parameter_node_.setInput(node);
        break;
    }
}

AudioNode* OscillatorNode::removeAutomation(const unsigned port) {

    AudioNode* node = nullptr;
    switch(static_cast<Parameters>(port)) {
    case Parameters::Frequency:
        node = frequency_parameter_node_.input();
        frequency_parameter_node_.setInput(nullptr);
        break;
    case Parameters::PulseWidth:
        node = pulse_width_parameter_node_.input();
        pulse_width_parameter_node_.setInput(nullptr);
        break;
    }

    return node;
}


