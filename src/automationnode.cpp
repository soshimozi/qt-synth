#include "automationnode.h"

#include <cstring>

AutomationNode::AutomationNode(const float initial_value) : static_value_(initial_value) {}

float AutomationNode::getStaticValue() const { return static_value_; }
void AutomationNode::setStaticValue(const float value) { static_value_ = value; }

void AutomationNode::processInternal(const unsigned int frames) {
    ensureBufferSize(frames);


    if (m_input != nullptr) {
        m_input->process(frames, m_last_processing_id);

        const float* input_buffer = m_input->buffer(); // Get the buffer from the input node

        // Copy the input  buffer into the parameter buffer
        memcpy(m_buffer.get(), input_buffer, frames * sizeof(float));
    }
    else {

        for (unsigned int i = 0; i < frames; ++i) {
            m_buffer[i] = static_value_;
        }
    }
}
