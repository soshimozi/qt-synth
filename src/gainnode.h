#pragma once
#include "audionode.h"
#include "automationnode.h"

class GainNode final : public AudioNode
{
public:
    GainNode() : gain_parameter_node_(1.0) {} // Default constructor with default gain value
	explicit GainNode(float gain);

	void processInternal(unsigned frames) override;
	void setGain(float gain);

private:
	AutomationNode gain_parameter_node_;

protected:
    void addAutomation(AudioNode* node, unsigned port) override;
    AudioNode* removeAutomation(unsigned port) override;

};
