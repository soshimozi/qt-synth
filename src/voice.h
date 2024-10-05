#ifndef VOICE_H
#define VOICE_H

#include "gainnode.h"
#include "lp12filternode.h"
#include "mixernode.h"
#include "oscillatornode.h"
class Voice {

public:
    Voice(int noteIndex, float velocity);

    void updateOsc1Frequency(float frequency, float detune);
    void updateOsc2Frequency(float frequency, float detune);
    void updateModOscFrequency(float frequency);

    void updateOscillatorMix(float oscillator1Gain, float oscillator2Gain);
    void updateModMix(float oscillator1Gain, float oscillator2Gain);

    void updateFilterCutoff(float frequency);


protected:
    virtual void buildAudioChain();
private:

    std::shared_ptr<OscillatorNode> m_osc1 = nullptr;
    std::shared_ptr<OscillatorNode> m_osc2 = nullptr;
    std::shared_ptr<OscillatorNode> m_modOsc = nullptr;

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
