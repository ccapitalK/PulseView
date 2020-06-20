//
// Author: Sahan Fernando <sahan.h.fernando@gmail.com>
// Date: 2020-06-20
//

#pragma once
#include "pulseview.h"
#include "render_model.h"

namespace PulseView::AudioSource {

class Source {
  public:
    virtual void populateFrame(PulseView::Frame &frame) = 0;
};

} // namespace PulseView::AudioSource
