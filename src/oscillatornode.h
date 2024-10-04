#pragma once

#include "audionode.h"
#include "automationnode.h"

enum class wave_shape {
    sine,
    triangle,
    square,
    sawtooth,
    inv_sawtooth,
    pulse
};

class OscillatorNode final : public AudioNode {

public:
    OscillatorNode(wave_shape waveform = wave_shape::sine, float frequency = 0, float detune_cents = 0, float pulse_width = 0.5f, float sample_rate = 44100);

    void processInternal(unsigned int frames) override;
    void setFrequency(float frequency);
    void setWaveform(wave_shape waveform);
    void setPulseWidth(float pulse_width);

private:
    wave_shape m_waveform;
    float m_phase;
    float detune_cents;
    float m_sample_rate;

    AutomationNode frequency_parameter_node_;
    AutomationNode pulse_width_parameter_node_;

protected:

private:
    float generateSample(float phase, float current_pulse_width) const;

    // Waveform generation methods
    static float sine(float phase);
    static float triangle(float phase);
    static float square(float phase);
    static float sawtooth(float phase);
    static float invSawtooth(float phase);
    static float pulse(float phase, float current_pulse_width);

public:
    void addAutomation(AudioNode* node, unsigned port) override;
    AudioNode* removeAutomation(unsigned port) override;
};
