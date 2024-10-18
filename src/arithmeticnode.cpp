#include "arithmeticnode.h"
#include <cstring>

ArithmeticNode::ArithmeticNode(AudioContext& context, Operation op, float value)
    : AudioNode(context), operation_(op), operation_automation_(context, value) {}


void ArithmeticNode::processInternal(unsigned int frames) {

    if (input_ == nullptr) {
        memset(buffer_.get(), 0, frames * sizeof(float));
        return;
    }

    input_->process(frames, last_processing_id_);

    // process the parameter nodes
    operation_automation_.process(frames, last_processing_id_);

    const auto value_buffer = std::make_unique<float []>(frames);
    memcpy(value_buffer.get(), operation_automation_.buffer(), frames * sizeof(float));

    for(unsigned i = 0; i < frames; ++i) {
        switch(operation_) {
        case Operation::Add:
            buffer_[i] = input_->buffer()[i] + value_buffer[i];
            break;

        case Operation::Divide:
            buffer_[i] = (operation_automation_.buffer()[i] == 0 ? 0 : value_buffer[i] / (operation_automation_.buffer()[i]));
            break;

        case Operation::Multiply:
            buffer_[i] = value_buffer[i] * operation_automation_.buffer()[i];
            break;

        case Operation::Subtract:
            buffer_[i] = input_->buffer()[i] - value_buffer[i];
            break;
        }
    }
}

void ArithmeticNode::addAutomation(AudioNode* node, unsigned int port) {
    switch(static_cast<Parameters>(port)) {
    case Parameters::ValueAutomation:
        operation_automation_.setInput(node);
        break;
    }
}

AudioNode * ArithmeticNode::removeAutomation(unsigned int port) {
    AudioNode* node = nullptr;
    switch(static_cast<Parameters>(port)) {
    case Parameters::ValueAutomation:
        node = operation_automation_.input();
        operation_automation_.setInput(nullptr);
    }

    return node;
}
