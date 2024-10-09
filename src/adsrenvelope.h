#ifndef ADSRENVELOPE_H
#define ADSRENVELOPE_H

#include "envelopenode.h"

class ADSREnvelope : public EnvelopeNode
{
public:
    ADSREnvelope();

private:
    float attack_;
    float decay_;
    float sustain_;
    float release_;
};

#endif // ADSRENVELOPE_H
