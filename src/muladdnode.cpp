#include "muladdnode.h"
#include <cstring>

MulAddNode::MulAddNode(AudioContext& context, float multpilier, float addend) : AudioNode(context), multiply_automation_(context, multpilier), add_automation_(context, addend)  {}


void MulAddNode::processInternal(unsigned int frames) {

    if(input_ == nullptr) {
        memset(buffer_.get(), 0, frames * sizeof(float));
        return;
    }

    multiply_automation_.process(frames, last_processing_id_);
    add_automation_.process(frames, last_processing_id_);

    const auto multiply_buffer = std::make_unique<float[]>(frames);
    memcpy(multiply_buffer.get(), multiply_automation_.buffer(), frames * sizeof(float));

    const auto add_buffer = std::make_unique<float[]>(frames);
    memcpy(add_buffer.get(), add_automation_.buffer(), frames * sizeof(float));

    input_->process(frames, last_processing_id_);
    const float* input_buffer = input_->buffer();

    for(unsigned int i = 0; i < frames; i++) {
        buffer_[i] = input_buffer[i] * multiply_buffer[i] + add_buffer[i];
    }
}

void MulAddNode::addAutomation(AudioNode* node, unsigned port) {
    switch(static_cast<Parameters>(port)) {
    case Parameters::Multiply:
        multiply_automation_.setInput(node);
        break;

    case Parameters::Add:
        add_automation_.setInput(node);
        break;
    }
}

AudioNode* MulAddNode::removeAutomation(unsigned port) {
    AudioNode* node = nullptr;
    switch(static_cast<Parameters>(port)) {
    case Parameters::Add:
        node = add_automation_.input();
        add_automation_.setInput(nullptr);
        break;

    case Parameters::Multiply:
        node = multiply_automation_.input();
        multiply_automation_.setInput(nullptr);
        break;
    }

    return node;
}
