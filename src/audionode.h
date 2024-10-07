#pragma once

#include <memory>

class AudioNode {

public:
	explicit AudioNode();
	virtual ~AudioNode() = default;

	// don't need copy
	AudioNode(const AudioNode&) = delete;
	AudioNode& operator=(const AudioNode&) = delete;

	// don't need move
	AudioNode(AudioNode&&) = delete;
	AudioNode& operator=(AudioNode&&) = delete;

	void process(unsigned int frames, unsigned int processing_id);

    void setInput(AudioNode* node);
    AudioNode* input() const { return m_input; }

    float* buffer() const;


    // void automate(AudioNode *node, int index) { node->addAutomation(this, index); }

    template<typename ParamType>
    void automate(AudioNode* node, ParamType index) {
        node->addAutomation(this, static_cast<unsigned int>(index)); // Type-safe index
    }

    void connect(AudioNode *node) { node->setInput(this); }

protected:
    AudioNode* m_input;

    std::unique_ptr<float[]> m_buffer;

    unsigned int m_buffer_size = 0;
    unsigned int m_last_processing_id = -1;

protected:
    virtual void processInternal(unsigned int frames) = 0;

    virtual void addAutomation(AudioNode* node, unsigned int port = 0) = 0;
    virtual AudioNode * removeAutomation(unsigned int port = 0) = 0;

	void ensureBufferSize(unsigned int frames);

};
