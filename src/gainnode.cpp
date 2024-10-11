#include "gainnode.h"

#include <cstring>

GainNode::GainNode(AudioContext& context, const float gain) : AudioNode(context), gain_automation_(context, gain) {}

void GainNode::addAutomation(AudioNode* node, unsigned port) {

    switch(static_cast<Parameters>(port)) {
    case Parameters::Gain:
        gain_automation_.setInput(node);
        break;
    }
}

AudioNode* GainNode::removeAutomation(unsigned port) {

    AudioNode* node = nullptr;

    switch(static_cast<Parameters>(port)) {
    case Parameters::Gain:
        node = gain_automation_.input();
        gain_automation_.setInput(nullptr);
        break;
    }

    return node;
}

void GainNode::processInternal(const unsigned frames) {

	if(input_ == nullptr) {
		memset(buffer_.get(), 0, frames * sizeof(float));
		return;
	}

    gain_automation_.process(frames, last_processing_id_);
    input_->process(frames, last_processing_id_);

	const float* input_buffer = input_->buffer();

    const auto gain_buffer = std::make_unique<float []>(frames);
    memcpy(gain_buffer.get(), gain_automation_.buffer(), frames * sizeof(float));

	for(unsigned int i = 0; i < frames; i++) {
        buffer_[i] = input_buffer[i] * gain_buffer[i];;
	}
}


