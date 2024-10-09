#include "gainnode.h"

#include <cstring>

GainNode::GainNode(const float gain) : gain_mod_node_(0), gain_(gain) {}

void GainNode::setGain(const float gain) {
    gain_ = gain;
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

    gain_mod_node_.process(frames, last_processing_id_);

	if(input_ == nullptr) {
		memset(buffer_.get(), 0, frames * sizeof(float));
		return;
	}

	input_->process(frames, last_processing_id_);

	const float* input_buffer = input_->buffer();
    const float* gain_mod_buffer = gain_mod_node_.buffer();

    if(gain_ == 0) {
        memset(buffer_.get(), 0, frames * sizeof(float));
        return;
    }

	for(unsigned int i = 0; i < frames; i++) {
        buffer_[i] = input_buffer[i] * (gain_ + gain_mod_buffer[i]);
	}
}


