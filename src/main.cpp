/*
 * Build A Shell
 * Linux Exellenteam
 * Samah Rajahi 211558556
 * To run in linux:
 * g++ -Wall -o main main.cpp shell_utils.cpp
 * ./main
 */

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

