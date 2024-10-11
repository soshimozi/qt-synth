#pragma once
#include "audionode.h"
#include "automationnode.h"

class GainNode : public AudioNode
{
public:
    enum class Parameters {
        Gain = 0
    };

public:
    explicit GainNode(AudioContext& context, float gain = 1.0);

	void processInternal(unsigned frames) override;
    void setGain(float gain) { gain_automation_.setBaseValue(gain); gain_ = gain; }

protected:
    void addAutomation(AudioNode* node, unsigned port) override;
    AudioNode* removeAutomation(unsigned port) override;

private:
    AutomationNode gain_automation_;
    float gain_;

};
