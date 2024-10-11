#pragma once

#include "audionode.h"
#include <queue>

class AutomationNode final : public AudioNode {
public:
    enum class Rate {
        AUDIO_RATE,
        CONTROL_RATE
    };

    struct AutomationEvent {
        enum class Type {
            SET,
            LINEAR_RAMP
        };

        Type type;
        float value;
        float time;
    };

    explicit AutomationNode(AudioContext& context, float base_value = 0.0f, Rate rate = AutomationNode::Rate::CONTROL_RATE);

    void setBaseValue(const float value) { base_value_ = value; }
    float baseValue() const { return base_value_; }

    void setValueAtTime(float value, float time);
    void linearRampValueAtTime(float value, float time);
    void applyScheduledEvents(float currentTime, unsigned int frames);

protected:
    void processInternal(unsigned int frames) override;
    void addAutomation(AudioNode* node, unsigned port) override {}
    AudioNode* removeAutomation(unsigned port) override { return nullptr; }

private:
    float base_value_;

    Rate rate_;
    // Priority queue to store automation events sorted by time
    std::priority_queue<AutomationEvent> scheduled_events_;
};

// Comparator for priority queue
bool operator<(const AutomationNode::AutomationEvent& a, const AutomationNode::AutomationEvent& b);

