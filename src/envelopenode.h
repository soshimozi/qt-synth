#ifndef ENVELOPENODE_H
#define ENVELOPENODE_H

#include "audionode.h"
#include <vector>
#include <functional>

class EnvelopeNode : public AudioNode
{
public:
    EnvelopeNode(float gate = 1.0f,
                 const std::vector<float>& levels = {0.0f, 1.0f, 0.0f},
                 const std::vector<float>& times = {1.0f, 1.0f},
                 int releaseStage = -1,
                 std::function<void()> onComplete = nullptr,
                 float sampleRate = 44100.0f
                 );

    ~EnvelopeNode() = default;

    void setGate(float gate);
    void setLevels(const std::vector<float>& levels);
    void setTimes(const std::vector<float>& times);
    void setReleaseStage(int releaseStage);
    void setOnComplete(std::function<void()> onComplete);

protected:
    void processInternal(unsigned int frames) override;

    void addAutomation(AudioNode* node, unsigned int port = 0) override;
    AudioNode * removeAutomation(unsigned int port = 0) override;

private:
    double m_gate_value;
    std::vector<float> m_levels;
    std::vector<float> m_times;
    int m_release_stage;
    std::function<void()> m_on_complete;
};

#endif // ENVELOPENODE_H
