//
// Created by rust7 on 21.12.2018.
//

#ifndef BRAINFORK_INTERPRETER_BRAINFORKPARSER_H
#define BRAINFORK_INTERPRETER_BRAINFORKPARSER_H

#include <memory>

class BrainforkExecutor {
    std::shared_ptr<std::string> instructions;
    wchar_t* memory;

public:
    BrainforkExecutor();
    ~BrainforkExecutor() = default;

    /**
     * Запускаем скрипт на выполнение
     * @param filename
     */
    void Execute(const std::string& filename);

private:
    /**
     * Вычитываем файл в instructions
     * @param filename
     */
    void ReadFile(const std::string& filename);
    /**
     * Пробегаем по инструкциям
     */
    void GoThroughInstructions();
};


#endif //BRAINFORK_INTERPRETER_BRAINFORKPARSER_H
