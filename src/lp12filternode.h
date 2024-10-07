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
    LP12FilterNode(float initial_cutoff = 20000.0f, float initial_resonance = 1.0f, float initial_detune = 0.0f, float sample_rate = 44100);

	void setCutoff(float cutoff);
	void setResonance(float resonance);
    void setDetune(float detune);

protected:
	void processInternal(unsigned frames) override;
    void addAutomation(AudioNode* node, unsigned port) override;
    AudioNode* removeAutomation(unsigned port) override;

private:
    void calculateCoefficients(float cutoff, float resonance, float detune);  // Calculate the filter coefficients

	float sampleRate_;

	// Internal state for the filter
	float vibraSpeed_ = 0.0f;
	float vibraPos_ = 0.0f;
	float w_, q_, r_, c_; // Filter coefficients

	// parameters
	AutomationNode cutoffParameterNode_;
	AutomationNode resonanceParameterNode_;
    AutomationNode detuneParameterNode_;

	float previousCutoff_;
	float previousResonance_;
    float previousDetune_;

};
