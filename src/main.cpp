#include <iostream>
#include <string>
#include "shell_utils.h"

// ----- Main function -----

int main() {
    std::string input;
    while (true) {
        std::cout << "shell> ";
        std::getline(std::cin, input);
        handleCommand(input);
    }
    return 0;
}

