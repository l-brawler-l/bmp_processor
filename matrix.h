#ifndef MATRIX1_MATRIX_H
#define MATRIX1_MATRIX_H

#include <cstddef>
#include <stdexcept>
#include <utility>

template <typename T>
class Matrix {
public:
    class Row {
        friend class Matrix;

    public:
        size_t GetNum() const {
            return num_;
        }

        size_t Size() const {
            return parent_->GetColsNum();
        }

        T& operator[](size_t j) {
            return parent_->At(num_, j);
        }

        const T& operator[](size_t j) const {
            return parent_->At(num_, j);
        }

    private:
        Row(size_t num, Matrix* parent) : parent_(parent), num_(num) {
        }

    private:
        Matrix* parent_;
        size_t num_;
    };

    class ConstRow {
        friend class Matrix;

    public:
        size_t GetNum() const {
            return num_;
        }

        size_t Size() const {
            return parent_->GetColsNum();
        }

        const T& operator[](size_t j) {
            return parent_->At(num_, j);
        }

        const T& operator[](size_t j) const {
            return parent_->At(num_, j);
        }

    private:
        ConstRow(size_t num, const Matrix* parent) : parent_(parent), num_(num) {
        }

    private:
        const Matrix* parent_;
        size_t num_;
    };

public:
    Matrix() : ptr_(nullptr), rows_num_(0), cols_num_(0) {
    }

    Matrix(size_t rows, size_t colms, T def_elem) : rows_num_(rows), cols_num_(colms) {
        if ((rows == 0 && colms != 0) || (rows != 0 && colms == 0)) {
            throw std::invalid_argument("You are trying to create matrix with 0 rows or 0 colums.");
        }
        AllocateMatrixStorage(rows, colms);
        for (size_t i = 0; i < rows * colms; ++i) {
            ptr_[i] = def_elem;
        }
    }

    Matrix(const Matrix& other)
        : ptr_(other.rows_num_ == 0 ? nullptr : AllocateMatrixStorage(other.rows_num_, other.cols_num_)),
          rows_num_(other.rows_num_),
          cols_num_(other.cols_num_) {
        if (!ptr_) {
            return;
        }
        for (size_t i = 0; i < rows_num_ * cols_num_; ++i) {
            ptr_[i] = other.ptr_[i];
        }
    }

    ~Matrix() {
        if (ptr_) {
            delete[] ptr_;
            ptr_ = nullptr;
        }
    }

    Matrix& operator=(const Matrix& rhs) {
        if (this == &rhs) {
            return *this;
        }
        Matrix temp(rhs);
        Swap(*this, temp);
        return *this;
    }

    Row operator[](size_t num) {
        return Row(num, this);
    }

    ConstRow operator[](size_t num) const {
        return ConstRow(num, this);
    }

    const T& At(size_t i, size_t j) const {
        if (i >= rows_num_ || j >= cols_num_) {
            throw std::out_of_range("Invalid matrix element's index.");
        }
        return ptr_[CalcOffset(i, j)];
    }

    T& At(size_t i, size_t j) {
        if (i >= rows_num_ || j >= cols_num_) {
            throw std::out_of_range("Invalid matrix element's index.");
        }
        return ptr_[CalcOffset(i, j)];
    }

    size_t GetColsNum() const {
        return cols_num_;
    }

    size_t GetRowsNum() const {
        return rows_num_;
    }

    bool IsEmpty() const {
        return rows_num_ == 0;
    }

private:
    size_t CalcOffset(size_t i, size_t j) const {
        return i * cols_num_ + j;
    }

    T* AllocateMatrixStorage(size_t rows, size_t cols) {
        ptr_ = new T[rows * cols];
        return ptr_;
    }

    static void Swap(Matrix& m1, Matrix& m2) noexcept {
        std::swap(m1.ptr_, m2.ptr_);
        std::swap(m1.rows_num_, m2.rows_num_);
        std::swap(m1.cols_num_, m2.cols_num_);
    }

private:
    T* ptr_;
    size_t rows_num_;
    size_t cols_num_;
};

#endif  // MATRIX1_MATRIX_H
