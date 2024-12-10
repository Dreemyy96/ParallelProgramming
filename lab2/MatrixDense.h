#pragma once

#include "IMatrix.h"
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <vector>

// Класс плотной матрицы
template<typename T = double>
class MatrixDense : public IMatrix {
private:
    unsigned _m, _n; 
    T* data;         

public:
    
    MatrixDense(unsigned m, unsigned n);

    
    ~MatrixDense();

    
    T& operator()(unsigned i, unsigned j);
    const T& operator()(unsigned i, unsigned j) const;

    
    MatrixDense operator+(const MatrixDense& other) const;
    MatrixDense operator-(const MatrixDense& other) const;
    MatrixDense elementwiseMultiplication(const MatrixDense& other) const;
    MatrixDense transpose() const;

    
    void exportToFile(const std::string& filename) const override;

    
    void importFromFile(const std::string& filename) override;

    
    void print() const override;
};
