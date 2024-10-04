#include "AudioPlayer.h"

#include <iostream>
#include <ostream>
#include <utility>


AudioPlayer::AudioPlayer(const void* user_data, const double sample_rate, const unsigned long frames_per_buffer)
    : stream_(nullptr), last_error_(paNoError), sample_rate_(sample_rate),
    frames_per_buffer_(frames_per_buffer), user_data_(user_data)
{
    // Initialize PortAudio
    last_error_ = Pa_Initialize();
    if (last_error_ != paNoError) {
        std::cerr << "PortAudio initialization failed: " << Pa_GetErrorText(last_error_) << "\n";
    }
}

AudioPlayer::~AudioPlayer()
{
    if (stream_) {
        Pa_CloseStream(stream_);
        stream_ = nullptr;
    }

    Pa_Terminate();
}

bool AudioPlayer::initializeStream()
{
    // Open an audio I/O stream
    last_error_ = Pa_OpenDefaultStream(&stream_,
        0,          // No input channels
        2,          // Stereo output
        paFloat32,  // 32-bit floating point output
        this->sample_rate_,
        this->frames_per_buffer_,  // Frames per buffer
        &AudioPlayer::paCallback,
        this);
    if (last_error_ != paNoError) {
        std::cerr << "Failed to open PortAudio stream: " << Pa_GetErrorText(last_error_) << "\n";
        return false;
    }
    return true;
}

bool AudioPlayer::start()
{
    last_error_ = Pa_StartStream(stream_);
    if (last_error_ != paNoError) {
        std::cerr << "Failed to start PortAudio stream: " << Pa_GetErrorText(last_error_) << "\n";
        return false;
    }
    return true;
}

bool AudioPlayer::stop()
{
    last_error_ = Pa_StopStream(stream_);
    if (last_error_ != paNoError) {
        std::cerr << "Failed to stop PortAudio stream: " << Pa_GetErrorText(last_error_) << "\n";
        return false;
    }
    return true;
}

void AudioPlayer::setSampleRate(const double sample_rate)
{ this->sample_rate_ = sample_rate; }

double AudioPlayer::getSampleRate() const
{ return this->sample_rate_; }

void AudioPlayer::setUserData(const void* user_data)
{ this->user_data_ = user_data; }

void AudioPlayer::setFramesPerBuffer(const unsigned long frames_per_buffer)
{ this->frames_per_buffer_ = frames_per_buffer; }

unsigned long AudioPlayer::getFramesPerBuffer() const
{ return this->frames_per_buffer_; }


void AudioPlayer::setCallback(std::function<void(const void* user_data, float* output, unsigned long frames_per_buffer)> callback)
{
    user_callback_ = std::move(callback);
}


int AudioPlayer::paCallback(const void* input_buffer, void* output_buffer,
    const unsigned long frames_per_buffer,
    const PaStreamCallbackTimeInfo* time_info,
    PaStreamCallbackFlags status_flags,
    void* user_data)
{
    const auto player = static_cast<AudioPlayer*>(user_data);
    const auto out = static_cast<float*>(output_buffer);

    if (player->user_callback_) {
        player->user_callback_(player->user_data_, out, frames_per_buffer);
    }
    return paContinue;
}