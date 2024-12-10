#pragma once

#include "IMatrix.h"
#include <vector>
#include <memory>
#include <stdexcept>
#include <fstream>
#include <iostream>

// Класс блочной матрицы
template<typename T = double>
class MatrixBlock : public IMatrix {
private:
    unsigned _rows;                       
    unsigned _cols;                       
    unsigned _blockSize;                 
    std::vector<std::unique_ptr<IMatrix>> blocks; 

public:
 
    MatrixBlock(unsigned rows, unsigned cols, unsigned blockSize);


    ~MatrixBlock() = default;


    T operator()(unsigned i, unsigned j) const;


    MatrixBlock operator+(const MatrixBlock& other) const;
    MatrixBlock operator-(const MatrixBlock& other) const;
    MatrixBlock elementwiseMultiplication(const MatrixBlock& other) const;
    MatrixBlock transpose() const;


    MatrixBlock kroneckerProduct(const MatrixBlock& other) const;


    void exportToFile(const std::string& filename) const override;


    void importFromFile(const std::string& filename) override;


    void print() const override;

private:

    IMatrix* getBlock(unsigned i, unsigned j) const;


    void validateDimensions(const MatrixBlock& other) const;
};
