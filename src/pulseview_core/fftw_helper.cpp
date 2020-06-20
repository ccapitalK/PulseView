//
// Author: Sahan Fernando <sahan.h.fernando@gmail.com>
// Date: 2020-06-13
//

#include <cassert>
#include <iostream>

#include <fftw3.h>

#include <fftw_helper.h>

namespace PulseView::fftw {

FFTWHelper::FFTWHelper(size_t log2NumSamples)
    : size{((size_t)1) << log2NumSamples}, fftw_in{size}, fftw_out{size},
      plan(fftw_plan_dft_1d(size, reinterpret_cast<fftw_complex *>(fftw_in.data()),
                            reinterpret_cast<fftw_complex *>(fftw_out.data()), FFTW_FORWARD, FFTW_ESTIMATE),
           fftw_destroy_plan) {}

void FFTWHelper::calculateDFT(const std::vector<double> &in, std::vector<double> &out) {
    assert(in.size() == size);
    assert(out.size() == size);
    assert(fftw_in.size() == size);
    assert(fftw_out.size() == size);
    for (auto i = 0u; i < size; ++i) {
        fftw_in[i].value[0] = in[i];
        fftw_in[i].value[1] = 0;
    }
    fftw_execute(&*plan);
    for (auto i = 0u; i < size; ++i) {
        out[i] = fftw_out[i].value[0];
    }
}

} // namespace PulseView::fftw
