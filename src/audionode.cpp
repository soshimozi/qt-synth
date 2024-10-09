#include "audionode.h"

AudioNode::AudioNode() : m_input(nullptr), m_buffer(nullptr), m_buffer_size(0) {
}

void AudioNode::setInput(AudioNode *node) {
    m_input = node;
}

void AudioNode::process(const unsigned frames, const unsigned processing_id)
{
	if (m_last_processing_id == processing_id)
	{
		// node has already been processed this cycle; skip
		return;
	}

    m_last_processing_id = processing_id;
	processInternal(frames);
}

float* AudioNode::buffer() const
{
	return m_buffer.get();
}

void AudioNode::ensureBufferSize(const unsigned frames)
{
    if(m_buffer_size < frames || m_buffer == nullptr)
    {
		m_buffer = std::make_unique<float[]>(frames);
		m_buffer_size = frames;
	}
}


