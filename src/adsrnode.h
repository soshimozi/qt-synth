#ifndef ADSRNODE_H
#define ADSRNODE_H

#include "audionode.h"
#include "automationnode.h"

class ADSRNode : public AudioNode
{
public:
    enum class Parameters {
        GateAutomation = 0
    };

public:
    explicit ADSRNode(float attack = 0.01f, float decay = 0.1f, float sustain = 0.8f, float release = 0.5f, float sample_rate = 44100);

    void setAttack(float attack);
    void setDecay(float decay);
    void setSustain(float sustain);
    void setRelease(float release);
    void setGate(bool gate);

protected:
    void processInternal(unsigned frames) override;
    void addAutomation(AudioNode* node, unsigned port) override;
    AudioNode* removeAutomation(unsigned port) override;

private:
    AutomationNode gate_automation_;


    float attack_;
    float decay_;
    float sustain_;
    float release_;
    float sample_rate_;
    bool gate_;

    float envelope_level_;

    AutomationNode gate_parameter_node_;

    enum class State { Idle, Attack, Decay, Sustain, Release } state_;

};

#endif // ADSRNODE_H
