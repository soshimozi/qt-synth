#include "automationnode.h"

#include <cstring>

AutomationNode::AutomationNode(AudioContext& context, const float initial_value) : AudioNode(context), static_value_(initial_value) {}

float AutomationNode::getStaticValue() const { return static_value_; }
void AutomationNode::setStaticValue(const float value) { static_value_ = value; }

void AutomationNode::processInternal(const unsigned int frames) {

    if (input_ != nullptr) {
        input_->process(frames, last_processing_id_);

        const float* input_buffer = input_->buffer(); // Get the buffer from the input node

        // Copy the input  buffer into the parameter buffer
        memcpy(buffer_.get(), input_buffer, frames * sizeof(float));
    }
    else {

        for (unsigned int i = 0; i < frames; ++i) {
            buffer_[i] = static_value_;
        }
    }
}
