#include "Matrix.h"
#include "MatrixDense.cpp"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <iomanip>
#include <map>


template <typename T = double>
class MatrixBlock : public Matrix<T> {
private:
    unsigned _blockRows;    // Количество блоков по строкам
    unsigned _blockCols;    // Количество блоков по столбцам
    unsigned _blockSize;   // Размер блока (предполагаем квадратные блоки)
    std::vector<MatrixDense<T>*> _blocks;

public:
    MatrixBlock(unsigned blockRows, unsigned blockCols, unsigned blockSize)
        : _blockRows(blockRows), _blockCols(blockCols), _blockSize(blockSize) {
            _blocks.resize(blockRows*blockCols, nullptr);
        }
    MatrixBlock(const MatrixBlock& other) : _blockRows(other._blockRows), _blockCols(other._blockCols), _blockSize(other._blockSize){
        _blocks.resize(_blockRows*_blockCols, nullptr);
        std::map<MatrixDense<T>*, MatrixDense<T>*> block_map;
        for(size_t i = 0; i < _blockRows * _blockCols; ++i){
            if(other._blocks[i]!= nullptr)
            {
                if(block_map.find(other._blocks[i]) == block_map.end()){
                    _blocks[i] = new MatrixDense<T>(*other._blocks[i]);
                    block_map[other._blocks[i]] = _blocks[i];
                }else{
                    _blocks[i] = block_map[other._blocks[i]];
                }
            }
        }
    }
    MatrixBlock& operator=(const MatrixBlock& other){
        if(this == &other) return *this;
        for(size_t i = 0; i < _blockRows*_blockCols; ++i){
            if(_blocks[i] != nullptr){
                delete _blocks[i];
            }
        }
        _blocks.clear();
        _blockRows = other._blockRows;
        _blockCols = other._blockCols;
        _blockSize = other._blockSize;
        _blocks.resize(_blockRows*_blockCols);
        std::map<MatrixDense<T>*, MatrixDense<T>*> block_map;
        for(size_t i = 0; i < _blockRows * _blockCols; ++i){
            if(other._blocks[i]!= nullptr)
            {
                if(block_map.find(other._blocks[i]) == block_map.end()){
                    _blocks[i] = new MatrixDense<T>(*other._blocks[i]);
                    block_map[other._blocks[i]] = _blocks[i];
                }else{
                    _blocks[i] = block_map[other._blocks[i]];
                }
            }
        }

        return *this;
    }

    ~MatrixBlock() override {
        for (auto block : _blocks) {
            if (block != nullptr) {
                delete block;
            }
        }
    }

    T& operator()(unsigned i, unsigned j) override {
        unsigned blockRow = i / _blockSize;
        unsigned blockCol = j / _blockSize;
        unsigned inBlockRow = i % _blockSize;
        unsigned inBlockCol = j % _blockSize;

        if (blockRow >= _blockRows || blockCol >= _blockCols || i >= _blockRows * _blockSize || j >= _blockCols * _blockSize) {
            throw std::out_of_range("Matrix index out of bounds.");
        }
        if (_blocks[blockRow * _blockCols + blockCol] == nullptr){
            static T zero = 0;
            return zero;
        }
        return (*_blocks[blockRow * _blockCols + blockCol])(inBlockRow, inBlockCol);
    }

    const T& operator()(unsigned i, unsigned j) const override {
       unsigned blockRow = i / _blockSize;
        unsigned blockCol = j / _blockSize;
        unsigned inBlockRow = i % _blockSize;
        unsigned inBlockCol = j % _blockSize;

        if (blockRow >= _blockRows || blockCol >= _blockCols || i >= _blockRows * _blockSize || j >= _blockCols * _blockSize) {
            throw std::out_of_range("Matrix index out of bounds.");
        }

        if (_blocks[blockRow * _blockCols + blockCol] == nullptr){
            static T zero = 0;
            return zero;
        }
        return (*_blocks[blockRow * _blockCols + blockCol])(inBlockRow, inBlockCol);
    }

