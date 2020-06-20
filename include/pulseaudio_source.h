//
// Author: Sahan Fernando <sahan.h.fernando@gmail.com>
// Date: 2020-06-20
//

#pragma once
#include "pulseview.h"
#include "render_model.h"
#include "source.h"

extern "C" struct pa_simple;

namespace PulseView::AudioSource {

class PulseAudioSource : public Source {
  public:
    PulseAudioSource();
    PulseAudioSource(const PulseAudioSource &) = delete;
    PulseAudioSource(PulseAudioSource &&) = delete;
    PulseAudioSource &operator=(const PulseAudioSource &) = delete;
    PulseAudioSource &operator=(PulseAudioSource &&) = delete;
    ~PulseAudioSource() noexcept;
    void populateFrame(PulseView::Frame &frame);

  private:
    pa_simple *simple_;
    static const size_t numChannels_ = 2;
};

} // namespace PulseView::AudioSource
