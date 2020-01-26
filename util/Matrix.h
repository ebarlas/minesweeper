#ifndef MINESWEEPER_MATRIX_H
#define MINESWEEPER_MATRIX_H

#include <set>
#include <functional>

namespace minesweeper {
    template<typename T>
    class Matrix {
    public:
        Matrix(int rows, int columns);
        T &at(int row, int col);
        void forEach(std::function<void(int row, int col, T &val)> fn);
    private:
        int rows;
        int columns;
        std::vector<T> matrix;
    };

    template<typename T>
    Matrix<T>::Matrix(int rows, int columns) : rows(rows), columns(columns), matrix(rows * columns) {

    }

    template<typename T>
    T &Matrix<T>::at(int row, int col) {
        int n = row * columns + col;
        return matrix[n];
    }

    template<typename T>
    void Matrix<T>::forEach(std::function<void(int row, int col, T &val)> fn) {
        for (int i = 0; i < matrix.size(); i++) {
            int row = i / columns;
            int col = i % columns;
            fn(row, col, matrix[i]);
        }
    }
}

#endif
