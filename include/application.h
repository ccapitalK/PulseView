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
#include "source.h"
#include "render_model.h"

namespace PulseView {

class Application {
public:
    Application();
    void run();
private:
    sf::RenderWindow window_;
    RenderModel model_;
    Frame frame_;
    AudioSource::PulseAudioSource source_;
};

} // namespace PulseView
