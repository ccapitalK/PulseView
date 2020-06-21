//
// Author: Sahan Fernando <sahan.h.fernando@gmail.com>
// Date: 2020-05-16
//

#include <string>
#include <vector>

#include <cxxopts.hpp>

#include <application.h>

int main(int argc, char *argv[]) {
    cxxopts::Options options{argv[0], "Simple graphical oscilloscope for pulseaudio"};
    try {
        size_t width = 800, height = 600;
        size_t log2FrameWidth = 11;
        size_t frameRate = 60;

        using DimensionVec = std::vector<size_t>;
        options.add_options()("h,help", "Show help message", cxxopts::value<bool>())(
            "d,dimensions", "Initial window dimensions",
            cxxopts::value<DimensionVec>())("f,frame-rate", "Number of updates per second", cxxopts::value<size_t>())(
            "w,log2-frame-width", "Log in base 2 of the number of samples shown on the screen at once",
            cxxopts::value<size_t>());
        auto result = options.parse(argc, argv);
        if (result.count("help")) {
            std::cout << options.help() << '\n';
            return 0;
        }
        if (result.count("dimensions")) {
            DimensionVec v = result["dimensions"].as<DimensionVec>();
            if (v.size() != 2) {
                throw cxxopts::OptionParseException("expected comma seperated pair of values for dimensions");
            }
            width = v[0];
            height = v[1];
        }
        if (result.count("log2-frame-width")) {
            log2FrameWidth = result["log2-frame-width"].as<size_t>();
            if (log2FrameWidth < 8 || log2FrameWidth > 12) {
                throw cxxopts::OptionParseException("log2-frame-width is out of range [8..12]");
            }
        }
        if (result.count("frame-rate")) {
            frameRate = result["frame-rate"].as<size_t>();
            if (frameRate < 1 || frameRate > 120) {
                throw cxxopts::OptionParseException("frame-rate is out of range [1..120]");
            }
        }

        sf::RenderWindow window{sf::VideoMode(width, height), "PulseView"};
        PulseView::Frame frame{log2FrameWidth};
        PulseView::AudioSource::PulseAudioSource source{frameRate * (1u << log2FrameWidth)};

        PulseView::Application app{window, source, frame};
        app.run();
    } catch (cxxopts::OptionParseException &e) {
        std::cout << options.help() << '\n';
        std::cerr << "Encountered critical error parsing options: " << e.what() << '\n';
        return 1;
    } catch (std::runtime_error &e) {
        std::cerr << "Encountered critical error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
