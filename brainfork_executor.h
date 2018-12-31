//
// Created by rust7 on 21.12.2018.
//

#ifndef BRAINFORK_INTERPRETER_BRAINFORKEXECUTOR_H
#define BRAINFORK_INTERPRETER_BRAINFORKEXECUTOR_H

#include <memory>
#include <vector>

class BrainforkExecutor {
    std::shared_ptr<std::string> mInstructions;
    uint16_t* mMemory;

public:
    BrainforkExecutor();
    ~BrainforkExecutor() = default;

    /**
     * Запускаем скрипт на выполнение
     * @param filename
     */
    void Execute(const std::string& filename, bool optimize = true);

private:
    // Типы операций
    enum OperationType {
        SHIFT = 0,  // Сдвиг вправо/влево >/<
        INC,        // Увеличить/уменьшить значение ячейки +/-
        ZERO,       // Обнуление ячейки [+]/[-]
        L_BEG,      // Начало цикла [
        L_END,      // Конец цикла ]
        READ,       // Напечатать значение ячейки .
        WRITE,      // Записать значение в ячейку ,
        ADD,        // Добавить значение текущей ячейки в ячейку k(с обнулением текущей ячейки) [- SHIFT(k) + SHIFT(-k)]
        MOVE,       // Переместить значение текущей ячейки в ячейку k [SHIFT(k) ZERO SHIFT(-k) ADD(k)]
        COPY,       // Скопировать значение текущей ячейки в ячейку k [SHIFT(k) ZERO SHIFT(t) ZERO SHIFT(-k-t) [ -SHIFT(k) + SHIFT(t) + SHIFT(-k-t) ] SHIFT(k+t) MOVE(-k-t)]

    };
    typedef std::pair<OperationType, int> Operation;
    /**
     * Будем хранить вектор из операций в виде пары (ТипОперации, ЗначениеОперации)
     *      Например, (SHIFT, -5) - сдвиг влево на 5 ячеек,
     *      (INC, -2) - уменьшить на два значение ячейки
     */
    std::shared_ptr<std::vector<Operation>> mOperations;
    /**
     * Вычитываем файл в instructions
     * @param filename
     */
    void ReadFile(const std::string& filename);
    /**
     * Проанализируем текст скрипта, запишем легальные операции в mOperations
     * @param optimize выделяем операции ZERO и сворачиваем повторы
     */
    void GenerateCode(bool optimize = true);
    /**
     * Выполним операции из operation
     */
    void Operate();
    /**
     * Проверим цикл на реализацию алгоритма ADD
     * @param loop вектор с циклом для проверки
     * @return
     */
    bool IsAdd(const std::vector<Operation>& loop);
    /**
     * Проверим цикл на реализацию алгоритма MOVE
     * @param loop
     * @return
     */
    bool IsMove(const std::vector<Operation>& loop);
    /**
     * Проверим цикл на реализацию алгоритма COPY
     * @param loop
     * @return
     */
    bool IsCopy(const std::vector<Operation>& loop);
};


#endif //BRAINFORK_INTERPRETER_BRAINFORKEXECUTOR_H
