#pragma once
#include "audionode.h"
#include "automationnode.h"

class LP12FilterNode : public AudioNode
{
public:
    enum class Parameters {
        Cutoff = 0,
        Resonance = 1,
        Detune = 2
    };

public:
    LP12FilterNode(AudioContext& context, float initial_cutoff = 20000.0f, float initial_resonance = 1.0f, float initial_detune = 0.0f);

    inline void setCutoff(float cutoff) { cutoff_automation_.setBaseValue(cutoff); };
    inline void setResonance(float resonance) { resonance_automation_.setBaseValue(resonance); }
    inline void setDetune(float detune) { detune_automation_.setBaseValue(detune); }

protected:
	void processInternal(unsigned frames) override;
    void addAutomation(AudioNode* node, unsigned port) override;
    AudioNode* removeAutomation(unsigned port) override;

private:
    void calculateCoefficients(float cutoff, float resonance, float detune);  // Calculate the filter coefficients

	// Internal state for the filter
    float vibra_speed_ = 0.0f;
    float vibra_pos_ = 0.0f;
	float w_, q_, r_, c_; // Filter coefficients

	// parameters
    AutomationNode cutoff_automation_;
    AutomationNode resonance_automation_;
    AutomationNode detune_automation_;

    float previous_cutoff_;
    float previous_resonance_;
    float previous_detune_;

};
