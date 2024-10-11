#pragma once
#include <mutex>
#include <vector>

#include "audionode.h"

class MixerNode final : public AudioNode
{
public:
    struct InputNodeInfo {
        AudioNode * node;
        float gain;
    };

public:
    explicit MixerNode(AudioContext& context) : AudioNode(context) {}

    void addInput(AudioNode* node, float gain);
    void removeInput(AudioNode* node);

    void setInputGain(AudioNode* node, float gain);

protected:
	void processInternal(unsigned frames) override;

public:
	void addAutomation(AudioNode* node, unsigned port) override {}
	AudioNode* removeAutomation(unsigned port) override { return nullptr;  }

private:
private:
    std::vector<InputNodeInfo> inputs_;
    std::mutex input_mutex_;
};
