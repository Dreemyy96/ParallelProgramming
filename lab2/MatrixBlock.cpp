#include "MatrixBlock.h"


template<typename T>
MatrixBlock<T>::MatrixBlock(unsigned rows, unsigned cols, unsigned blockSize)
    : _rows(rows), _cols(cols), _blockSize(blockSize) {
    blocks.resize(rows * cols, nullptr);
}


template<typename T>
T MatrixBlock<T>::operator()(unsigned i, unsigned j) const {
    unsigned blockRow = i / _blockSize;
    unsigned blockCol = j / _blockSize;
    unsigned innerRow = i % _blockSize;
    unsigned innerCol = j % _blockSize;

    IMatrix* block = getBlock(blockRow, blockCol);
    if (!block)
        return 0; // нулевой блок
    return (*block)(innerRow, innerCol);
}


template<typename T>
IMatrix* MatrixBlock<T>::getBlock(unsigned i, unsigned j) const {
    if (i >= _rows || j >= _cols)
        throw std::out_of_range("Block index out of range.");
    return blocks[i * _cols + j].get();
}


template<typename T>
void MatrixBlock<T>::validateDimensions(const MatrixBlock& other) const {
    if (_rows != other._rows || _cols != other._cols || _blockSize != other._blockSize)
        throw std::invalid_argument("Matrix dimensions or block size do not match.");
}


template<typename T>
MatrixBlock<T> MatrixBlock<T>::operator+(const MatrixBlock& other) const {
    validateDimensions(other);
    MatrixBlock result(_rows, _cols, _blockSize);
    for (unsigned i = 0; i < blocks.size(); ++i) {
        if (blocks[i] && other.blocks[i])
            result.blocks[i] = std::make_unique<IMatrix>(*blocks[i] + *other.blocks[i]);
        else if (blocks[i])
            result.blocks[i] = std::make_unique<IMatrix>(*blocks[i]);
        else if (other.blocks[i])
            result.blocks[i] = std::make_unique<IMatrix>(*other.blocks[i]);
    }
    return result;
}


template<typename T>
MatrixBlock<T> MatrixBlock<T>::operator-(const MatrixBlock& other) const {
    validateDimensions(other);
    MatrixBlock result(_rows, _cols, _blockSize);
    for (unsigned i = 0; i < blocks.size(); ++i) {
        if (blocks[i] && other.blocks[i])
            result.blocks[i] = std::make_unique<IMatrix>(*blocks[i] - *other.blocks[i]);
        else if (blocks[i])
            result.blocks[i] = std::make_unique<IMatrix>(*blocks[i]);
        else if (other.blocks[i])
            result.blocks[i] = std::make_unique<IMatrix>(*other.blocks[i] * -1);
    }
    return result;
}


template<typename T>
MatrixBlock<T> MatrixBlock<T>::elementwiseMultiplication(const MatrixBlock& other) const {
    validateDimensions(other);
    MatrixBlock result(_rows, _cols, _blockSize);
    for (unsigned i = 0; i < blocks.size(); ++i) {
        if (blocks[i] && other.blocks[i])
            result.blocks[i] = std::make_unique<IMatrix>(blocks[i]->elementwiseMultiplication(*other.blocks[i]));
    }
    return result;
}


template<typename T>
MatrixBlock<T> MatrixBlock<T>::transpose() const {
    MatrixBlock result(_cols, _rows, _blockSize);
    for (unsigned i = 0; i < _rows; ++i) {
        for (unsigned j = 0; j < _cols; ++j) {
            result.blocks[j * _rows + i] = blocks[i * _cols + j]; // перестановка блоков
        }
    }
    return result;
}


template<typename T>
MatrixBlock<T> MatrixBlock<T>::kroneckerProduct(const MatrixBlock& other) const {
    unsigned newRows = _rows * other._rows;
    unsigned newCols = _cols * other._cols;
    unsigned newBlockSize = _blockSize * other._blockSize;

    MatrixBlock result(newRows, newCols, newBlockSize);
    for (unsigned i = 0; i < _rows; ++i) {
        for (unsigned j = 0; j < _cols; ++j) {
            IMatrix* blockA = getBlock(i, j);
            if (!blockA)
                continue;

            for (unsigned k = 0; k < other._rows; ++k) {
                for (unsigned l = 0; l < other._cols; ++l) {
                    IMatrix* blockB = other.getBlock(k, l);
                    if (!blockB)
                        continue;

                    unsigned newBlockIndex = (i * other._rows + k) * newCols + (j * other._cols + l);
                    result.blocks[newBlockIndex] = std::make_unique<IMatrix>(blockA->elementwiseMultiplication(*blockB));
                }
            }
        }
    }
    return result;
}


template<typename T>
void MatrixBlock<T>::exportToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file)
        throw std::runtime_error("Cannot open file.");
    file << "MatrixBlock\n";
    file << _rows << " " << _cols << " " << _blockSize << "\n";
    for (const auto& block : blocks) {
        if (block)
            block->exportToFile(file);
        else
            file << "NULL\n";
    }
}


template<typename T>
void MatrixBlock<T>::importFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file)
        throw std::runtime_error("Cannot open file.");
    std::string header;
    file >> header;
    if (header != "MatrixBlock")
        throw std::runtime_error("Invalid file format.");
    file >> _rows >> _cols >> _blockSize;
    blocks.resize(_rows * _cols);
    for (auto& block : blocks) {
        std::string blockHeader;
        file >> blockHeader;
        if (blockHeader == "NULL")
            block.reset();
        else {
            block = std::make_unique<IMatrix>();
            block->importFromFile(file);
        }
    }
}


template<typename T>
void MatrixBlock<T>::print() const {
    for (unsigned i = 0; i < _rows * _blockSize; ++i) {
        for (unsigned j = 0; j < _cols * _blockSize; ++j) {
            std::cout << (*this)(i, j) << " ";
        }
        std::cout << "\n";
    }
}
