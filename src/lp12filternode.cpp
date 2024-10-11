#include "lp12filternode.h"

#include "definitions.h"
#include <cmath>
#include <limits>
#include <memory>
#include <cstring>

LP12FilterNode::LP12FilterNode(AudioContext& context, float initial_cutoff, float initial_resonance, float initial_detune) : AudioNode(context),
    cutoff_automation_(context, initial_cutoff), resonance_automation_(context, initial_resonance), detune_automation_(context,initial_detune),
    previous_cutoff_(initial_cutoff), previous_resonance_(initial_resonance), previous_detune_(initial_detune) {

    calculateCoefficients(initial_cutoff, initial_resonance, initial_detune);
}

// void LP12FilterNode::setCutoff(float cutoff) {
// 	cutoff_automation_.setStaticValue(cutoff);
// }

// void LP12FilterNode::setResonance(float resonance) {
// 	resonance_automation_.setStaticValue(resonance);
// }

// void LP12FilterNode::setDetune(float detune) {
//     detune_automation_.setStaticValue(detune);
// }

void LP12FilterNode::addAutomation(AudioNode* node, unsigned port) {
	// port 0 = cutoff
	// port 1 = resonance

    switch(static_cast<Parameters>(port))    {
    case Parameters::Cutoff:
        cutoff_automation_.setInput(node);
        break;
    case Parameters::Resonance:
        resonance_automation_.setInput(node);
        break;
    case Parameters::Detune:
        detune_automation_.setInput(node);
        break;
    }

    // if(port == 0)
    // {
    // 	cutoffParameterNode_.setInput(node);
    // } else if(port == 1)
    // {
    // 	resonanceParameterNode_.setInput(node);
 //    } else if(port == 2) {
 //        detuneParameterNode_.setInput(node);
 //    }
}

AudioNode* LP12FilterNode::removeAutomation(unsigned port)
{
	AudioNode* node = nullptr;
    switch(static_cast<Parameters>(port))
    {
    case Parameters::Cutoff:
        node = cutoff_automation_.input();
        cutoff_automation_.setInput(nullptr);
        break;

    case Parameters::Resonance:
        node = resonance_automation_.input();
        resonance_automation_.setInput(nullptr);
        break;

    case Parameters::Detune:
        node = detune_automation_.input();
        detune_automation_.setInput(nullptr);
        break;
    }

    // if (port == 0) {
 //        node = cutoffParameterNode_.input();
    // 	cutoffParameterNode_.setInput(nullptr);
    // }
    // else if (port == 1) {
 //        node = resonanceParameterNode_.input();
    // 	resonanceParameterNode_.setInput(nullptr);
 //    } else if(port == 2) {
 //        node = detuneParameterNode_.input();
 //        detuneParameterNode_.setInput(nullptr);
 //    }

	return node;
}

void LP12FilterNode::calculateCoefficients(float cutoff, float resonance, float detune) {

	constexpr float pi2 = TWO_PI;

    float detunedCutoff = cutoff * std::pow(2.0f, detune / 1200.0f);

    w_ = pi2 * detunedCutoff / context_.sampleRate();
	q_ = 1.0f - w_ / (2 * (resonance + 0.5f / (1.0f + w_)) + w_ - 2);
	r_ = q_ * q_;
	c_ = r_ + 1.0f - 2.0f * static_cast<float>(cos(w_)) * q_;
}


void LP12FilterNode::processInternal(unsigned frames) {

	if (input_ == nullptr) {
		memset(buffer_.get(), 0, frames * sizeof(float));
		return;
	}

	input_->process(frames, last_processing_id_);

	// process the parameter nodes
	cutoff_automation_.process(frames, last_processing_id_);
	resonance_automation_.process(frames, last_processing_id_);
    detune_automation_.process(frames, last_processing_id_);

	// Get the cutoff and resonance buffers (these could be dynamic inputs or static values)
	const auto cutoff_buffer = std::make_unique<float[]>(frames);
    memcpy(cutoff_buffer.get(), cutoff_automation_.buffer(), frames * sizeof(float));

	const auto resonance_buffer = std::make_unique<float[]>(frames);
    memcpy(resonance_buffer.get(), resonance_automation_.buffer(), frames * sizeof(float));

    const auto detune_buffer = std::make_unique<float []>(frames);
    memcpy(detune_buffer.get(), detune_automation_.buffer(), frames * sizeof(float));

	for(unsigned int i = 0; i < frames; ++i) {

		const float current_cutoff = cutoff_buffer[i];
		const float current_resonance = resonance_buffer[i];
        const float current_detune = detune_buffer[i];

		// Recalculate coefficients for the current frame if cutoff or resonance has changed
		if(std::fabs( previous_cutoff_ - current_cutoff) <= std::numeric_limits<float>::epsilon() || 
            std::fabs(previous_resonance_ - current_resonance) <= std::numeric_limits<float>::epsilon() ||
            std::fabs(previous_detune_ - current_detune) <= std::numeric_limits<float>::epsilon()) {

            calculateCoefficients(current_cutoff, current_resonance, current_detune);

			previous_cutoff_ = current_cutoff;
			previous_resonance_ = current_resonance;
            previous_detune_ = current_detune;
		}

		// process the sample
        const float sample = input_->buffer()[i];

		vibra_speed_ += (sample - vibra_pos_) * c_;
		vibra_pos_ += vibra_speed_;

		vibra_speed_ *= r_;
		buffer_[i] = vibra_pos_;
	}



}



