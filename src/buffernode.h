#ifndef BUFFERNODE_H
#define BUFFERNODE_H

#include "audionode.h"
#include <mutex>
#include <vector>

class BufferNode : public AudioNode
{
public:
    BufferNode() = default;

    void addInput(AudioNode* node, float gain);
    void removeInput(AudioNode* node);

    void setInputGain(AudioNode* node, float gain);

    struct InputNodeInfo {
        AudioNode * node;
        float gain;
    };

protected:
    void processInternal(unsigned frames) override;
    void addAutomation(AudioNode* node, unsigned port) override {}
    AudioNode* removeAutomation(unsigned port) override { return nullptr; }

private:
    std::vector<InputNodeInfo> inputs_;
    std::mutex input_mutex_;
};

#endif // BUFFERNODE_H
