#pragma once

#include "IMatrix.h"
#include <vector>
#include <stdexcept>
#include <fstream>
#include <iostream>

// Класс диагональной матрицы
template<typename T = double>
class MatrixDiagonal : public IMatrix {
private:
    unsigned _size;                
    std::vector<T> data;           
    std::vector<int> diagonals;   

public:
    
    MatrixDiagonal(unsigned size, const std::vector<int>& diagIndices);

   
    ~MatrixDiagonal() = default;

    
    T& operator()(unsigned i, unsigned j);
    const T& operator()(unsigned i, unsigned j) const;

    
    MatrixDiagonal operator+(const MatrixDiagonal& other) const;
    MatrixDiagonal operator-(const MatrixDiagonal& other) const;
    MatrixDiagonal elementwiseMultiplication(const MatrixDiagonal& other) const;
    MatrixDiagonal transpose() const;

    
    void exportToFile(const std::string& filename) const override;

    
    void importFromFile(const std::string& filename) override;

    
    void print() const override;

private:
    
    bool isValidIndex(unsigned i, unsigned j) const;
};
