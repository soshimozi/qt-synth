#ifndef VOICENODE_H
#define VOICENODE_H

#include "audionode.h"
#include "gainnode.h"
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
    VoiceNode(wave_shape mod_waveform,
              wave_shape oscillator_1_waveform,
              wave_shape oscillator_2_waveform,
              float mod_frequency,
              float oscillator_1_mod_mix,
              float oscillator_2_mod_mix,
              float oscillator_1_frequency,
              float oscillator_2_frequency,
              float filter_cutoff,
              float filter_q,
              float filter_mod_mix,
              float filter_env_mix,
              ADSR filter_envelope,
              ADSR volume_envelope,
              float drive,
              float reverb,
              float volume
              );

private:
    OscillatorNode m_oscillators[2];
    OscillatorNode m_mod_oscillator;
    GainNode m_oscillator_gain[2] { GainNode(0.0f), GainNode(0.0f) };
    GainNode m_mod_oscillatorGain;
};

#endif // VOICENODE_H
