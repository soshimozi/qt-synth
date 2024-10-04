#pragma once
#include <vector>

#include "audionode.h"

class MixerNode final : public AudioNode
{
public:
	explicit MixerNode(unsigned int num_inputs);

	void setGain(unsigned int input_index, float gain);

	void addMix(AudioNode* node, unsigned int index);
	AudioNode* removeInput(unsigned int index);

protected:
	void processInternal(unsigned frames) override;

public:
	void addAutomation(AudioNode* node, unsigned port) override {}
	AudioNode* removeAutomation(unsigned port) override { return nullptr;  }

private:
	std::vector<AudioNode*> inputs_;
	std::vector<float> gains_;
};
