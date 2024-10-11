#pragma once

#include "audionode.h"
#include "automationnode.h"
#include "definitions.h"
#include <cmath>

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
    enum class Parameters {
        Frequency = 0,
        PulseWidth = 1
    };

public:
    OscillatorNode(AudioContext& context, wave_shape waveform = wave_shape::sine, float frequency = 0, float detune_cents = 0, float pulse_width = 0.5f);

    void setFrequency(float frequency);
    void setWaveform(wave_shape waveform);
    void setPulseWidth(float pulse_width);
    void setDetune(float detune);

private:
    wave_shape waveform_;
    float phase_;
    float detune_cents_;

    AutomationNode frequency_automation_;
    AutomationNode pulse_width_automation_;

protected:
    void addAutomation(AudioNode* node, unsigned port) override;
    AudioNode* removeAutomation(unsigned port) override;
    void processInternal(unsigned int frames) override;

private:
    float generateSample(float phase, float current_pulse_width) const;

    // Waveform generation methods
    inline float triangle(float phase) const { return (2.0f * fabsf(2.0f * (phase / TWO_PI - floorf(phase / TWO_PI + 0.5f))) - 1.0f); }
    inline float sine(float phase) const { return sinf(phase); }
    inline float square(float phase) const { return sine(phase) >= 0.0f ? 1.0f : -1.0f; };
    inline float sawtooth(float phase) const { return 2.0f * (phase / TWO_PI - floorf(phase / TWO_PI + 0.5f)); }
    inline float invSawtooth(float phase) const { return -sawtooth(phase); }
    inline float pulse(float phase, float current_pulse_width) const { return fmodf(phase / TWO_PI, 1.0f) < fmodf(current_pulse_width, 1.0f) ? 1.0f : -1.0f; };
};
