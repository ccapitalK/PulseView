//
// Author: Sahan Fernando <sahan.h.fernando@gmail.com>
// Date: 2020-06-06
//

#pragma once
#include <string>

#define die(msg) throw std::runtime_error(__FILE__ ":" + std::to_string(__LINE__) + ": " msg)
#define unimplemented() die("Unimplemented")

namespace PulseView {

void fail_errno(std::string err, int err_no = errno);

} // namespace PulseView
