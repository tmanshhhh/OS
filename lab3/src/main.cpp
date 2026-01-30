#include "app.h"
#include <string>

int main(int argc, char* argv[]) {
    bool is_child = false;
    int child_mode = 0;

    if (argc == 3) {
        std::string flag = argv[1];
        if (flag == "--child") {
            is_child = true;
            child_mode = std::stoi(argv[2]);
        }
    }

    lab3::run_app(is_child, child_mode);
    return 0;
}
