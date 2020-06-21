//
// Author: Sahan Fernando <sahan.h.fernando@gmail.com>
// Date: 2020-06-20
//

#include "application.h"
#include "pulseview.h"

namespace PulseView {

Application::Application(sf::RenderWindow &window, AudioSource::Source &source, Frame &frame)
    : window_{window}, model_{window_}, frame_{frame}, source_{source} {}

void Application::run() {
    sf::Event ev;
    bool running{true};
    while (running) {
        source_.populateFrame(frame_);
        while (window_.pollEvent(ev)) {
            switch (ev.type) {
            case sf::Event::Closed: {
                running = false;
                break;
            }
            case sf::Event::Resized: {
                model_.resize(ev.size.width, ev.size.height);
                break;
            }
            default:
                break;
            }
        }

        model_.drawFrame(frame_);
        window_.display();
    }
}

} // namespace PulseView
