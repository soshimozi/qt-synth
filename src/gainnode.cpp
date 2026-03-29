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
    if (input_ == nullptr) {
        memset(buffer_.get(), 0, frames * sizeof(float));
        return;
    }

    gain_automation_.process(frames, last_processing_id_);
    input_->process(frames, last_processing_id_);

    const float* input_buffer = input_->buffer();
    const float* gain_buffer = gain_automation_.buffer();
    const float base_gain = gain_automation_.baseValue();

    for (unsigned int i = 0; i < frames; i++) {
        const float gain = std::max(0.0f, base_gain + gain_buffer[i]);
        buffer_[i] = input_buffer[i] * gain;
    }
}

