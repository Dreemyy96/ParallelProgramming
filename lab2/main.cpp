#include <iostream>
#include "MatrixDense.h"
#include "MatrixDiagonal.h"
#include "MatrixBlock.h"

int main() {
    try {
        // Пример работы с MatrixDense
        std::cout << "=== MatrixDense Example ===\n";
        MatrixDense<> denseMatrix(3, 3);
        denseMatrix(0, 0) = 1.0; denseMatrix(0, 1) = 2.0; denseMatrix(0, 2) = 3.0;
        denseMatrix(1, 0) = 4.0; denseMatrix(1, 1) = 5.0; denseMatrix(1, 2) = 6.0;
        denseMatrix(2, 0) = 7.0; denseMatrix(2, 1) = 8.0; denseMatrix(2, 2) = 9.0;
        denseMatrix.print();

        std::cout << "Transpose of denseMatrix:\n";
        auto transposedDense = denseMatrix.transpose();
        transposedDense.print();

        denseMatrix.exportToFile("denseMatrix.txt");
        std::cout << "denseMatrix exported to 'denseMatrix.txt'.\n";

        // Пример работы с MatrixDiagonal
        std::cout << "\n=== MatrixDiagonal Example ===\n";
        MatrixDiagonal<> diagMatrix(3);
        diagMatrix(0, 0) = 1.0; diagMatrix(1, 1) = 2.0; diagMatrix(2, 2) = 3.0;
        diagMatrix.print();

        std::cout << "Transpose of diagMatrix:\n";
        auto transposedDiag = diagMatrix.transpose();
        transposedDiag.print();

        diagMatrix.exportToFile("diagMatrix.txt");
        std::cout << "diagMatrix exported to 'diagMatrix.txt'.\n";

        // Пример работы с MatrixBlock
        std::cout << "\n=== MatrixBlock Example ===\n";
        MatrixBlock<> blockMatrix(2, 2, 2); // 2x2 матрица блоков, каждый блок размером 2x2

        // Заполнение блоков
        auto denseBlock1 = std::make_unique<MatrixDense<>>(2, 2);
        (*denseBlock1)(0, 0) = 1.0; (*denseBlock1)(0, 1) = 2.0;
        (*denseBlock1)(1, 0) = 3.0; (*denseBlock1)(1, 1) = 4.0;

        auto denseBlock2 = std::make_unique<MatrixDense<>>(2, 2);
        (*denseBlock2)(0, 0) = 5.0; (*denseBlock2)(0, 1) = 6.0;
        (*denseBlock2)(1, 0) = 7.0; (*denseBlock2)(1, 1) = 8.0;

        blockMatrix(0, 0) = denseBlock1.release();
        blockMatrix(0, 1) = denseBlock2.release();

        std::cout << "Block Matrix:\n";
        blockMatrix.print();

        auto transposedBlock = blockMatrix.transpose();
        std::cout << "Transpose of Block Matrix:\n";
        transposedBlock.print();

        blockMatrix.exportToFile("blockMatrix.txt");
        std::cout << "blockMatrix exported to 'blockMatrix.txt'.\n";

        std::cout << "\n=== Kronecker Product Example ===\n";
        auto kronMatrix = blockMatrix.kroneckerProduct(blockMatrix);
        kronMatrix.print();
        kronMatrix.exportToFile("kronMatrix.txt");
        std::cout << "Kronecker product matrix exported to 'kronMatrix.txt'.\n";

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
    }

    return 0;
}
