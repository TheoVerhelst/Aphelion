#ifndef GRIDFIELD_HPP
#define GRIDFIELD_HPP

#include <cmath>
#include <vector.hpp>
#include <Matrix.hpp>
#include <serializers.hpp>

template <typename T>
class GridField {
public:
    GridField(const Vector2i& gridSize, const Vector2f& cellSize, const Vector2f& origin);
    const Vector2i& getGridSize() const;
    T& at(const Vector2i& pos);
    const T& at(const Vector2i& pos) const;
    T interpolate(Vector2f pos) const;

    friend void to_json(nlohmann::json& json, const GridField& field) {
        json = {
            {"values", field._values},
            {"cellSize", field._cellSize},
            {"origin", field._origin}
        };
    }

    friend void from_json(const nlohmann::json& json, GridField& field) {
        json.at("values").get_to(field._values);
        json.at("cellSize").get_to(field._cellSize);
        json.at("origin").get_to(field._origin);
    }

private:
    Matrix<T> _values;
    Vector2f _cellSize;
    Vector2f _origin;
};

template <typename T>
GridField<T>::GridField(const Vector2i& gridSize, const Vector2f& cellSize, const Vector2f& origin):
    _values{gridSize.x, gridSize.y},
    _cellSize{cellSize},
    _origin{origin} {
}

template <typename T>
const Vector2i& GridField<T>::getGridSize() const {
    return {_values.getRows(), _values.getCols()};
}

template <typename T>
T& GridField<T>::at(const Vector2i& pos) {
    return _values(pos.x, pos.y);
}

template <typename T>
const T& GridField<T>::at(const Vector2i& pos) const {
    return _values(pos.x, pos.y);
}

template <typename T>
T GridField<T>::interpolate(Vector2f pos) const {
    pos -= _origin;
    pos.x = std::clamp(pos.x, 0.1f, static_cast<float>(_values.getRows() - 1) * _cellSize.x - 0.1f);
    pos.y = std::clamp(pos.y, 0.1f, static_cast<float>(_values.getCols() - 1) * _cellSize.y - 0.1f);

    std::size_t i{static_cast<std::size_t>(std::floor(pos.x / _cellSize.x))};
    if (i == _values.getRows() - 1) {
        i--;
    }
    std::size_t j{static_cast<std::size_t>(std::floor(pos.y / _cellSize.y))};
    if (j == _values.getCols() - 1) {
        j--;
    }
    const std::size_t ip{i + 1};
    const std::size_t jp{j + 1};
    const float x{static_cast<float>(i) * _cellSize};
    const float y{static_cast<float>(j) * _cellSize};
    const float xp{static_cast<float>(ip) * _cellSize};
    const float yp{static_cast<float>(jp) * _cellSize};
    const float ax{(xp - pos.x) / (xp - x)};
    const float ay{(yp - pos.y) / (yp - y)};
    return (at(i, j) * ax  + at(ip, j)  * (1 - ax)) * ay +
           (at(i, jp) * ax + at(ip, jp) * (1 - ax)) * (1 - ay);
}

#endif // GRIDFIELD_HPP
