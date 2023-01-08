#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <vector>
#include <json.hpp>

template <typename T>
class Matrix {
public:
    Matrix(std::size_t rows, std::size_t cols);
    T& operator()(std::size_t i, std::size_t j);
    const T& operator()(std::size_t i, std::size_t j) const;
    std::size_t getRows() const;
    std::size_t getCols() const;

    friend void to_json(nlohmann::json& json, const Matrix& matrix) {
        json = {
            {"rows", matrix._rows},
            {"cols", matrix._cols},
            {"data", matrix._data}
        };
    }

    friend void from_json(const nlohmann::json& json, Matrix& matrix) {
        json.at("rows").get_to(matrix._rows);
        json.at("cols").get_to(matrix._rows);
        json.at("data").get_to(matrix._data);
    }

private:
    std::size_t _rows;
    std::size_t _cols;
    std::vector<T> _data;
};


template <typename T>
Matrix<T>::Matrix(std::size_t rows, std::size_t cols):
    _rows{rows},
    _cols{cols},
    _data(rows * cols) {
}

template <typename T>
T& Matrix<T>::operator()(std::size_t i, std::size_t j) {
    return _data[i * _rows + j];
}

template <typename T>
const T& Matrix<T>::operator()(std::size_t i, std::size_t j) const {
    return _data[i * _rows + j];
}

template <typename T>
std::size_t Matrix<T>::getRows() const {
    return _rows;
}

template <typename T>
std::size_t Matrix<T>::getCols() const {
    return _cols;
}


#endif // MATRIX_HPP
