#include "MatrixDense.h"


template<typename T>
MatrixDense<T>::MatrixDense(unsigned m, unsigned n) : _m(m), _n(n) {
    data = new T[m * n]();
}

template<typename T>
MatrixDense<T>::~MatrixDense() {
    delete[] data;
}


template<typename T>
T& MatrixDense<T>::operator()(unsigned i, unsigned j) {
    return data[i * _n + j];
}

template<typename T>
const T& MatrixDense<T>::operator()(unsigned i, unsigned j) const {
    return data[i * _n + j];
}


template<typename T>
MatrixDense<T> MatrixDense<T>::operator+(const MatrixDense& other) const {
    if (_m != other._m || _n != other._n)
        throw std::invalid_argument("Matrix dimensions must match.");
    MatrixDense result(_m, _n);
    for (unsigned i = 0; i < _m * _n; ++i)
        result.data[i] = data[i] + other.data[i];
    return result;
}


template<typename T>
MatrixDense<T> MatrixDense<T>::operator-(const MatrixDense& other) const {
    if (_m != other._m || _n != other._n)
        throw std::invalid_argument("Matrix dimensions must match.");
    MatrixDense result(_m, _n);
    for (unsigned i = 0; i < _m * _n; ++i)
        result.data[i] = data[i] - other.data[i];
    return result;
}


template<typename T>
MatrixDense<T> MatrixDense<T>::elementwiseMultiplication(const MatrixDense& other) const {
    if (_m != other._m || _n != other._n)
        throw std::invalid_argument("Matrix dimensions must match.");
    MatrixDense result(_m, _n);
    for (unsigned i = 0; i < _m * _n; ++i)
        result.data[i] = data[i] * other.data[i];
    return result;
}


template<typename T>
MatrixDense<T> MatrixDense<T>::transpose() const {
    MatrixDense result(_n, _m);
    for (unsigned i = 0; i < _m; ++i)
        for (unsigned j = 0; j < _n; ++j)
            result(j, i) = (*this)(i, j);
    return result;
}


template<typename T>
void MatrixDense<T>::exportToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file)
        throw std::runtime_error("Cannot open file.");
    file << "MatrixDense\n";
    file << _m << " " << _n << "\n";
    for (unsigned i = 0; i < _m; ++i) {
        for (unsigned j = 0; j < _n; ++j)
            file << (*this)(i, j) << " ";
        file << "\n";
    }
}


template<typename T>
void MatrixDense<T>::importFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file)
        throw std::runtime_error("Cannot open file.");
    std::string header;
    file >> header;
    if (header != "MatrixDense")
        throw std::runtime_error("Invalid file format.");
    file >> _m >> _n;
    delete[] data;
    data = new T[_m * _n];
    for (unsigned i = 0; i < _m; ++i)
        for (unsigned j = 0; j < _n; ++j)
            file >> (*this)(i, j);
}


template<typename T>
void MatrixDense<T>::print() const {
    for (unsigned i = 0; i < _m; ++i) {
        for (unsigned j = 0; j < _n; ++j)
            std::cout << (*this)(i, j) << " ";
        std::cout << "\n";
    }
}


