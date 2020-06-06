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
#include <pulseview.h>
#include <render_model.h>

int main() {
    try {
        sf::RenderWindow window(sf::VideoMode(800, 600), "PulseView");
        sf::Event ev;
        PulseView::RenderModel model{window};
        bool running{true};
        PulseView::AudioSource::PCMProcessSource source;
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
                    sf::FloatRect visibleArea(0, 0, ev.size.width, ev.size.height);
                    window.setView(sf::View(visibleArea));
                    break;
                }
                }
            }

            model.drawFrame(frame);
            window.display();
        }
    } catch (std::runtime_error &e) {
        std::cerr << "Encountered critical error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
