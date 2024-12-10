#include "MatrixDiagonal.h"


template<typename T>
MatrixDiagonal<T>::MatrixDiagonal(unsigned size, const std::vector<int>& diagIndices)
    : _size(size), diagonals(diagIndices) {
    if (diagIndices.empty())
        throw std::invalid_argument("Diagonal indices must not be empty.");
    data.resize(size * diagIndices.size(), 0);
}


template<typename T>
bool MatrixDiagonal<T>::isValidIndex(unsigned i, unsigned j) const {
    int offset = static_cast<int>(j) - static_cast<int>(i);
    return std::find(diagonals.begin(), diagonals.end(), offset) != diagonals.end();
}


template<typename T>
T& MatrixDiagonal<T>::operator()(unsigned i, unsigned j) {
    if (!isValidIndex(i, j))
        throw std::out_of_range("Accessing non-diagonal element is not allowed.");
    int offset = static_cast<int>(j) - static_cast<int>(i);
    auto diagIndex = std::find(diagonals.begin(), diagonals.end(), offset) - diagonals.begin();
    return data[diagIndex * _size + i];
}

template<typename T>
const T& MatrixDiagonal<T>::operator()(unsigned i, unsigned j) const {
    if (!isValidIndex(i, j))
        throw std::out_of_range("Accessing non-diagonal element is not allowed.");
    int offset = static_cast<int>(j) - static_cast<int>(i);
    auto diagIndex = std::find(diagonals.begin(), diagonals.end(), offset) - diagonals.begin();
    return data[diagIndex * _size + i];
}


template<typename T>
MatrixDiagonal<T> MatrixDiagonal<T>::operator+(const MatrixDiagonal& other) const {
    if (_size != other._size || diagonals != other.diagonals)
        throw std::invalid_argument("Matrix dimensions or diagonals must match.");
    MatrixDiagonal result(_size, diagonals);
    for (size_t i = 0; i < data.size(); ++i)
        result.data[i] = data[i] + other.data[i];
    return result;
}


template<typename T>
MatrixDiagonal<T> MatrixDiagonal<T>::operator-(const MatrixDiagonal& other) const {
    if (_size != other._size || diagonals != other.diagonals)
        throw std::invalid_argument("Matrix dimensions or diagonals must match.");
    MatrixDiagonal result(_size, diagonals);
    for (size_t i = 0; i < data.size(); ++i)
        result.data[i] = data[i] - other.data[i];
    return result;
}


template<typename T>
MatrixDiagonal<T> MatrixDiagonal<T>::elementwiseMultiplication(const MatrixDiagonal& other) const {
    if (_size != other._size || diagonals != other.diagonals)
        throw std::invalid_argument("Matrix dimensions or diagonals must match.");
    MatrixDiagonal result(_size, diagonals);
    for (size_t i = 0; i < data.size(); ++i)
        result.data[i] = data[i] * other.data[i];
    return result;
}


template<typename T>
MatrixDiagonal<T> MatrixDiagonal<T>::transpose() const {
    std::vector<int> transposedDiagonals;
    for (int diag : diagonals)
        transposedDiagonals.push_back(-diag);
    MatrixDiagonal result(_size, transposedDiagonals);
    result.data = data;
    return result;
}


template<typename T>
void MatrixDiagonal<T>::exportToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file)
        throw std::runtime_error("Cannot open file.");
    file << "MatrixDiagonal\n";
    file << _size << " " << diagonals.size() << "\n";
    for (int diag : diagonals)
        file << diag << " ";
    file << "\n";
    for (const auto& value : data)
        file << value << " ";
    file << "\n";
}


template<typename T>
void MatrixDiagonal<T>::importFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file)
        throw std::runtime_error("Cannot open file.");
    std::string header;
    file >> header;
    if (header != "MatrixDiagonal")
        throw std::runtime_error("Invalid file format.");
    unsigned numDiagonals;
    file >> _size >> numDiagonals;
    diagonals.resize(numDiagonals);
    for (unsigned i = 0; i < numDiagonals; ++i)
        file >> diagonals[i];
    data.resize(_size * numDiagonals);
    for (unsigned i = 0; i < data.size(); ++i)
        file >> data[i];
}


template<typename T>
void MatrixDiagonal<T>::print() const {
    for (unsigned i = 0; i < _size; ++i) {
        for (unsigned j = 0; j < _size; ++j) {
            if (isValidIndex(i, j))
                std::cout << (*this)(i, j) << " ";
            else
                std::cout << "0 ";
        }
        std::cout << "\n";
    }
}
