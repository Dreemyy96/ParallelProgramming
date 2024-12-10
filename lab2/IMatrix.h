#pragma once

#include <string>

class IMatrix {
public:
    virtual ~IMatrix() = default;

    // экспорт матрицы в текстовый файл
    virtual void exportToFile(const std::string& filename) const = 0;

    // импорт матрицы из текстового файла
    virtual void importFromFile(const std::string& filename) = 0;

    // вывод матрицы в консоль
    virtual void print() const = 0;
};
