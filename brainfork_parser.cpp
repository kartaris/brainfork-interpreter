//
// Created by rust7 on 21.12.2018.
//

#include <fstream>
#include <iostream>
#include <string>
#include <stack>
#include <utility>

#include "brainfork_parser.h"

BrainforkExecutor::BrainforkExecutor() : memory(nullptr) {};

void BrainforkExecutor::Execute(const std::string &filename) {
    ReadFile(filename);
    GoThroughInstructions();
}

void BrainforkExecutor::ReadFile(const std::string &filename) {
    std::ifstream file(filename);
    if(!file.is_open()) {
        std::cout << "The specified file can't be opened\n";
        exit(1);
    }
    if(!instructions)
        instructions = std::make_shared<std::string>();
    std::string line;
    while(getline(file, line)) {
        (*instructions) += line;
    }
    file.close();
}

void BrainforkExecutor::GoThroughInstructions() {
    if(!instructions)
        return;
    if(!memory)
        memory = new wchar_t[30000]{0};
    std::stack<std::pair<size_t, wchar_t>> loops;
    for(size_t i = 0; i != (*instructions).length(); ++i) {
        char instruction = (*instructions)[i];
        if(!loops.empty() && !loops.top().second
        && (instruction != '[' && instruction != ']'))
            continue;
        switch(instruction) {
            case '>':
                // На ячейку вправо
                ++memory;
                break;
            case '<':
                // На ячейку влево
                --memory;
                break;
            case '+':
                // Увеличить значение ячейки на 1
                (*memory)++;
                break;
            case '-':
                // Уменьшить значение ячейки на 1
                (*memory)--;
                break;
            case '.':
                // Печать значения текущей ячейки
                std::wcout << *memory;
                break;
            case ',':
                // Ввод значения в текущую ячейку
                std::wcin >> *memory;
                break;
            case '[':
                // Начало цикла
                loops.push(std::make_pair(i, *memory));
                break;
            case ']':
                // Конец цикла
                {
                    if(loops.empty()) {
                        std::cout << "Incorrect program structure\n";
                        exit(2);
                    }
                    size_t pos = loops.top().first;
                    if(!*memory)
                    {
                        loops.pop();
                        break;
                    }
                    loops.top().second = *memory;
                    i = pos;
                }
                break;
            default:
                break;
        }
    }
    delete memory;
}