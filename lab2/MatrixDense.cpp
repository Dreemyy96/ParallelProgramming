#include "Matrix.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <iomanip>

template <typename T = double>
class MatrixDense : public Matrix<T> {
private:
    unsigned _m, _n;
    T* data;

public:
    MatrixDense(unsigned m, unsigned n) : _m(m), _n(n) {
        if (m == 0 || n == 0) {
            throw std::invalid_argument("Matrix dimensions must be positive.");
        }
        data = new T[_m * _n];
        for (size_t i = 0; i < _m * _n; ++i)
            data[i] = 0; //Инициализация нулями
    }

    MatrixDense(const MatrixDense& other) : _m(other._m), _n(other._n) {
      data = new T[_m * _n];
      for(size_t i = 0; i < _m*_n; ++i){
        data[i] = other.data[i];
      }
    }

    Matrix<T>* operator+(const Matrix<T>& other) const override {
        const MatrixDense<T>* otherDense = dynamic_cast<const MatrixDense<T>*>(&other);
        if (!otherDense) {
           throw std::invalid_argument("Incompatible matrix types for addition.");
        }

        if(_m != otherDense->_m || _n != otherDense->_n) {
            throw std::invalid_argument("Matrix dimensions must be equal for addition.");
        }

        MatrixDense<T>* result = new MatrixDense<T>(_m, _n);
        for(size_t i = 0; i < _m; ++i) {
            for(size_t j = 0; j < _n; ++j){
                (*result)(i, j) = (*this)(i, j) + (*otherDense)(i, j);
            }
        }

        return result;
    }

    
    Matrix<T>* operator-(const Matrix<T>& other) const override{
        const MatrixDense<T>* otherDense = dynamic_cast<const MatrixDense<T>*>(&other);
        if (!otherDense) {
           throw std::invalid_argument("Incompatible matrix types for substraction.");
        }

        if(_m != otherDense->_m || _n != otherDense->_n) {
            throw std::invalid_argument("Matrix dimensions must be equal for substraction.");
        }

        MatrixDense<T>* result = new MatrixDense<T>(_m, _n);
        for(size_t i = 0; i < _m; ++i) {
            for(size_t j = 0; j < _n; ++j){
                (*result)(i, j) = (*this)(i, j) - (*otherDense)(i, j);
            }
        }

        return result;
    }

    MatrixDense& operator=(const MatrixDense& other) {
      if (this == &other) return *this;
      delete[] data;
      _m = other._m;
      _n = other._n;
      data = new T[_m*_n];
       for(size_t i = 0; i < _m*_n; ++i){
        data[i] = other.data[i];
      }
      return *this;
    }

    
    Matrix<T>* operator*(const Matrix<T>& other) const override{
        const MatrixDense<T>* otherDense = dynamic_cast<const MatrixDense<T>*>(&other);
        if (!otherDense) {
           throw std::invalid_argument("Incompatible matrix types for multiplication.");
        }

        if(_n != otherDense->_m) {
            throw std::invalid_argument("Incompatible matrix dimensions for multiplication.");
        }

        MatrixDense<T>* result = new MatrixDense<T>(_m, otherDense->_n);
        for(size_t i = 0; i < _m; ++i) {
            for(size_t j = 0; j < otherDense->_n; ++j){
                T sum = 0;
                for(size_t k = 0; k < _n; ++k){
                    sum += (*this)(i, k) * (*otherDense)(k, j);
                }
            (*result)(i, j) = sum;
            }   
        }

        return result;
    }


    Matrix<T>* elementWiseMultiplication(const Matrix<T>& other) const override{
        const MatrixDense<T>* otherDense = dynamic_cast<const MatrixDense<T>*>(&other);
        if (!otherDense) {
           throw std::invalid_argument("Incompatible matrix types for elementWiseMultiplication.");
        }

        if(_m != otherDense->_m || _n != otherDense->_n) {
            throw std::invalid_argument("Matrix dimensions must be equal for elementWiseMultiplication.");
        }

        MatrixDense<T>* result = new MatrixDense<T>(_m, _n);
        for(size_t i = 0; i < _m; ++i) {
            for(size_t j = 0; j < _n; ++j){
                (*result)(i, j) = (*this)(i, j) * (*otherDense)(i, j);
            }
        }

        return result;
    }

    
    Matrix<T>* transpose() const override{
        MatrixDense<T>* result = new MatrixDense<T>(_n, _m);
        for(size_t i = 0; i < _m; ++i){
            for(size_t j = 0; j < _n; ++j){
                (*result)(j, i) = (*this)(i, j);
            }
        }
    return result;
    }

    ~MatrixDense() override {
        delete[] data;
    }

    T& operator()(unsigned i, unsigned j) override {
        if (i >= _m || j >= _n) {
            throw std::out_of_range("Matrix index out of bounds.");
        }
        return data[i * _n + j];
    }

    const T& operator()(unsigned i, unsigned j) const override {
        if (i >= _m || j >= _n) {
            throw std::out_of_range("Matrix index out of bounds.");
        }
        return data[i * _n + j];
    }
    
    unsigned getRows() const {
        return _m;
    }

    unsigned getCols() const {
        return _n;
    }

//Операции и другие методы (см. ниже)...
    void importFromFile(const std::string& filename) override{
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filename);
        }

        std::string matrixType;
        file >> matrixType;
        if(matrixType != "MatrixDense"){
            throw std::runtime_error("Invalid matrix type in file: " + filename);
        }

        unsigned m, n;
        file >> m >> n;
        if (file.fail() || m == 0 || n == 0) {
            throw std::runtime_error("Invalid matrix dimensions in file: " + filename);
        }
        delete[] data;
        _m = m;
        _n = n;
        data = new T[_m * _n];

        for (unsigned i = 0; i < _m; ++i) {
            for (unsigned j = 0; j < _n; ++j) {
                if (!(file >> data[i * _n + j])) {
                    throw std::runtime_error("Error reading matrix data from file: " + filename);
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

        file << "MatrixDense\n";
        file << _m << " " << _n << "\n";

        for (unsigned i = 0; i < _m; ++i) {
            for (unsigned j = 0; j < _n; ++j) {
                file << (*this)(i, j) << " ";
            }
            file << "\n";
        }

        file.close();
    }

    void print() const override{
        for (unsigned i = 0; i < _m; ++i) {
            for (unsigned j = 0; j < _n; ++j) {
                std::cout << std::setw(10) << (*this)(i, j) << " "; // Используем setw для выравнивания
            }
        std::cout << std::endl;
        }   
    }
};


