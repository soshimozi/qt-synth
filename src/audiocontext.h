#ifndef AUDIOCONTEXT_H
#define AUDIOCONTEXT_H

#include <atomic>

class AudioContext
{
public:
    explicit AudioContext(float sampleRate) : sample_rate_(sampleRate) {}


    float sampleRate() const { return sample_rate_.load();}
    void setSampleRate(float sampleRate) { sample_rate_.store(sampleRate); }

private:
    std::atomic<float> sample_rate_;
};

#endif // AUDIOCONTEXT_H
