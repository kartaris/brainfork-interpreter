#include <iostream>
#include <fstream>
#include <string>
#include "brainfork_executor.h"

int main(int argc, char *argv[]) {
    if(argc != 2) {
        std::cout << "Legal command is \"" << argv[0]
                  << "<script_path>" << "\"";
        return 1;
    }

    BrainforkExecutor{}.Execute(argv[1]);
    return 0;
}
