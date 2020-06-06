//
// Author: Sahan Fernando <sahan.h.fernando@gmail.com>
// Date: 2020-06-06
//

#include <iostream>
#include <limits>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <pcm_process_source.h>

namespace PulseView::AudioSource {

PCMProcessSource::PCMProcessSource() {
    int fildes[2];
    int status;
    if (pipe(fildes) < 0) {
        PulseView::fail_errno("Failed to call pipe(): ");
    }
    pid_t pid = fork();
    if (pid < 0) {
        PulseView::fail_errno("Failed to call fork(): ");
    } else if (pid == 0) {
        // we are the child
        dup2(fildes[1], STDOUT_FILENO);
        std::vector<const char *> procArgv;
        procArgv.push_back(static_cast<const char *>("pacat"));
        procArgv.push_back(static_cast<const char *>("--raw"));
        procArgv.push_back(static_cast<const char *>("--record"));
        procArgv.push_back(static_cast<const char *>("--latency-msec=10"));
        procArgv.push_back(static_cast<const char *>("--format=s16le"));
        procArgv.push_back(static_cast<const char *>("--rate=61440"));
        procArgv.push_back(nullptr);
        auto argv = const_cast<char **>(&procArgv[0]);
        if (execvp(procArgv[0], argv) < 0) {
            PulseView::fail_errno("Failed to exec() source process: ");
        }
    } else {
        // we are the parent
        processFD_ = fildes[0];
        processPID_ = pid;
    }
}

PCMProcessSource::~PCMProcessSource() noexcept {
    kill(processPID_, SIGTERM);
    waitpid(processPID_, nullptr, 0);
}

void PCMProcessSource::populateFrame(PulseView::Frame &frame) {
    frame.clear();
    const size_t bytesToRead = numChannels_ * sizeof(PulseView::S16LESample) * frame.numSamples;
    std::vector<char> buffer(bytesToRead);
    size_t bytesRead{0};
    while (bytesRead < bytesToRead) {
        ssize_t readResult = read(processFD_, buffer.data() + bytesRead, bytesToRead - bytesRead);
        if (readResult < 0) {
            PulseView::fail_errno("Failed to read() from child process: ");
        } else {
            bytesRead += readResult;
        }
    }
    const auto *samples = reinterpret_cast<PulseView::S16LESample *>(buffer.data());
    const double maxSize = std::numeric_limits<PulseView::S16LESample>::max();
    for (size_t i = 0; i < frame.numSamples; ++i) {
        const auto *offset = samples + numChannels_ * i;
        const auto leftSample = ((double)offset[0]) / maxSize;
        const auto rightSample = ((double)offset[1]) / maxSize;
        frame.getChunk(PulseView::AudioChannel::Left).samples.push_back(leftSample);
        frame.getChunk(PulseView::AudioChannel::Right).samples.push_back(rightSample);
    }
    frame.finalize();
}

} // namespace PulseView::AudioSource
