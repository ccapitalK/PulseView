//
// Author: Sahan Fernando <sahan.h.fernando@gmail.com>
// Date: 2020-06-13
//

#pragma once

#include <limits>
#include <memory>
#include <new>
#include <vector>

#include <fftw3.h>

namespace PulseView::fftw {

struct FFTWComplex {
    fftw_complex value;
};

static_assert(sizeof(FFTWComplex) == sizeof(fftw_complex));

template <typename T> struct FFTWAllocator {
    typedef T value_type;
    static_assert(std::is_same<value_type, FFTWComplex>::value);

    FFTWAllocator() = default;
    template <typename U> constexpr FFTWAllocator(const FFTWAllocator<U> &) noexcept {}

    [[nodiscard]] T *allocate(std::size_t n) {
        if (n > std::numeric_limits<std::size_t>::max() / sizeof(T)) {
            throw std::bad_alloc();
        }

        if (auto p = static_cast<T *>(fftw_malloc(sizeof(T) * n))) {
            return p;
        }

        throw std::bad_alloc();
    }

    void deallocate(T *p, std::size_t) noexcept { fftw_free(p); }
};

using FFTWPlan = fftw_plan_s;

static_assert(std::is_same<FFTWPlan *, fftw_plan>::value);

struct FFTWHelper {
    FFTWHelper() = delete;
    FFTWHelper(size_t log2NumSamples);
    void calculateDFT(const std::vector<double> &in, std::vector<double> &out);
    size_t size;
    std::vector<FFTWComplex, FFTWAllocator<FFTWComplex>> fftw_in;
    std::vector<FFTWComplex, FFTWAllocator<FFTWComplex>> fftw_out;
    std::unique_ptr<FFTWPlan, void (*)(FFTWPlan *)> plan;
};

} // namespace PulseView::fftw
