#include "mixernode.h"
#include <algorithm>


//MixerNode::MixerNode(unsigned num_inputs) {}

// void MixerNode::setGain(unsigned input_index, float gain) {

// 	if(input_index < gains_.size()) {
// 		gains_[input_index] = gain;
// 	}
// }


// void MixerNode::addMix(AudioNode* node, unsigned int index) {
// 	if (index < inputs_.size()) {
// 		inputs_[index] = node;
// 	}
// }

// AudioNode* MixerNode::removeInput(unsigned int index)
// {
// 	if(index < inputs_.size())	{
// 		AudioNode* node = inputs_[index];
// 		inputs_[index] = nullptr;

// 		return node;
// 	}

// 	return nullptr;
// }

void MixerNode::processInternal(unsigned frames) {

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


void MixerNode::addInput(AudioNode *node, float gain) {
    std::lock_guard<std::mutex> lock(input_mutex_);

    auto it = std::find_if(inputs_.begin(), inputs_.end(),
                           [node](const InputNodeInfo& info) {
                               return info.node == node;
                           });

    if (it == inputs_.end()) {
        inputs_.push_back({node, gain});
    }
}

void MixerNode::removeInput(AudioNode *node) {
    std::lock_guard<std::mutex> lock(input_mutex_);

    inputs_.erase(std::remove_if(inputs_.begin(), inputs_.end(),
                                 [node](const InputNodeInfo& info) {
                                     return info.node == node;
                                 }),
                  inputs_.end());

}

void MixerNode::setInputGain(AudioNode* node, float gain) {
    std::lock_guard<std::mutex> lock(input_mutex_);

    auto it = std::find_if(inputs_.begin(), inputs_.end(),
                           [node](const InputNodeInfo& info) {
                               return info.node == node;
                           });

    if(it != inputs_.end()) {
        (*it).gain = gain;
    }
}


