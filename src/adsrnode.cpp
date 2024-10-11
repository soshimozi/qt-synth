#include "adsrnode.h"
#include <algorithm>

ADSRNode::ADSRNode(AudioContext& context, float attack, float decay, float sustain, float release) : AudioNode(context),
    gate_automation_(context, 0.0), attack_(attack), decay_(decay), sustain_(sustain),
    release_(release), envelope_level_(0.0f), state_(State::Idle)
{}


void ADSRNode::setAttack(float attack) {
    attack_ = std::max(0.0001f, attack);
}

void ADSRNode::setDecay(float decay) {
    decay_ = std::max(0.0001f,  decay);
}

void ADSRNode::setSustain(float sustain) {
    sustain_ = std::clamp(sustain, 0.0f, 1.0f);
}

void ADSRNode::setRelease(float release) {
    release_ = std::max(0.0001f,  release);
}

void ADSRNode::setGate(bool gate) {
    gate_automation_.setBaseValue(gate ? 1.0f : 0.0f);
}


void ADSRNode::processInternal(unsigned frames) {

    auto sampleRate = context_.sampleRate();

    gate_automation_.process(frames, last_processing_id_);

    const auto gate_buffer = std::make_unique<float[]>(frames);
    memcpy(gate_buffer.get(), gate_automation_.buffer(), frames * sizeof(float));

    for(unsigned i = 0; i < frames; ++i) {
        auto gate = (gate_buffer[i] != 0);

        if(gate && state_ == State::Idle) {
            state_ = State::Attack;
        } else if(!gate && state_ != State::Idle && state_ != State::Release) {
            state_ = State::Release;
        }

        switch(state_) {
        case State::Idle:
            envelope_level_ = 0.0f;
            if(gate) {
                state_ = State::Attack;
            }
            break;

        case State::Attack:
            envelope_level_ += 1.0f / (attack_ * sampleRate);
            if(envelope_level_ >= 1.0f) {
                envelope_level_ = 1.0f;
                state_ = State::Decay;
            }
            break;

        case State::Decay:
            envelope_level_ -= (1.0f - sustain_) / (decay_ * sampleRate);
            if(envelope_level_ <= sustain_) {
                envelope_level_ = sustain_;
                state_ = State::Sustain;
            }
            break;

        case State::Sustain:
            envelope_level_ = sustain_;
            if(!gate) {
                state_ = State::Release;
            }
            break;

        case State::Release:
            envelope_level_ -= sustain_ / (release_ * sampleRate);
            if(envelope_level_ <= 0.0f) {
                envelope_level_ = 0.0f;
                state_ = State::Idle;
            }
            break;
        }

        buffer_[i] = envelope_level_;
    }

}

void ADSRNode::addAutomation(AudioNode* node, unsigned port) {
    switch(static_cast<Parameters>(port))
    {
    case Parameters::GateAutomation:
        gate_automation_.setInput(node);
        break;
    }
}

AudioNode* ADSRNode::removeAutomation(unsigned port) {

    AudioNode* node = nullptr;
    switch(static_cast<Parameters>(port)) {
    case Parameters::GateAutomation:
        node = gate_automation_.input();
        gate_automation_.setInput(nullptr);
        break;
    }

    return node;
}
