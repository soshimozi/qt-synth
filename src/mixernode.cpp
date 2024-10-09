#include "mixernode.h"


MixerNode::MixerNode(unsigned num_inputs) : inputs_(num_inputs, nullptr), gains_(num_inputs, 1.0f) {}

void MixerNode::setGain(unsigned input_index, float gain) {

	if(input_index < gains_.size()) {
		gains_[input_index] = gain;
	}
}


void MixerNode::addMix(AudioNode* node, unsigned int index) {
	if (index < inputs_.size()) {
		inputs_[index] = node;
	}
}

AudioNode* MixerNode::removeInput(unsigned int index)
{
	if(index < inputs_.size())	{
		AudioNode* node = inputs_[index];
		inputs_[index] = nullptr;

		return node;
	}

	return nullptr;
}

void MixerNode::processInternal(unsigned frames) {
	ensureBufferSize(frames);

	std::fill(buffer_.get(), buffer_.get() + frames, 0.0f);

	for(size_t i = 0; i < inputs_.size(); ++i) {
		if (inputs_[i] != nullptr) {

			inputs_[i]->process(frames, last_processing_id_);

			const float* input_buffer = inputs_[i]->buffer();

			// Add the input to the mix buffer with the corresponding gain
			for(unsigned int j = 0; j < frames; ++j) {
				buffer_[j] += input_buffer[j] * gains_[i];
			}
		
		}
	}

}




