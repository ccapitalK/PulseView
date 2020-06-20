//
// Author: Sahan Fernando <sahan.h.fernando@gmail.com>
// Date: 2020-05-16
//

#include <complex>
#include <csignal>
#include <cstring>
#include <iostream>
#include <unistd.h>

#include <SFML/Graphics.hpp>

#include <pcm_process_source.h>
#include <pulseaudio_source.h>
#include <pulseview.h>
#include <render_model.h>
#include <source.h>

void run(PulseView::AudioSource::Source &source) {
    sf::RenderWindow window(sf::VideoMode(800, 600), "PulseView");
    sf::Event ev;
    PulseView::RenderModel model{window};
    bool running{true};
    PulseView::Frame frame(11ull);
    while (running) {
        source.populateFrame(frame);
        while (window.pollEvent(ev)) {
            switch (ev.type) {
            case sf::Event::Closed: {
                running = false;
                break;
            }
            case sf::Event::Resized: {
                model.resize(ev.size.width, ev.size.height);
                break;
            }
            default: break;
            }
        }

        model.drawFrame(frame);
        window.display();
    }
}

int main() {
    try {
        PulseView::AudioSource::PulseAudioSource source;
        run(static_cast<PulseView::AudioSource::Source &>(source));
    } catch (std::runtime_error &e) {
        std::cerr << "Encountered critical error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
