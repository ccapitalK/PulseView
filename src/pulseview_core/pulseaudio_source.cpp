//
// Author: Sahan Fernando <sahan.h.fernando@gmail.com>
// Date: 2020-06-20
//

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

PulseAudioSource::PulseAudioSource() : simple_(nullptr) {
    // TODO: Apparently, we need this ?
    // https://www.freedesktop.org/wiki/Software/PulseAudio/Documentation/Developer/Clients/ApplicationProperties/
    // setenv("PULSE_PROP_application.name", _("PulseView oscilloscope"), 1);
    // setenv("PULSE_PROP_application.icon_name", "pulseview", 1);
    // setenv("PULSE_PROP_media.role", "music", 1);

    pa_sample_spec ss;
    ss.format = PA_SAMPLE_S16NE;
    ss.channels = numChannels_;
    ss.rate = 61440;
    int error = 0;

    simple_ =
        pa_simple_new(nullptr, "PulseView", PA_STREAM_RECORD, nullptr, "Oscilloscope", &ss, nullptr, nullptr, &error);
    if (simple_ == nullptr) {
        fail_pulse("Failed to connect to pulseaudio: ", error);
    }
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
    if (pa_simple_read(simple_, buffer.data(), bytesToRead, &error) < 0) {
        fail_pulse("Failed to read from pulseaudio source", error);
    }
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
