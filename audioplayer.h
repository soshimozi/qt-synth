#pragma once

#include <functional>
#include <portaudio.h>

class AudioPlayer {

public:
	explicit AudioPlayer(const void* user_data = nullptr, double sample_rate = 44100.0, unsigned long frames_per_buffer = 256);
    ~AudioPlayer();

    bool initializeStream();
    bool start();
    bool stop();

    void setSampleRate(double sample_rate);
    double getSampleRate() const;

    void setUserData(const void* user_data);

    void setFramesPerBuffer(unsigned long frames_per_buffer);
    unsigned long getFramesPerBuffer() const;

    void setCallback(std::function<void(const void* user_data, float* output, unsigned long frames_per_buffer)> callback);

private:
    static int paCallback(const void* input_buffer, void* output_buffer,
        unsigned long frames_per_buffer,
        const PaStreamCallbackTimeInfo* time_info,
        PaStreamCallbackFlags status_flags,
        void* user_data);

    PaStream* stream_;
    PaError last_error_;
    double sample_rate_;
    unsigned long frames_per_buffer_;
    const void* user_data_;

    std::function<void(const void* user_data, float* output, unsigned long frames_per_buffer)> user_callback_;
};
