//
// Author: Sahan Fernando <sahan.h.fernando@gmail.com>
// Date: 2020-05-16
//

#include <complex>
#include <csignal>
#include <cstring>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#include <SFML/Graphics.hpp>

#include <pulseview.h>

enum class AudioChannel { Left = 0, Right };

using S16LESample = int16_t;
using Complex = std::complex<double>;

struct PCMChunk {
  std::vector<Complex> samples;
  std::vector<Complex> dft;
  void reserveSize(size_t numSamples) {
    samples.reserve(numSamples);
    dft.resize(numSamples);
  }
  void clear() { samples.resize(0); }
  std::pair<double, double> lerpSample(double point) {
    if (samples.size() == 0) {
        throw std::runtime_error("Tried to index unpopulated pcm chunk");
    }
  }
  void calculateDFT() {
    for (auto &c : dft) {
      c = 0;
    }
  }
};

struct Frame {
  Frame() = delete;
  Frame(size_t logNumSamples) : numSamples(((size_t)1) << logNumSamples) {
    leftChunk_.reserveSize(numSamples);
    rightChunk_.reserveSize(numSamples);
  }
  void clear() {
    leftChunk_.clear();
    rightChunk_.clear();
  }
  void finalize() {
    leftChunk_.calculateDFT();
    rightChunk_.calculateDFT();
  }
  size_t numSamples;
  PCMChunk leftChunk_;
  PCMChunk rightChunk_;
  PCMChunk &getChunk(AudioChannel channel) noexcept {
    switch (channel) {
    case AudioChannel::Left:
      return leftChunk_;
    case AudioChannel::Right:
    default:
      return rightChunk_;
    }
  }
};

class RenderModel {
public:
  RenderModel(sf::RenderWindow &window) : window_(window) {}
  void drawFrame(Frame &frame) {
    window_.clear(sf::Color::Red);
    auto windowDimensions = window_.getSize();
  }

private:
  sf::RenderWindow &window_;
};

void fail_errno(std::string err, int err_no = errno) {
  err.append(strerror(err_no));
  throw std::runtime_error(err);
}

class PCMProcessSource {
public:
  PCMProcessSource() {
    int fildes[2];
    int status;
    if (pipe(fildes) < 0) {
      fail_errno("Failed to call pipe(): ");
    }
    pid_t pid = fork();
    if (pid < 0) {
      fail_errno("Failed to call fork(): ");
    } else if (pid == 0) {
      // we are the child
      dup2(fildes[1], STDOUT_FILENO);
      std::vector<const char *> procArgv;
      procArgv.push_back(static_cast<const char *>("pacat"));
      procArgv.push_back(static_cast<const char *>("--raw"));
      procArgv.push_back(static_cast<const char *>("--record"));
      procArgv.push_back(static_cast<const char *>("--latency-msec=10"));
      procArgv.push_back(static_cast<const char *>("--format=s16le"));
      procArgv.push_back(static_cast<const char *>("--rate=44100"));
      procArgv.push_back(nullptr);
      auto argv = const_cast<char**>(&procArgv[0]);
      execvp(procArgv[0], argv);
    } else {
      // we are the parent
      processFD_ = fildes[0];
      processPID_ = pid;
    }
  }
  ~PCMProcessSource() noexcept {
    kill(processPID_, SIGTERM);
    waitpid(processPID_, nullptr, 0);
  }
  void populateFrame(Frame &frame) {
    frame.clear();
    size_t bytesToRead = numChannels_ * sizeof(S16LESample) * frame.numSamples;
    std::vector<std::byte> buffer(bytesToRead);
    size_t bytesRead{0};
    while (bytesRead < bytesToRead) {
        ssize_t readResult = read(processFD_, buffer.data(), bytesToRead - bytesRead);
        if (readResult < 0) {
            fail_errno("Failed to read() from child process: ");
        } else {
            bytesRead += readResult;
        }
    }
    for(size_t i = 0; i < frame.numSamples; ++i) {
        char *offset = &reinterpret_cast<char*>(buffer.data())[i * sizeof(S16LESample) * numChannels_];
        S16LESample leftSample = *reinterpret_cast<S16LESample*>(offset);
        S16LESample rightSample = *reinterpret_cast<S16LESample*>(offset+sizeof(S16LESample));
        frame.getChunk(AudioChannel::Left).samples.push_back(leftSample);
        frame.getChunk(AudioChannel::Right).samples.push_back(rightSample);
    }
    frame.finalize();
  }

protected:
  std::string processCMDLine_;
  static const size_t numChannels_ = 2;
  int processFD_;
  pid_t processPID_;
};

int main() {
  try {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Hello world");
    sf::Event ev;
    RenderModel model{window};
    bool running{true};
    PCMProcessSource source;
    Frame frame(12ull);
    while (running) {
      source.populateFrame(frame);
      while (window.pollEvent(ev)) {
        switch (ev.type){
        case sf::Event::Closed:
          running = false;
          break;
        }
      }

      model.drawFrame(frame);
      window.display();
    }
  } catch (std::runtime_error &e) {
    std::cerr << "Critical error: " << e.what() << '\n';
    return 1;
  }
  return 0;
}
