#include "lp12filternode.h"

#include "definitions.h"
#include <cmath>
#include <limits>
#include <memory>
#include <cstring>

LP12FilterNode::LP12FilterNode(float initial_cutoff, float initial_resonance, float initial_detune, float sample_rate) :
	sampleRate_(sample_rate),
    cutoffParameterNode_(initial_cutoff), resonanceParameterNode_(initial_resonance), detuneParameterNode_(initial_detune),
    previousCutoff_(initial_cutoff), previousResonance_(initial_resonance), previousDetune_(initial_detune) {

    calculateCoefficients(initial_cutoff, initial_resonance, initial_detune);
}

void LP12FilterNode::setCutoff(float cutoff) {
	cutoffParameterNode_.setStaticValue(cutoff);
}

void LP12FilterNode::setResonance(float resonance) {
	resonanceParameterNode_.setStaticValue(resonance);
}

void LP12FilterNode::setDetune(float detune) {
    detuneParameterNode_.setStaticValue(detune);
}

void LP12FilterNode::addAutomation(AudioNode* node, unsigned port) {
	// port 0 = cutoff
	// port 1 = resonance

    switch(static_cast<Parameters>(port))    {
    case Parameters::Cutoff:
        cutoffParameterNode_.setInput(node);
        break;
    case Parameters::Resonance:
        resonanceParameterNode_.setInput(node);
        break;
    case Parameters::Detune:
        detuneParameterNode_.setInput(node);
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
        node = cutoffParameterNode_.input();
        cutoffParameterNode_.setInput(nullptr);
        break;

    case Parameters::Resonance:
        node = resonanceParameterNode_.input();
        resonanceParameterNode_.setInput(nullptr);
        break;

    case Parameters::Detune:
        node = detuneParameterNode_.input();
        detuneParameterNode_.setInput(nullptr);
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

    w_ = pi2 * detunedCutoff / sampleRate_;
	q_ = 1.0f - w_ / (2 * (resonance + 0.5f / (1.0f + w_)) + w_ - 2);
	r_ = q_ * q_;
	c_ = r_ + 1.0f - 2.0f * static_cast<float>(cos(w_)) * q_;
}


void LP12FilterNode::processInternal(unsigned frames) {
	ensureBufferSize(frames);

	if (m_input == nullptr) {
		memset(m_buffer.get(), 0, frames * sizeof(float));
		return;
	}

	m_input->process(frames, m_last_processing_id);

	// process the parameter nodes
	cutoffParameterNode_.process(frames, m_last_processing_id);
	resonanceParameterNode_.process(frames, m_last_processing_id);
    detuneParameterNode_.process(frames, m_last_processing_id);

	// Get the cutoff and resonance buffers (these could be dynamic inputs or static values)
	const auto cutoff_buffer = std::make_unique<float[]>(frames);
    memcpy(cutoff_buffer.get(), cutoffParameterNode_.buffer(), frames * sizeof(float));

	const auto resonance_buffer = std::make_unique<float[]>(frames);
    memcpy(resonance_buffer.get(), resonanceParameterNode_.buffer(), frames * sizeof(float));

    const auto detune_buffer = std::make_unique<float []>(frames);
    memcpy(detune_buffer.get(), detuneParameterNode_.buffer(), frames * sizeof(float));

	for(unsigned int i = 0; i < frames; ++i) {

		const float current_cutoff = cutoff_buffer[i];
		const float current_resonance = resonance_buffer[i];
        const float current_detune = detune_buffer[i];

		// Recalculate coefficients for the current frame if cutoff or resonance has changed
		if(std::fabs( previousCutoff_ - current_cutoff) <= std::numeric_limits<float>::epsilon() || 
            std::fabs(previousResonance_ - current_resonance) <= std::numeric_limits<float>::epsilon() ||
            std::fabs(previousDetune_ - current_detune) <= std::numeric_limits<float>::epsilon()) {

            calculateCoefficients(current_cutoff, current_resonance, current_detune);

			previousCutoff_ = current_cutoff;
			previousResonance_ = current_resonance;
            previousDetune_ = current_detune;
		}

		// process the sample
        const float sample = m_input->buffer()[i];

		vibraSpeed_ += (sample - vibraPos_) * c_;
		vibraPos_ += vibraSpeed_;

		vibraSpeed_ *= r_;
		m_buffer[i] = vibraPos_;
	}



}



