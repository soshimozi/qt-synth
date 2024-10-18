#ifndef ARITHMETICNODE_H
#define ARITHMETICNODE_H

#include "audionode.h"
#include "automationnode.h"

class ArithmeticNode : public AudioNode
{
public:

    enum class Parameters {
        ValueAutomation = 0
    };


    enum class Operation {
        Add,
        Subtract,
        Multiply,
        Divide
    };

    explicit ArithmeticNode(AudioContext& context, Operation op = Operation::Add, float value = 0);

    void setValue(float value) { operation_automation_.setBaseValue(value); }

protected:
    virtual void processInternal(unsigned int frames);

    virtual void addAutomation(AudioNode* node, unsigned int port = 0);
    virtual AudioNode * removeAutomation(unsigned int port = 0);

private:
    AutomationNode operation_automation_;
    Operation operation_;
};

#endif // ARITHMETICNODE_H
