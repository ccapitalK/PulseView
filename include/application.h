//
// Author: Sahan Fernando <sahan.h.fernando@gmail.com>
// Date: 2020-06-20
//

#pragma once

#include <iostream>

#include <SFML/Graphics.hpp>

#include "pcm_process_source.h"
#include "pulseaudio_source.h"
#include "pulseview.h"
#include "render_model.h"
#include "source.h"

namespace PulseView {

class Application {
  public:
    Application() = delete;
    Application(sf::RenderWindow &window, AudioSource::Source &source, Frame &frame);
    void run();

  private:
    sf::RenderWindow &window_;
    RenderModel model_;
    Frame &frame_;
    AudioSource::Source &source_;
};

} // namespace PulseView
