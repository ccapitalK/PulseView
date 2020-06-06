//
// Author: Sahan Fernando <sahan.h.fernando@gmail.com>
// Date: 2020-06-06
//

#pragma once
#include "pulseview.h"
#include "render_model.h"

namespace PulseView::AudioSource {

class PCMProcessSource {
  public:
    PCMProcessSource();
    ~PCMProcessSource();
    void populateFrame(PulseView::Frame &frame);

  protected:
    std::string processCMDLine_;
    static const size_t numChannels_ = 2;
    int processFD_;
    pid_t processPID_;
};

} // namespace PulseView::AudioSource
