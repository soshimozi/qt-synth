#include "buffernode.h"
#include <algorithm>

void BufferNode::processInternal(unsigned frames) {
    ensureBufferSize(frames);

    // Zero the output buffer
    std::fill_n(buffer_.get(), frames, 0.0f);

    if(inputs_.size() == 0) return;

    // Calculate normalization factor
    float normalizationFactor = 1.0f / static_cast<float>(inputs_.size());

    // Sum the outputs of all input nodes with normalization
    for (auto input_node : inputs_) {
        input_node.node->process(frames, last_processing_id_);
        float* inputBuffer = input_node.node->buffer();

        for (unsigned int i = 0; i < frames; ++i) {
            buffer_[i] += ((inputBuffer[i] * input_node.gain) * normalizationFactor);
        }
    }
}

void BufferNode::addInput(AudioNode *node, float gain) {
    std::lock_guard<std::mutex> lock(input_mutex_);

    auto it = std::find_if(inputs_.begin(), inputs_.end(),
                           [node](const InputNodeInfo& info) {
                               return info.node == node;
                           });

    if (it == inputs_.end()) {
        inputs_.push_back({node, gain});
    }
}

void BufferNode::removeInput(AudioNode *node) {
    std::lock_guard<std::mutex> lock(input_mutex_);

    inputs_.erase(std::remove_if(inputs_.begin(), inputs_.end(),
                                    [node](const InputNodeInfo& info) {
                                        return info.node == node;
                                    }),
                     inputs_.end());

}

void BufferNode::setInputGain(AudioNode* node, float gain) {
    std::lock_guard<std::mutex> lock(input_mutex_);

    auto it = std::find_if(inputs_.begin(), inputs_.end(),
                           [node](const InputNodeInfo& info) {
                               return info.node == node;
                           });

    if(it != inputs_.end()) {
        (*it).gain = gain;
    }
}

