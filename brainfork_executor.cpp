//
// Created by rust7 on 21.12.2018.
//

#include <fstream>
#include <iostream>
#include <string>
#include <stack>
#include <utility>
#include <list>

#include "brainfork_executor.h"


BrainforkExecutor::BrainforkExecutor() : mMemory(nullptr) {};

void BrainforkExecutor::Execute(const std::string &filename) {
    ReadFile(filename);
    Optimize();
    Operate();
}

void BrainforkExecutor::ReadFile(const std::string &filename) {
    std::ifstream file(filename);
    if(!file.is_open()) {
        std::cout << "The specified file can't be opened\n";
        exit(1);
    }
    if(!mInstructions)
        mInstructions = std::make_shared<std::string>();
    std::string line;
    while(getline(file, line)) {
        (*mInstructions) += line;
    }
    file.close();
}

void BrainforkExecutor::Optimize() {
    if(!mInstructions)
        return;
    if(!mMemory)
        mMemory = new uint16_t[30000]{0};
    if(!mOperations)
        mOperations = std::make_shared<std::list<Operation>>();

    auto push_operation = [this](const OperationType &o_type, const int& o_value, bool no_repeat = false) {
        if(no_repeat || this->mOperations->empty() || this->mOperations->back().first != o_type) {
            this->mOperations->push_back(std::make_pair(o_type, o_value));
            return;
        }
        if(o_type == ZERO)
            return;
        this->mOperations->back().second += o_value;
        if(!this->mOperations->back().second) {
            this->mOperations->pop_back();
        }
    };

    for(size_t i = 0; i != mInstructions->length(); ++i) {
        char instruction = (*mInstructions)[i];
        /* 1. Так как операции </> отличаются лишь направлением сдвига, то можно считать их однотипными операциями,
         *      что позволит свернуть цепочки из операций </>.
         * 2. То же самое можно сказать и про операции +/-, их также свернем.
         * 3. Помимо этого был выделен еще один тип однотипных операций [+]/[-] - обнуление значения ячейки.
         *      Их также будем сворачивать.
         * 4. Операторы ./,/[/] никак не сворачиваем
         */
        switch(instruction) {
            case '>':
                push_operation(SHIFT, 1);
                break;
            case '<':
                push_operation(SHIFT, -1);
                break;
            case '+':
                push_operation(INC, 1);
                break;
            case '-':
                push_operation(INC, -1);
                break;
            case '.':
                push_operation(READ, 1, true);
                break;
            case ',':
                push_operation(WRITE, 1, true);
                break;
            case '[': {
                // "Вычленяем" операцию обнуления ячейки [+]/[-]
                if(i + 2 < mInstructions->length() && (*mInstructions)[i+2] == ']') {
                    const char& op = (*mInstructions)[i+1];
                    if(op == '+' || op == '-') {
                        push_operation(ZERO, 1);
                        i+=2;
                        break;
                    }
                }
                push_operation(L_BEG, 1, true);
            }
            break;
            case ']':
                push_operation(L_END, 1, true);
                break;
            default:
                break;
        }
    }
    mInstructions.reset();
}

void BrainforkExecutor::Operate() {
    if(!mOperations)
        return;
    if(!mMemory)
        mMemory = new uint16_t[30000]{0};
    std::stack<std::pair<std::list<std::pair<OperationType, int>>::iterator, uint16_t >> loops;
    for(auto iter = mOperations->begin(); iter != mOperations->end(); ++iter) {
        std::pair<OperationType, int> instruction = *iter;
        if(!loops.empty() && !loops.top().second
        && (instruction.first != L_BEG && instruction.first != L_END))
            continue;
        switch(instruction.first) {
            case SHIFT:
                mMemory += instruction.second;
                break;
            case ZERO:
                *mMemory = 0;
                break;
            case INC:
                (*mMemory) += instruction.second;
                break;
            case READ:
                // Печать значения текущей ячейки
                std::wcout << *mMemory;
                break;
            case WRITE:
                // Ввод значения в текущую ячейку
                std::wcin >> *mMemory;
                break;
            case L_BEG:
                // Начало цикла
                loops.push(std::make_pair(iter, *mMemory));
                break;
            case L_END:
                // Конец цикла
                {
                    if(loops.empty()) {
                        std::cout << "Incorrect program structure\n";
                        exit(2);
                    }
                    auto pos = loops.top().first;
                    if(!*mMemory)
                    {
                        loops.pop();
                        break;
                    }
                    loops.top().second = *mMemory;
                    iter = pos;
                }
                break;
            default:
                break;
        }
    }
    delete mMemory;
}
