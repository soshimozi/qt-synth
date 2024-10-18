#include "audionode.h"
#include "audiocontext.h"

AudioNode::AudioNode(AudioContext &context) : input_(nullptr), buffer_(nullptr), buffer_size_(0), context_(context) {}

void AudioNode::setInput(AudioNode *node) {
    input_ = node;
}

void AudioNode::process(const unsigned frames, const unsigned processing_id)
{
	if (last_processing_id_ == processing_id)
	{
		// node has already been processed this cycle; skip
		return;
	}

    last_processing_id_ = processing_id;

    ensureBufferSize(frames);
	processInternal(frames);
}

float* AudioNode::buffer() const
{
	return buffer_.get();
}

void AudioNode::ensureBufferSize(const unsigned frames)
{
    if(buffer_size_ < frames || buffer_ == nullptr)
    {
		buffer_ = std::make_unique<float[]>(frames);
		buffer_size_ = frames;
	}
}


