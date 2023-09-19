#pragma once
#include <complex>
#include <vector>
#include <exception>
#include <iostream>

namespace Work_namespace {

    /// <summary>
    /// General template class of matrix that
    /// provides some stuff to work with typed matrixes
    /// </summary>
    /// <typeparam name="T">General parameter</typeparam>
    template<class T>
    class Matrix {
    private:
        std::vector<std::vector<T>> matr;
        long columns, rows;

    public:
        Matrix() : columns(0), rows(0) {}
        Matrix(const Matrix& a);
        Matrix& operator=(const Matrix& a);
        void kill();
        void SetDims(long n, long m);
        void SetVals(const std::vector<std::vector<T>>& value);
        long NumRows() const { return rows; }
        long NumCols() const { return columns; }
        std::vector<T>& operator[](long i) { return matr[i]; }
        const std::vector<T>& operator[](long i) const { return matr[i]; }
        /*friend function to make it easy to output an obect of Matrix class*/
        friend std::ostream& operator<<(std::ostream& out, const Matrix& m) {
            for (long i = 0; i < m.NumRows(); ++i) {
                for (long j = 0; j < m.NumCols(); ++j) {
                    out << m[i][j] << " ";
                }
                out << std::endl;
            }
            return out;
        }
    };

    template<class T>
    Matrix<T>::Matrix(const Matrix& src) :
        matr(src.matr), columns(src.columns), rows(src.rows) {}

    template<class T>
    Matrix<T>& Matrix<T>::operator=(const Matrix& src)
    {
        if (this == &src) return *this;

        if (src.NumCols() == 0 || src.NumRows() == 0)
            SetDims(src.NumRows(), src.NumCols());
        else if (columns != src.NumCols() || rows != src.NumCols()) {
            for (auto& row : matr) {
                row.clear();
            }
            matr.clear();
            matr.resize(src.NumRows(), std::vector<T>(src.NumCols(), 0));
            for (long i = 0; i < rows; ++i) {
                matr[i] = src[i];
            }
        }
        else if (columns == src.NumCols() && rows == src.NumCols()) {
            for (long i = 0; i < rows; ++i) {
                matr[i] = src[i];
            }
        }
        else throw std::exception("Unhandled behaviour of matrix assignment.");

        return *this;
    }
    template<class T>
    void Matrix<T>::kill()
    {
        columns = 0; rows = 0;
        for (auto& row : matr) {
            row.clear();
        }
        matr.clear();
    }


    template<class T>
    void Matrix<T>::SetDims(long n, long m)
    {
        if (n < 0 || m < 0)
            throw std::invalid_argument("Bad args in SetDims method of Matrix");
        columns = n; rows = m;
    }
    template <class T>
    void Matrix<T>::SetVals(const std::vector<std::vector<T>>& value) {
        if (value.size() == 0) {
            rows = 0; columns = 0;
            return;
        }
        else {
            rows = value.size();
            columns = value[0].size();
            matr = value;
        }
    }
}

//namespace QLab {
//
//    using ::Work_namespace::Matrix;
//
//}

