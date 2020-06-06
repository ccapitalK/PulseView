//
// Author: Sahan Fernando <sahan.h.fernando@gmail.com>
// Date: 2020-06-06
//

#include <cstring>
#include <stdexcept>

#include "pulseview.h"

namespace PulseView {

void fail_errno(std::string err, int err_no) {
    err.append(strerror(err_no));
    throw std::runtime_error(err);
}

} // namespace PulseView
