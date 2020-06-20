//
// Author: Sahan Fernando <sahan.h.fernando@gmail.com>
// Date: 2020-05-16
//

#include <cmath>
#include <iostream>

#include <fftw3.h>

#include <fftw_helper.h>
#include <pulseview.h>
#include <render_model.h>

namespace PulseView {

AudioChannel AudioChannels[2] = {AudioChannel::Left, AudioChannel::Right};

void PCMChunk::reserveSize(size_t log2NumSamples) {
    log2Size = log2NumSamples;
    const size_t size = 1 << log2Size;
    samples.reserve(size);
    dft.resize(size);
}

void PCMChunk::clear() { samples.resize(0); }

double PCMChunk::minInRange(size_t s, size_t e, size_t numSteps) const {
    assert(s <= e);
    assert(e <= numSteps);
    auto i1 = (s * samples.size()) / numSteps;
    auto i2 = (e * samples.size()) / numSteps;
    double rv = 1.;
    for (auto i = i1; i <= i2; ++i) {
        rv = std::min(rv, samples[i]);
    }
    return rv;
}

double PCMChunk::maxInRange(size_t s, size_t e, size_t numSteps) const {
    assert(s <= e);
    assert(e <= numSteps);
    auto i1 = (s * samples.size()) / numSteps;
    auto i2 = (e * samples.size()) / numSteps;
    double rv = -1.;
    for (auto i = i1; i <= i2; ++i) {
        rv = std::max(rv, samples[i]);
    }
    return rv;
}

void PCMChunk::calculateDFT(fftw::FFTWHelper &helper) { helper.calculateDFT(samples, dft); }

double PCMChunk::getDftValueOverRange(size_t s, size_t e, size_t numSteps) const {
    assert(s <= e);
    assert(e <= numSteps);
    size_t i1 = (s * s * samples.size()) / (2 * numSteps * numSteps);
    size_t i2 = (e * e * samples.size()) / (2 * numSteps * numSteps);
    double rv = 0.;
    for (auto i = i1; i <= i2; ++i) {
        rv += std::abs(dft[i]);
    }
    rv /= std::max(i2 - i1, (size_t)1);
    // make the returned value slightly nicer to parse
    return std::pow(rv, 0.7) / 32.;
}

Frame::Frame(size_t logNumSamples)
    : log2Size(logNumSamples), numSamples(((size_t)1) << logNumSamples), fftw(logNumSamples) {
    leftChunk.reserveSize(log2Size);
    rightChunk.reserveSize(log2Size);
}

void Frame::clear() {
    leftChunk.clear();
    rightChunk.clear();
}

void Frame::finalize() {
    leftChunk.calculateDFT(fftw);
    rightChunk.calculateDFT(fftw);
}

PCMChunk &Frame::getChunk(AudioChannel channel) noexcept {
    switch (channel) {
    case AudioChannel::Left:
        return leftChunk;
    case AudioChannel::Right:
    default:
        return rightChunk;
    }
}

const PCMChunk &Frame::getChunk(AudioChannel channel) const noexcept {
    switch (channel) {
    case AudioChannel::Left:
        return leftChunk;
    case AudioChannel::Right:
    default:
        return rightChunk;
    }
}

RenderModel::RenderModel(sf::RenderWindow &window) : window_(window) { texture_.create(1024, 1024); }

void RenderModel::resize(size_t width, size_t height) {
    auto textureWidth = texture_.getSize().x;
    sf::FloatRect visibleArea(0, 0, width, height);
    if (width > textureWidth || height > textureWidth) {
        while (textureWidth < std::max(width, height)) {
            textureWidth *= 2;
        }
        texture_.create(textureWidth, textureWidth);
    }
    window_.setView(sf::View(visibleArea));
}

void RenderModel::drawFrame(const Frame &frame) {
    texture_.clear(backgroundColor);
    auto windowDimensions = window_.getSize();
    auto width = windowDimensions.x;
    auto height = windowDimensions.y;
    // draw the fft
    for (auto channel : AudioChannels) {
        const auto &chunk = frame.getChunk(channel);
        const auto numRects = 128u;
        sf::RectangleShape rectangle;
        rectangle.setFillColor(fftColor);
        for (auto i = 0u; i < numRects; ++i) {
            double value = chunk.getDftValueOverRange(i, i + 1, numRects);
            auto y = value * height / 2.;
            auto x1 = (i * width) / numRects;
            auto x2 = ((i + 1) * width) / numRects;
            if (channel == AudioChannel::Left) {
                rectangle.setPosition(sf::Vector2f(x1, 0));
                rectangle.setSize(sf::Vector2f(x2 - x1, y));
            } else {
                rectangle.setPosition(sf::Vector2f(x1, height - y));
                rectangle.setSize(sf::Vector2f(x2 - x1, y));
            }
            texture_.draw(rectangle);
        }
    }
    // draw the audio waveform
    for (auto channel : AudioChannels) {
        auto &chunk = frame.getChunk(channel);
        sf::VertexArray line(sf::LineStrip, frame.numSamples + 1);
        line[0].position = sf::Vector2f(0., height / 2.);
        for (auto i = 0u; i < frame.numSamples; ++i) {
            double x = (i * width) / ((double)frame.numSamples);
            double y = (height * (1. - chunk.samples[i])) / 2.;
            line[i + 1].position = sf::Vector2f(x, y);
        }
        texture_.draw(line);
    }
    texture_.display();
    window_.draw(sf::Sprite(texture_.getTexture()));
}

} // namespace PulseView
