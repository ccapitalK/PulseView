//
// Author: Sahan Fernando <sahan.h.fernando@gmail.com>
// Date: 2020-06-06
//

#pragma once
#include "pulseview.h"
#include "render_model.h"
#include "source.h"

namespace PulseView::AudioSource {

class PCMProcessSource : public Source {
  public:
    PCMProcessSource();
    ~PCMProcessSource();
    void populateFrame(PulseView::Frame &frame);

  private:
    std::string processCMDLine_;
    static const size_t numChannels_ = 2;
    int processFD_;
    pid_t processPID_;
};

} // namespace PulseView::AudioSource
