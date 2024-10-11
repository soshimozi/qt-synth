#ifndef MULADDNODE_H
#define MULADDNODE_H

#include "audionode.h"
#include "automatedaudionode.h"
#include "automationnode.h"

class MulAddNode : public AudioNode
{
public:

    enum class Parameters {
        Multiply = 0,
        Add = 1
    };

    MulAddNode(AudioContext& context, float multiplier = 1.0f, float addend = 0.0f);

private:
    AutomationNode multiply_automation_;
    AutomationNode add_automation_;

protected:
    void processInternal(unsigned int frames) override;

    void addAutomation(AudioNode* node, unsigned int port = 0) override;
    AudioNode * removeAutomation(unsigned int port = 0) override;
};

#endif // MULADDNODE_H
