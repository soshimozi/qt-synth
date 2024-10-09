#ifndef VOICENODE_H
#define VOICENODE_H

#include "audionode.h"
#include "gainnode.h"
#include "lp12filternode.h"
#include "mixernode.h"
#include "oscillatornode.h"

typedef struct _adsr {
    float attack;
    float sustain;
    float decay;
    float release;
} ADSR;


class VoiceNode : public AudioNode
{
public:
    struct VoiceParameters {
        // Oscillator and Modulator Parameters
        wave_shape mod_waveform = wave_shape::sine;
        wave_shape oscillator_1_waveform = wave_shape::sine;
        wave_shape oscillator_2_waveform = wave_shape::sine;

        float mod_frequency = 0.0f;
        float oscillator_1_mod_gain = 0.0f;
        float oscillator_2_mod_gain = 0.0f;

        float oscillator_1_frequency = 0.0f;
        float oscillator_2_frequency = 0.0f;

        float oscillator_1_gain = 0.0f;
        float oscillator_2_gain = 0.0f;

        float oscillator_1_detune = 0.0f;
        float oscillator_2_detune = 0.0f;

        // ... Add more parameters as needed
    };

    class Builder {
    public:
        Builder() = default;

        // Setter methods
        Builder& setModWaveform(wave_shape value) {
            params_.mod_waveform = value;
            return *this;
        }

        Builder& setOscillator1Waveform(wave_shape value) {
            params_.oscillator_1_waveform = value;
            return *this;
        }

        Builder& setOscillator2Waveform(wave_shape value) {
            params_.oscillator_2_waveform = value;
            return *this;
        }

        Builder& setModFrequency(float value) {
            params_.mod_frequency = value;
            return *this;
        }

        Builder& setOscillator1ModGain(float value) {
            params_.oscillator_1_mod_gain = value;
            return *this;
        }

        Builder& setOscillator2ModGain(float value) {
            params_.oscillator_2_mod_gain = value;
            return *this;
        }

        Builder& setOscillator1Frequency(float value) {
            params_.oscillator_1_frequency = value;
            return *this;
        }

        Builder& setOscillator2Frequency(float value) {
            params_.oscillator_2_frequency = value;
            return *this;
        }

        Builder& setOscillator1Gain(float value) {
            params_.oscillator_1_gain = value;
            return *this;
        }

        Builder& setOscillator2Gain(float value) {
            params_.oscillator_2_gain = value;
            return *this;
        }

        Builder& setOscillator1Detune(float value) {
            params_.oscillator_1_detune = value;
            return *this;
        }

        Builder& setOscillator2Detune(float value) {
            params_.oscillator_2_detune = value;
            return *this;
        }

        // Build method
        VoiceNode build() {
            return VoiceNode(params_);
        }

        VoiceParameters parameters() const { return params_; }

    private:
        VoiceParameters params_;
    };

    explicit VoiceNode();

private:
    explicit VoiceNode(const VoiceParameters& params);

public:
    void setParameters(const VoiceParameters& parameters);

    void setModWaveform(wave_shape waveform);
    void setOscillator1Waveform(wave_shape waveform);
    void setOscillator2Waveform(wave_shape waveform);

    void updateModFrequency(float frequency);

    void updateModOscillator1Gain(float gain);
    void updateModOscillator2Gain(float gain);

    void updateOscillator1Frequency(float frequency);
    void updateOscillator2Frequency(float frequency);

    void updateOscillator1Gain(float gain);
    void updateOscillator2Gain(float gain);

    void updateOscillator1Detune(float detune);
    void updateOscillator2Detune(float detune);

    bool note_on() const { return note_on_; }
protected:
    void addAutomation(AudioNode* node, unsigned port) override {}
    AudioNode* removeAutomation(unsigned port) override { return nullptr;}
    void processInternal(unsigned int frames) override;

private:

    void buildDeviceChain();
    // VoiceNode(wave_shape mod_waveform,
    //           wave_shape oscillator_1_waveform,
    //           wave_shape oscillator_2_waveform,
    //           float mod_frequency,
    //           float oscillator_1_mod_gain,
    //           float oscillator_2_mod_gain,
    //           float oscillator_1_frequency,
    //           float oscillator_2_frequency,
    //           float oscillator_1_gain,
    //           float oscillator_2_gain,
    //           float oscillator_1_detune,
    //           float oscillator_2_detune
    //           );


    // void buildEffectChain(wave_shape mod_waveform,
    //                       wave_shape oscillator_1_waveform,
    //                       wave_shape oscillator_2_waveform,
    //                       float mod_frequency,
    //                       float oscillator_1_mod_gain,
    //                       float oscillator_2_mod_gain,
    //                       float oscillator_1_frequency,
    //                       float oscillator_2_frequency,
    //                       float oscillator_1_gain,
    //                       float oscillator_2_gain,
    //                       float oscillator_1_detune,
    //                       float oscillator_2_detune);
private:
    OscillatorNode m_oscillator1;
    OscillatorNode m_oscillator2;
    OscillatorNode m_mod_oscillator;
    GainNode m_oscillator_gain1;
    GainNode m_oscillator_gain2;
    GainNode m_mod_oscillator_gain1;
    GainNode m_mod_oscillator_gain2;
    MixerNode m_mixer_node;

    bool note_on_ = false;
};

#endif // VOICENODE_H
