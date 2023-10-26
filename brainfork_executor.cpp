//
// Created by rust7 on 21.12.2018.
//

#include <fstream>
#include <iostream>
#include <stack>
#include <utility>
#include <vector>

#include "brainfork_executor.h"


BrainforkExecutor::BrainforkExecutor() : mMemory(nullptr) {};

bool BrainforkExecutor::execute(const std::string &filename, bool optimize) {
    mResult.clear();
    mOperations.reset();
    if(!readFile(filename))
        return false;
    generateCode(optimize);
    std::cout << mOperations->size() << " operations\n";
    return operate();
}

bool BrainforkExecutor::readFile(const std::string &filename) {
    std::ifstream file(filename);
    if(!file.is_open()) {
        std::cout << "The specified file can't be opened\n";
        return false;
    }
    if(!mInstructions)
        mInstructions = std::make_shared<std::string>();
    std::string line;
    while(getline(file, line)) {
        (*mInstructions) += line;
    }
    file.close();
    return true;
}

void BrainforkExecutor::generateCode(bool optimize) {
    if(!mInstructions)
        return;
    if(!mMemory)
        mMemory = new uint16_t[30000]{0};
    if(!mOperations)
        mOperations = std::make_shared<std::vector<Operation>>();
    auto in_loop_operations = std::stack<std::vector<Operation>>();
    auto push_operation = [this, &optimize, &in_loop_operations](const OperationType &o_type,
            const int& o_value, bool no_repeat = false) {
        auto& operations = optimize && !in_loop_operations.empty() ? in_loop_operations.top() : *(this->mOperations);
        if(!optimize || no_repeat || operations.empty() || operations.back().first != o_type) {
            operations.emplace_back(std::make_pair(o_type, o_value));
            return;
        }
        if(o_type == ZERO)
            return;
        operations.back().second += o_value;
        if(!operations.back().second) {
            operations.pop_back();
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
                // "Вычленяем" операцию обнуления ячейки [+]/[-], но только, если включены оптимизации
                if(optimize && i + 2 < mInstructions->length() && (*mInstructions)[i+2] == ']') {
                    const char& op = (*mInstructions)[i+1];
                    if(op == '+' || op == '-') {
                        push_operation(ZERO, 1);
                        i+=2;
                        break;
                    }
                }
                if(optimize)
                    in_loop_operations.emplace();
                push_operation(L_BEG, 1, true);
            }
            break;
            case ']':
                {
                    push_operation(L_END, 1, true);
                    if(!optimize)
                        break;
                    auto top_operations = in_loop_operations.top();
                    in_loop_operations.pop();
                    if(isAdd(top_operations))
                        push_operation(ADD, top_operations[2].second, true);
                    else if(isMult(top_operations))
                        push_operation(MULT, top_operations[3].second, true);
                    else if(isMove(top_operations))
                        push_operation(MOVE, top_operations[1].second, true);
                    else if(isCopy(top_operations))
                        push_operation(COPY, top_operations[1].second, true);
                    else {
                        auto &current_operations = (in_loop_operations.empty() ? *mOperations : in_loop_operations.top());
                        current_operations.insert(
                                current_operations.end(),
                                top_operations.begin(),
                                top_operations.end());
                    }
                }
                break;
            default:
                break;
        }
    }
    mInstructions.reset();
}

bool BrainforkExecutor::operate() {
    if(!mOperations)
        return true;

    if(!mMemory)
        mMemory = new uint16_t[30000]{0};

    auto memoryHead = mMemory;

    std::stack<std::pair<std::vector<Operation>::iterator, uint16_t>> loops;
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
                mResult += (wchar_t)*mMemory;
                break;
            case WRITE:
                // Ввод значения в текущую ячейку
                std::wcin >> *mMemory;
                break;
            case ADD:
            {
                *(mMemory + instruction.second) += *mMemory;
                *mMemory = 0;
                break;
            }
            case MULT:
            {
                *(mMemory + instruction.second) *= *mMemory;
                *mMemory = 0;
                break;
            }
            case MOVE:
            {
                *(mMemory + instruction.second) = *mMemory;
                *mMemory = 0;
                break;
            }
            case COPY:
            {
                *(mMemory + instruction.second) = *mMemory;
                break;
            }
            case L_BEG:
                // Начало цикла
                loops.push(std::make_pair(iter, *mMemory));
                break;
            case L_END:
            // Конец цикла
            {
                if(loops.empty()) {
                    std::cout << "Incorrect program structure\n";
                    return false;
                }
                auto pos = loops.top().first;
                if(!*mMemory)
                {
                    loops.pop();
                    break;
                }
                loops.top().second = *mMemory;
                iter = pos;
                break;
            }
            default:
                break;
        }
    }
    delete []memoryHead;
    mMemory = nullptr;
    return true;
}

bool BrainforkExecutor::isAdd(const std::vector<Operation>& loop) {
    // [INC(-1) SHIFT(k) INC(1) SHIFT(-k)]
    if(loop.size() != 6)
        return false;
    if(loop[1].first != INC || loop[3].first != INC)
        return false;
    if(loop[1].second != -1 || loop[3].second != 1)
        return false;
    if(loop[2].first != SHIFT || loop[4].first != SHIFT)
        return false;
    return loop[2].second == -(loop[4].second);
}

bool BrainforkExecutor::isMove(const std::vector<Operation>& loop) {
    // [SHIFT(k) ZERO SHIFT(-k) ADD(k)]
    if(loop.size() != 6)
        return false;
    if(loop[2].first != ZERO)
        return false;
    if(loop[5].first != ADD)
        return false;
    if(loop[1].first != SHIFT || loop[3].first != SHIFT)
        return false;
    return loop[1].second == -(loop[3].second) && loop[1].second == loop[5].second;
}

bool BrainforkExecutor::isCopy(const std::vector<Operation>& loop) {
    // [SHIFT(k) ZERO SHIFT(t) ZERO SHIFT(-k-t) [ *7- *8SHIFT(k) *9+ SHIFT(t) *11+ *12SHIFT(-k-t) ] *14SHIFT(k+t) MOVE(-k-t)]
    if(loop.size() != 17)
        return false;
    OperationType mask[] = {L_BEG, SHIFT,   ZERO,   SHIFT,  ZERO,   SHIFT,
                            L_BEG, INC,     SHIFT,  INC,    SHIFT,  INC,
                            SHIFT, L_END,   SHIFT,  MOVE,   L_END};
    for(size_t i = 1; i < 16; ++i) {
        if(loop[i].first != mask[i])
            return false;
    }
    int k = loop[1].second;
    int t = loop[3].second;
    if(loop[5].second != -k-t || loop[8].second != k
    || loop[10].second != t || loop[12].second != -k-t
    || loop[14].second != k+t || loop[15].second != -k-t )
        return false;
    return loop[7].second == -1 || loop[9].second == 1 || loop[11].second == 1;
}

bool BrainforkExecutor::isMult(const std::vector<Operation>& loop) {
    // [- SHIFT(k) INC(b) SHIFT(-k)]
    if(loop[1].first != INC || loop[1].second != -1)
        return false;
    if(loop[2].first != SHIFT || loop[4].first != SHIFT)
        return false;
    if(loop[2].second != -loop[4].first)
        return false;
    return loop[3].first == INC;
}

std::wstring BrainforkExecutor::result() {
    return mResult;
}