    void setBlock(unsigned blockRow, unsigned blockCol, MatrixDense<T>* block) {
        if (blockRow >= _blockRows || blockCol >= _blockCols || block->getRows() != _blockSize || block->getCols() != _blockSize) {
            throw std::invalid_argument("Invalid block dimensions or position.");
        }
        if(_blocks[blockRow*_blockCols + blockCol]!= nullptr){
            delete _blocks[blockRow*_blockCols + blockCol];
        }
        _blocks[blockRow * _blockCols + blockCol] = block;
    }
    unsigned getRows() const { return _blockRows * _blockSize; }
    unsigned getCols() const { return _blockCols * _blockSize; }

    Matrix<T>* operator+(const Matrix<T>& other) const override {
        const MatrixBlock<T>* otherBlock = dynamic_cast<const MatrixBlock<T>*>(&other);
        if (!otherBlock) {
            throw std::invalid_argument("Incompatible matrix types for addition.");
        }

        if (_blockRows != otherBlock->_blockRows || _blockCols != otherBlock->_blockCols || _blockSize != otherBlock->_blockSize) {
            throw std::invalid_argument("Matrix block dimensions must be equal for addition.");
        }

        MatrixBlock<T>* result = new MatrixBlock<T>(_blockRows, _blockCols, _blockSize);

        for (size_t i = 0; i < _blockRows; ++i) {
            for (size_t j = 0; j < _blockCols; ++j) {
                if (_blocks[i * _blockCols + j] != nullptr && otherBlock->_blocks[i * _blockCols + j] != nullptr) {
                    result->setBlock(i, j, *(_blocks[i * _blockCols + j]->operator+(*(otherBlock->_blocks[i * _blockCols + j])))); 
                } 
            }
        }

        return result;
    }

    Matrix<T>* operator-(const Matrix<T>& other) const override {
        const MatrixBlock<T>* otherBlock = dynamic_cast<const MatrixBlock<T>*>(&other);
        if (!otherBlock) {
            throw std::invalid_argument("Incompatible matrix types for subtraction.");
        }

        if (_blockRows != otherBlock->_blockRows || _blockCols != otherBlock->_blockCols || _blockSize != otherBlock->_blockSize) {
            throw std::invalid_argument("Matrix block dimensions must be equal for subtraction.");
        }

        MatrixBlock<T>* result = new MatrixBlock<T>(_blockRows, _blockCols, _blockSize);

        for (size_t i = 0; i < _blockRows; ++i) {
            for (size_t j = 0; j < _blockCols; ++j) {
                if (_blocks[i * _blockCols + j] != nullptr && otherBlock->_blocks[i * _blockCols + j] != nullptr) {
                    result->setBlock(i, j, *(_blocks[i * _blockCols + j]->operator-(*(otherBlock->_blocks[i * _blockCols + j])))); 
                } 
            }
        }

        return result;
    }

    Matrix<T>* operator*(const Matrix<T>& other) const override {
        const MatrixBlock<T>* otherBlock = dynamic_cast<const MatrixBlock<T>*>(&other);
        if (!otherBlock) {
            throw std::invalid_argument("Incompatible matrix types for multiplication.");
        }

        if (_blockCols != otherBlock->_blockRows) {
            throw std::invalid_argument("Incompatible matrix dimensions for multiplication.");
        }

        MatrixBlock<T>* result = new MatrixBlock<T>(_blockRows, otherBlock->_blockCols, _blockSize);

        for (size_t i = 0; i < _blockRows; ++i) {
            for (size_t j = 0; j < otherBlock->_blockCols; ++j) {
                MatrixDense<T>* blockSum = new MatrixDense<T>(_blockSize, _blockSize);
                for (size_t k = 0; k < _blockCols; ++k) {
                    if (_blocks[i * _blockCols + k] != nullptr && otherBlock->_blocks[k * otherBlock->_blockCols + j] != nullptr) {
                        *blockSum = *blockSum + *(_blocks[i * _blockCols + k]->operator*(*(otherBlock->_blocks[k * otherBlock->_blockCols + j]))); 
                    }
                }
                result->setBlock(i, j, blockSum);
            }
        }

        return result;
    }

