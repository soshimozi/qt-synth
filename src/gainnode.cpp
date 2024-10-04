#include "gainnode.h"

#include <cstring>

GainNode::GainNode(const float gain) : gain_parameter_node_(gain) {}

void GainNode::setGain(const float gain) {
    this->gain_parameter_node_.setStaticValue(gain);
}

void GainNode::addAutomation(AudioNode* node, unsigned port) {

	if(port == 0) {
		gain_parameter_node_.setInput(node);
	}
}

AudioNode* GainNode::removeAutomation(unsigned port) {

	if(port == 0)	{
		AudioNode* node = gain_parameter_node_.getInput();
		gain_parameter_node_.setInput(nullptr);
		return node;
	}

	return nullptr;
}

void GainNode::processInternal(const unsigned frames) {
	ensureBufferSize(frames);


	gain_parameter_node_.process(frames, m_last_processing_id);

	if(m_input == nullptr) {
		memset(m_buffer.get(), 0, frames * sizeof(float));
		return;
	}

	m_input->process(frames, m_last_processing_id);

	const float* input_buffer = m_input->getBuffer();
	const float* gain_buffer = gain_parameter_node_.getBuffer();

	for(unsigned int i = 0; i < frames; i++) {
		m_buffer[i] = input_buffer[i] * gain_buffer[i];
	}
}


