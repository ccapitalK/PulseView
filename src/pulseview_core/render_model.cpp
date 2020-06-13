//
// Author: Sahan Fernando <sahan.h.fernando@gmail.com>
// Date: 2020-05-16
//

#include <iostream>

#include <pulseview.h>
#include <render_model.h>

namespace PulseView {

AudioChannel AudioChannels[2] = {AudioChannel::Left, AudioChannel::Right};

void PCMChunk::reserveSize(size_t log2NumSamples) {
    log2Size = log2NumSamples;
    samples.reserve(1 << log2Size);
    dft.resize(1 << log2Size);
}

void PCMChunk::clear() { samples.resize(0); }

double PCMChunk::minInRange(size_t s, size_t e, size_t numSteps) const {
    assert(s <= e);
    assert(e <= numSteps);
    auto i1 = (s * samples.size()) / numSteps;
    auto i2 = (e * samples.size()) / numSteps;
    double rv = 1.;
    for (auto i = i1; i <= i2; ++i) {
        rv = std::min(rv, samples[i].real());
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
        rv = std::max(rv, samples[i].real());
    }
    return rv;
}

void PCMChunk::calculateDFT() {
    assert(samples.size() == ((size_t)1) << log2Size);
    assert(dft.size() == ((size_t)1) << log2Size);
    // copy sample array into dft buffer
    std::copy(samples.begin(), samples.end(), dft.begin());
    //sort by reverse byte order
    //In this case, we are doing this in place
    for(size_t i = 0; i < samples.size(); ++i){
        size_t rev = 0, i2 = i;
        for(size_t j = 0; j < log2Size; ++j){
            rev<<=1;
            rev |= i2&1;
            i2>>=1;
        }
        if(i<rev){
            std::swap(dft[i], dft[rev]);
        }
    }
    //Perform fft
    size_t mmax = 1;
    while(mmax < dft.size()){
        size_t istep = 2 * mmax;
        double theta = -M_PI/mmax;
        double temp = sin(theta/2.0);
        Complex wp{-2.0*temp*temp, sin(theta)};
        Complex w{1.0, 0.0};
        for(unsigned int m=0; m<mmax; ++m){
            for(unsigned int i=m; i<dft.size(); i+=istep){
                unsigned int j = i+mmax;
                Complex temp = w * dft[j];
                dft[j]=dft[i]-temp;
                dft[i]=dft[i]+temp;
            }
            w = w * wp + w;
        }
        mmax = istep;
    }
}

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
    return sqrt(rv)/32.;
}

Frame::Frame(size_t logNumSamples) : log2Size(logNumSamples), numSamples(((size_t)1) << logNumSamples) {
    leftChunk.reserveSize(log2Size);
    rightChunk.reserveSize(log2Size);
}

void Frame::clear() {
    leftChunk.clear();
    rightChunk.clear();
}

void Frame::finalize() {
    leftChunk.calculateDFT();
    rightChunk.calculateDFT();
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

void RenderModel::drawFrame(const Frame &frame) {
    window_.clear(backgroundColor);
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
            double value = chunk.getDftValueOverRange(i, i+1, numRects);
            auto y = value * height / 2.;
            auto x1 = (i * width) / numRects;
            auto x2 = ((i+1) * width) / numRects;
            if (channel == AudioChannel::Left) {
                rectangle.setPosition(sf::Vector2f(x1, 0));
                rectangle.setSize(sf::Vector2f(x2-x1, y));
            } else {
                rectangle.setPosition(sf::Vector2f(x1, height - y));
                rectangle.setSize(sf::Vector2f(x2-x1, y));
            }
            window_.draw(rectangle);
        }
    }
    // draw the audio waveform
    for (auto channel : AudioChannels) {
        auto &chunk = frame.getChunk(channel);
        sf::VertexArray line(sf::LineStrip, frame.numSamples+1);
        line[0].position = sf::Vector2f(0., height / 2.);
        for (auto i = 0u; i < frame.numSamples; ++i) {
            double x = (i * width) / ((double)frame.numSamples);
            double y = (height * (1. - chunk.samples[i].real())) / 2.;
            line[i+1].position = sf::Vector2f(x, y);
        }
        window_.draw(line);
    }
}

} // namespace PulseView
