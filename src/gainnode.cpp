#include "gainnode.h"

#include <cstring>

GainNode::GainNode(const float gain) : gain_mod_node_(0), m_gain(gain) {}

void GainNode::setGain(const float gain) {
    m_gain = gain;
}

void GainNode::addAutomation(AudioNode* node, unsigned port) {

    if(static_cast<Parameters>(port) == Parameters::GainModulation) {
        gain_mod_node_.setInput(node);
	}
}

AudioNode* GainNode::removeAutomation(unsigned port) {

    if(static_cast<Parameters>(port) == Parameters::GainModulation)	{
        AudioNode* node = gain_mod_node_.input();
        gain_mod_node_.setInput(nullptr);
		return node;
	}

	return nullptr;
}

void GainNode::processInternal(const unsigned frames) {
	ensureBufferSize(frames);

    gain_mod_node_.process(frames, m_last_processing_id);

	if(m_input == nullptr) {
		memset(m_buffer.get(), 0, frames * sizeof(float));
		return;
	}

	m_input->process(frames, m_last_processing_id);

	const float* input_buffer = m_input->buffer();
    const float* gain_mod_buffer = gain_mod_node_.buffer();

    if(m_gain == 0) {
        memset(m_buffer.get(), 0, frames * sizeof(float));
        return;
    }

	for(unsigned int i = 0; i < frames; i++) {
        m_buffer[i] = input_buffer[i] * (m_gain + gain_mod_buffer[i]);
	}
}


