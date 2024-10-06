#ifndef VOICE_H
#define VOICE_H

#include "gainnode.h"
#include "lp12filternode.h"
#include "mixernode.h"
#include "oscillatornode.h"
class Voice {

public:
    Voice(int noteIndex, float velocity);

    void setOsc1Frequency(float frequency);
    void setOsc2Frequency(float frequency);
    void setModOscFrequency(float frequency);

    void setOsc1Detune(float detune);
    void setOsc2Detune(float detune);

    void setOsc1Waveform(int waveform);
    void setOsc2Waveform(int waveform);
    void setModOscWaveform(int waveform);

    void setOscillatorMix(float oscillator1Gain, float oscillator2Gain);
    void setModMix(float oscillator1Gain, float oscillator2Gain);

    void setFilterCutoff(float frequency);
    void setFilterDetune(float detune);
    void setFilterEnvelopeGain(float mix);
    void setFilterModGain(float mix);

protected:
    void buildAudioChain();

private:

    std::shared_ptr<OscillatorNode> m_oscillator1 = nullptr;
    std::shared_ptr<OscillatorNode> m_oscillator2 = nullptr;
    std::shared_ptr<OscillatorNode> m_oscillatorMod = nullptr;

    // instead of separate gains we will just use a mixer
    // and control the indvidual gains of the mixer channels
    // the mixer can take care of the math
    std::shared_ptr<MixerNode> m_oscillatorMixer = nullptr;
    std::shared_ptr<MixerNode> m_oscillatorModMixer = nullptr;

    std::shared_ptr<LP12FilterNode> m_filter1 = nullptr;
    std::shared_ptr<LP12FilterNode> m_filter2 = nullptr;

    std::shared_ptr<GainNode> m_modOsc1Gain = nullptr;
    std::shared_ptr<GainNode> m_modOsc2Gain = nullptr;

    // envelope will control the gain of the gain node
    // output of envelope depends on state            ___/---\___
    std::shared_ptr<GainNode> m_filterEnvelopeGain = nullptr;
    std::shared_ptr<GainNode> m_volumeEnvelopeGain = nullptr;

    // actual envelopes themselves
    //std::shared_ptr<EnvelopeNode> m_filterEnvelope;
    //std::shared_ptr<EnvelopeNode> m_volumeEnvelope;

    // TODO:
    // overdrive node
    // reverb node
    // compressor node

    float m_originalFrequency = 0;

};

#endif // VOICE_H
