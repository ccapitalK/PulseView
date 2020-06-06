//
// Author: Sahan Fernando <sahan.h.fernando@gmail.com>
// Date: 2020-05-16
//

#include <iostream>

#include <pulseview.h>
#include <render_model.h>

namespace PulseView {

AudioChannel AudioChannels[2] = {AudioChannel::Left, AudioChannel::Right};

void RenderModel::drawFrame(const Frame &frame) {
    window_.clear(backgroundColor);
    auto windowDimensions = window_.getSize();
    auto width = windowDimensions.x;
    auto height = windowDimensions.y;
    for (auto channel : AudioChannels) {
        auto &chunk = frame.getChunk(channel);
        auto prevPosition = sf::Vector2f(0., height / 2.);
        for (auto i = 0u; i < frame.numSamples; ++i) {
            auto currPosition =
                sf::Vector2f((i * width) / ((double)frame.numSamples), (height * (1. - chunk.samples[i].real())) / 2.);
            sf::VertexArray line(sf::LineStrip, 2);
            line[0].position = prevPosition;
            line[1].position = currPosition;
            window_.draw(line);
            prevPosition = currPosition;
        }
    }
}

} // namespace PulseView
