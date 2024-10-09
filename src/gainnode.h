#pragma once
#include "audionode.h"
#include "automationnode.h"

class GainNode : public AudioNode
{
public:
    enum class Parameters {
        GainModulation = 0,
    };

public:
	explicit GainNode(float gain);

	void processInternal(unsigned frames) override;
	void setGain(float gain);

private:
    AutomationNode gain_mod_node_;
    float m_gain;

protected:
    void addAutomation(AudioNode* node, unsigned port) override;
    AudioNode* removeAutomation(unsigned port) override;

};