    Matrix<T>* elementWiseMultiplication(const Matrix<T>& other) const override {
        const MatrixBlock<T>* otherBlock = dynamic_cast<const MatrixBlock<T>*>(&other);
        if (!otherBlock) {
            throw std::invalid_argument("Incompatible matrix types for element-wise multiplication.");
        }

        if (_blockRows != otherBlock->_blockRows || _blockCols != otherBlock->_blockCols || _blockSize != otherBlock->_blockSize) {
            throw std::invalid_argument("Matrix block dimensions must be equal for element-wise multiplication.");
        }

        MatrixBlock<T>* result = new MatrixBlock<T>(_blockRows, _blockCols, _blockSize);

        for (size_t i = 0; i < _blockRows; ++i) {
            for (size_t j = 0; j < _blockCols; ++j) {
                if (_blocks[i * _blockCols + j] != nullptr && otherBlock->_blocks[i * _blockCols + j] != nullptr) {
                    result->setBlock(i, j, *(_blocks[i * _blockCols + j]->elementWiseMultiplication(*(otherBlock->_blocks[i * _blockCols + j])))); 
                }
            }
        }

        return result;
    }

    Matrix<T>* transpose() const override {
        MatrixBlock<T>* result = new MatrixBlock<T>(_blockCols, _blockRows, _blockSize);

        for (size_t i = 0; i < _blockRows; ++i) {
            for (size_t j = 0; j < _blockCols; ++j) {
                if (_blocks[i * _blockCols + j] != nullptr) {
                    result->setBlock(j, i, _blocks[i * _blockCols + j]->transpose()); 
                }
            }
        }

        return result;
    }

    // Остальные методы: операции, importFromFile, exportToFile, print (см. ниже)
    void importFromFile(const std::string& filename) override{
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filename);
        }

        std::string matrixType;
        file >> matrixType;
        if(matrixType != "MatrixBlock"){
            throw std::runtime_error("Invalid matrix type in file: " + filename);
        }

        unsigned blockRows, blockCols, blockSize;
        file >> blockRows >> blockCols >> blockSize;
        if (file.fail() || blockRows == 0 || blockCols == 0 || blockSize == 0) {
            throw std::runtime_error("Invalid matrix dimensions in file: " + filename);
        }
        for(size_t i = 0; i < _blockRows*_blockCols; ++i){
            if(_blocks[i]!=nullptr) delete _blocks[i];
        }
        _blocks.clear();
        _blockRows = blockRows;
        _blockCols = blockCols;
        _blockSize = blockSize;
        _blocks.resize(_blockRows * _blockCols);
        for(size_t i = 0; i < _blockRows; ++i){
            for(size_t j = 0; j < _blockCols; ++j){
                std::string blockType;
                file >> blockType;
                if(blockType == "nullptr"){
                    _blocks[i*_blockCols+j] = nullptr;
                }else if(blockType == "MatrixDense"){
                    _blocks[i*_blockCols + j] = new MatrixDense<T>(blockSize, blockSize);
                    for(size_t k = 0; k < blockSize; ++k){
                        for(size_t l = 0; l < blockSize; ++l){
                            file >> (*_blocks[i*_blockCols+j])(k, l);
                        }
                    }
                }else{
                    throw std::runtime_error("Invalid block type in file");
                }
            }
        }
        file.close();
    }
    void exportToFile(const std::string& filename) const override{
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filename);
        }
        file << "MatrixBlock\n";
        file << _blockRows << " " << _blockCols << " " << _blockSize << "\n";
        for(size_t i = 0; i < _blockRows; ++i){
            for(size_t j = 0; j < _blockCols; ++j){
                if(_blocks[i*_blockCols+j] == nullptr){
                    file << "nullptr\n";
                }else{
                    file << "MatrixDense\n";
                    for(size_t k = 0; k < _blockSize; ++k){
                        for(size_t l = 0; l < _blockSize; ++l){
                            file << (*_blocks[i*_blockCols+j])(k, l) << " ";
                        }
                    file << "\n";
                    }
                }
            }
        }
        file.close();
    }
    void print() const override{
        for (unsigned i = 0; i < getRows(); ++i) {
            for (unsigned j = 0; j < getCols(); ++j) {
                std::cout << std::setw(10) << (*this)(i, j) << " ";
            }
        std::cout << std::endl;
        }
    }
};