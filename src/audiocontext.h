#ifndef AUDIOCONTEXT_H
#define AUDIOCONTEXT_H

#include <atomic>

class AudioContext
{
public:
    explicit AudioContext(float sampleRate, unsigned frames) : sample_rate_(sampleRate), last_batch_id_(0), frames_(frames) {}

    float sampleRate() const { return sample_rate_.load();}
    void setSampleRate(float sampleRate) { sample_rate_.store(sampleRate); }

    int lastBatch() const { return last_batch_id_; }
    void updateBatch() { last_batch_id_ += 1; }

private:
    std::atomic<float> sample_rate_;
    std::atomic<int> last_batch_id_;
    std::atomic<unsigned> frames_;
};

#endif // AUDIOCONTEXT_H
