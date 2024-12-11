#include "Matrix.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <iomanip>



template <typename T = double>
class MatrixDiagonal : public Matrix<T> {
private:
    unsigned _size; // Размер матрицы (количество строк/столбцов)
    T* data;      // Массив для хранения диагональных элементов

public:
    MatrixDiagonal(unsigned size) : _size(size) {
        if (size == 0) {
            throw std::invalid_argument("Matrix size must be positive.");
        }
        data = new T[_size];
        for(size_t i = 0; i < _size; ++i)
            data[i] = 0;
    }

        MatrixDiagonal(const MatrixDiagonal& other) : _size(other._size) {
      data = new T[_size];
      for(size_t i = 0; i < _size; ++i){
        data[i] = other.data[i];
      }
    }

    MatrixDiagonal& operator=(const MatrixDiagonal& other) {
      if (this == &other) return *this;
      delete[] data;
      _size = other._size;
      data = new T[_size];
       for(size_t i = 0; i < _size; ++i){
        data[i] = other.data[i];
      }
      return *this;
    }

    ~MatrixDiagonal() override {
        delete[] data;
    }

    T& operator()(unsigned i, unsigned j) override {
        if (i >= _size || j >= _size) {
            throw std::out_of_range("Matrix index out of bounds.");
        }
        if (i == j) {
            return data[i];
        } else {
            static T zero = 0;
            return zero; // Возвращаем ссылку на статическую переменную 0
        }
    }

    const T& operator()(unsigned i, unsigned j) const override {
        if (i >= _size || j >= _size) {
            throw std::out_of_range("Matrix index out of bounds.");
        }
        if (i == j) {
            return data[i];
        } else {
            static T zero = 0;
            return zero;
        }
    }

    Matrix<T>* operator+(const Matrix<T>& other) const override{
        const MatrixDiagonal<T>* otherDiagonal = dynamic_cast<const MatrixDiagonal<T>*>(&other);
        if (!otherDiagonal) {
           throw std::invalid_argument("Incompatible matrix types for addition.");
        }

        if(_size != otherDiagonal->_size) {
            throw std::invalid_argument("Matrix dimensions must be equal for addition.");
        }

        MatrixDiagonal<T>* result = new MatrixDiagonal<T>(_size);
        for(size_t i = 0; i < _size; ++i) {
            (*result)(i, i) = (*this)(i, i) + (*otherDiagonal)(i, i);
        }

        return result;
    }

    Matrix<T>* operator-(const Matrix<T>& other) const {
        const MatrixDiagonal<T>* otherDiagonal = dynamic_cast<const MatrixDiagonal<T>*>(&other);
        if (!otherDiagonal) {
            throw std::invalid_argument("Incompatible matrix types for substraction.");
        }

        if(_size != otherDiagonal->_size) {
            throw std::invalid_argument("Matrix dimensions must be equal for substraction.");
        }

        MatrixDiagonal<T>* result = new MatrixDiagonal<T>(_size);
        for(size_t i = 0; i < _size; ++i) {
            (*result)(i, i) = (*this)(i, i) - (*otherDiagonal)(i, i);
        }

        return result;
    }

    
    Matrix<T>* operator*(const Matrix<T>& other) const override{
        const MatrixDiagonal<T>* otherDiagonal = dynamic_cast<const MatrixDiagonal<T>*>(&other);
        if (!otherDiagonal) {
            throw std::invalid_argument("Incompatible matrix types for multiplication.");
        }

        if(_size != otherDiagonal->_size) {
            throw std::invalid_argument("Incompatible matrix dimensions for multiplication.");
        }

        MatrixDiagonal<T>* result = new MatrixDiagonal<T>(_size);
        for(size_t i = 0; i < _size; ++i) {
            (*result)(i, i) = (*this)(i, i) * (*otherDiagonal)(i, i);
        }

        return result;
    }
    
    Matrix<T>* elementWiseMultiplication(const Matrix<T>& other) const override{
        return this->operator*(other);
    }


    Matrix<T>* transpose() const override{
        return new MatrixDiagonal<T>(*this);
    }

    // Операции и другие методы (см. ниже)
    void importFromFile(const std::string& filename) override{
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filename);
        }

        std::string matrixType;
        file >> matrixType;
        if(matrixType != "MatrixDiagonal"){
            throw std::runtime_error("Invalid matrix type in file: " + filename);
        }

        unsigned size;
        file >> size;
        if (file.fail() || size == 0) {
            throw std::runtime_error("Invalid matrix dimensions in file: " + filename);
        }
        delete[] data;
        _size = size;
        data = new T[_size];

        for (unsigned i = 0; i < _size; ++i) {
            if (!(file >> data[i])) {
                throw std::runtime_error("Error reading matrix data from file: " + filename);
            }
        }

        file.close();
    }
    void exportToFile(const std::string& filename) const override{
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filename);
        }

        file << "MatrixDiagonal\n";
        file << _size << "\n";

        for (unsigned i = 0; i < _size; ++i) {
            file << data[i] << " ";
        }
        file << "\n";

        file.close();
    }
    void print() const override{
        for (unsigned i = 0; i < _size; ++i) {
            for (unsigned j = 0; j < _size; ++j) {
                if (i == j) {
                    std::cout << std::setw(10) << data[i] << " ";
                } else {
                    std::cout << std::setw(10) << 0 << " ";
                }
            }
        std::cout << std::endl;
        }
    }
};