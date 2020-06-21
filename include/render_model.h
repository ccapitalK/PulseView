//
// Author: Sahan Fernando <sahan.h.fernando@gmail.com>
// Date: 2020-06-06
//

#include <cassert>
#include <complex>

#include <SFML/Graphics.hpp>
#include <fftw3.h>

#include <fftw_helper.h>
#include <pulseview.h>

#pragma once

namespace PulseView {

enum class AudioChannel { Left = 0, Right };
extern AudioChannel AudioChannels[2];

using S16NESample = int16_t;
using Complex = std::complex<double>;

struct PCMChunk {
    void reserveSize(size_t log2NumSamples);
    void clear();
    double minInRange(size_t s, size_t e, size_t numSteps) const;
    double maxInRange(size_t s, size_t e, size_t numSteps) const;
    void calculateDFT(fftw::FFTWHelper &helper);
    double getDftValueOverRange(size_t s, size_t e, size_t numSteps) const;
    std::vector<double> samples;
    std::vector<double> dft;
    size_t log2Size;
};

struct Frame {
    Frame() = delete;
    Frame(size_t logNumSamples);
    void clear();
    void finalize();
    PCMChunk &getChunk(AudioChannel channel) noexcept;
    const PCMChunk &getChunk(AudioChannel channel) const noexcept;
    size_t log2Size;
    size_t numSamples;
    PCMChunk leftChunk;
    PCMChunk rightChunk;
    fftw::FFTWHelper fftw;
};

class RenderModel {
  public:
    RenderModel(sf::RenderWindow &window);
    void resize(size_t width, size_t height);
    void drawFrame(const Frame &frame);

  private:
    void prepareVertexArrays(size_t numQuadVertices, size_t numWaveVertices);
    sf::RenderWindow &window_;
    sf::VertexArray quadVertices_;
    sf::VertexArray waveVertices_;
    static inline const sf::Color waveColor{255, 255, 255, 255};
    static inline const sf::Color backgroundColor{29, 116, 239, 255};
    static inline const sf::Color fftColor{0, 93, 224, 255};
};

} // namespace PulseView
