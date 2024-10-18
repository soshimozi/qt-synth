#include "automationnode.h"

#include <cstring>

AutomationNode::AutomationNode(AudioContext& context, const float base_value, Rate rate) : AudioNode(context), base_value_(base_value), rate_(rate) {}

void AutomationNode::setValueAtTime(float value, float time) {
    scheduled_events_.push({AutomationEvent::Type::SET, value, time});
}

void AutomationNode::linearRampValueAtTime(float value, float time) {
    scheduled_events_.push({AutomationEvent::Type::LINEAR_RAMP, value, time});
}

void AutomationNode::applyScheduledEvents(float time, unsigned frames) {
    float frameDuration = 1.0f / context_.sampleRate();

    for (unsigned int i = 0; i < frames; ++i) {
        float frameTime = time + i * frameDuration;

        // process each event in the priority queue
        while(!scheduled_events_.empty() && scheduled_events_.top().time <= frameTime) {
            const auto& event = scheduled_events_.top();

            switch(event.type) {
                case AutomationEvent::Type::SET:
                    base_value_ = event.value;
                    break;

                case AutomationEvent::Type::LINEAR_RAMP: {
                    if (!scheduled_events_.empty()) {
                        scheduled_events_.pop();
                        if (!scheduled_events_.empty()) {
                            const auto& next_event = scheduled_events_.top();
                            if (next_event.time > frameTime) {
                                // Interpolate between the current event and the next event
                                float t = (frameTime - event.time) / (next_event.time - event.time);
                                base_value_ = event.value + t * (next_event.value - event.value);
                            }
                        }
                    }
                    break;
                }
            }

            scheduled_events_.pop();

        }
    }
}

void AutomationNode::processInternal(const unsigned int frames) {
    float currentTime = static_cast<float>(last_processing_id_ * frames) / context_.sampleRate();

    if (input_ != nullptr) {
        input_->process(frames, last_processing_id_);

        const float* input_buffer = input_->buffer(); // Get the buffer from the input node

        // Copy the input  buffer into the parameter buffer
        memcpy(buffer_.get(), input_buffer, frames * sizeof(float));
    }
    else {

        if(rate_ == Rate::CONTROL_RATE) {
            applyScheduledEvents(currentTime, frames);
        }
        for (unsigned int i = 0; i < frames; ++i) {
            buffer_[i] = base_value_;
        }
    }
}

// Comparator for priority queue
bool operator<(const AutomationNode::AutomationEvent& a, const AutomationNode::AutomationEvent& b) {
    return a.time > b.time; // Min-heap based on event time
}
