//
// Author: Sahan Fernando <sahan.h.fernando@gmail.com>
// Date: 2020-06-20
//

#include <iostream>
#include <stdexcept>
#include <stdlib.h>
#include <string.h>

#include <pulse/error.h>
#include <pulse/simple.h>

#include "pulseaudio_source.h"
#include "pulseview.h"
#include "render_model.h"

namespace PulseView::AudioSource {

void fail_pulse(std::string err, int pulseErrorCode) { throw std::runtime_error(err + pa_strerror(pulseErrorCode)); }

PulseAudioSource::PulseAudioSource(size_t audioRate) : simple_(nullptr) {
    pa_sample_spec ss;
    ss.format = PA_SAMPLE_S16NE;
    ss.channels = numChannels_;
    ss.rate = audioRate;
    int error = 0;

    simple_ =
        pa_simple_new(nullptr, "PulseView", PA_STREAM_RECORD, nullptr, "Oscilloscope", &ss, nullptr, nullptr, &error);
    if (simple_ == nullptr) {
        fail_pulse("Failed to connect to pulseaudio: ", error);
    }
}

PulseAudioSource::PulseAudioSource(PulseAudioSource &&other) {
    simple_ = other.simple_;
    other.simple_ = nullptr;
}

PulseAudioSource &PulseAudioSource::operator=(PulseAudioSource &&other) {
    simple_ = other.simple_;
    other.simple_ = nullptr;
    return *this;
}

PulseAudioSource::~PulseAudioSource() noexcept {
    if (simple_) {
        pa_simple_free(simple_);
        simple_ = nullptr;
    }
}

void PulseAudioSource::populateFrame(PulseView::Frame &frame) {
    frame.clear();
    const size_t bytesToRead = numChannels_ * sizeof(PulseView::S16NESample) * frame.numSamples;
    std::vector<char> buffer(bytesToRead);
    int error = 0;
    // TODO: Move to async API, avoids lag on first read
    // std::cout << "PA: Starting read\n";
    if (pa_simple_read(simple_, buffer.data(), bytesToRead, &error) < 0) {
        fail_pulse("Failed to read from pulseaudio source", error);
    }
    // std::cout << "PA: Finished read\n";
    const auto *samples = reinterpret_cast<PulseView::S16NESample *>(buffer.data());
    const double maxSize = std::numeric_limits<PulseView::S16NESample>::max();
    for (size_t i = 0; i < frame.numSamples; ++i) {
        const auto *offset = samples + numChannels_ * i;
        const auto leftSample = ((double)offset[0]) / maxSize;
        const auto rightSample = ((double)offset[1]) / maxSize;
        frame.getChunk(PulseView::AudioChannel::Left).samples.push_back(leftSample);
        frame.getChunk(PulseView::AudioChannel::Right).samples.push_back(rightSample);
    }
    frame.finalize();
}

} // namespace PulseView::AudioSource
