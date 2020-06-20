//
// Author: Sahan Fernando <sahan.h.fernando@gmail.com>
// Date: 2020-05-16
//

#include <application.h>

int main() {
    try {
        PulseView::Application app;
        app.run();
    } catch (std::runtime_error &e) {
        std::cerr << "Encountered critical error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
