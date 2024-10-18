#ifndef AUTOMATEDAUDIONODE_H
#define AUTOMATEDAUDIONODE_H

#include "audionode.h"

class AutomatedAudioNode : public AudioNode
{
public:
    AutomatedAudioNode(AudioContext& context) : AudioNode(context) {}

public:
    template<typename ParamType>
    void automate(AutomatedAudioNode* node, ParamType index) {
        node->addAutomation(this, static_cast<unsigned int>(index)); // Type-safe index
    }

protected:
    virtual void addAutomation(AudioNode* node, unsigned int port = 0) = 0;
    virtual AudioNode * removeAutomation(unsigned int port = 0) = 0;

};

#endif // AUTOMATEDAUDIONODE_H
