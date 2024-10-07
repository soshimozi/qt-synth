#pragma once

#include "audionode.h"

class AutomationNode final : public AudioNode {
public:
	explicit AutomationNode(float initial_value = 0.0f);

    void setStaticValue(float value);
    float getStaticValue() const;

protected:
    void processInternal(unsigned int frames) override;
	void addAutomation(AudioNode* node, unsigned port) override {}
    AudioNode* removeAutomation(unsigned port) override { return nullptr; }

private:
    float static_value_;
};
