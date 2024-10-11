#ifndef VOICENODE_H
#define VOICENODE_H

#include "adsrnode.h"
//#include "arithmeticnode.h"
#include "audionode.h"
//#include "buffernode.h"
#include "gainnode.h"
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

        float volume_envelope_a_ = 0.001;
        float volume_envelope_d_ = 0.001;
        float volume_envelope_s_ = 0.001;
        float volume_envelope_r_ = 0.001;

        // ... Add more parameters as needed
    };

    class Builder {
    public:
        Builder(AudioContext& context) : context_(context) {}

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

        Builder& setVolumeEnvelopeA(float value) {
            params_.volume_envelope_a_ = value;
            return *this;
        }

        Builder& setVolumeEnvelopeD(float value) {
            params_.volume_envelope_d_ = value;
            return *this;
        }

        Builder& setVolumeEnvelopeS(float value) {
            params_.volume_envelope_s_ = value;
            return *this;
        }

        Builder& setVolumeEnvelopeR(float value) {
            params_.volume_envelope_r_ = value;
            return *this;
        }

        // Build method
        VoiceNode build() {
            return VoiceNode(context_, params_);
        }

        VoiceParameters parameters() const { return params_; }

    private:
        VoiceParameters params_;
        AudioContext& context_;
    };

    explicit VoiceNode(AudioContext& context);

private:
    explicit VoiceNode(AudioContext& context, const VoiceParameters& params);

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

    void updateVolumeEnvelopeA(float attack);
    void updateVolumeEnvelopeD(float decay);
    void updateVolumeEnvelopeS(float sustain);
    void updateVolumeEnvelopeR(float release);

    //bool note_on() const { return note_on_; }
    void noteOn() { volume_envelope_.setGate(1.0); }
    void noteOff() { volume_envelope_.setGate(0.0); }

protected:
    void addAutomation(AudioNode* node, unsigned port) override {}
    AudioNode* removeAutomation(unsigned port) override { return nullptr;}
    void processInternal(unsigned int frames) override;

private:

    void buildDeviceChain();

private:
    OscillatorNode oscillator_1_;
    OscillatorNode oscillator_2_;
    OscillatorNode mod_oscillator_;

    GainNode oscillator_gain_1_;
    GainNode oscillator_gain_2_;

    GainNode mod_oscillator_gain_1_;
    GainNode mod_oscillator_gain_2_;

    GainNode oscillator_1_volume_envelope_gain_;
    GainNode oscillator_2_volume_envelope_gain_;

    ADSRNode filter_envelope_;
    ADSRNode volume_envelope_;

    //ArithmeticNode mod_oscillator_mod_multiplier; //(context_, ArithmeticNode::Operation::Multiply, 1);
    //ArithmeticNode mod_oscillator_mod_adder; //(context_, ArithmeticNode::Operation::Add, 1.25);


    MixerNode output_;

    //bool note_on_ = false;
};

#endif // VOICENODE_H
