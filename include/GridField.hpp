#ifndef GRIDFIELD_HPP
#define GRIDFIELD_HPP

#include <cmath>
#include <vector>
#include <vector.hpp>
#include <serializers.hpp>

template <typename T>
class GridField {
public:
    GridField() = default;
    GridField(const GridField<T>& other) = default;
    const Vector2s& getGridSize() const;
    const Vector2f& getCellSize() const;
    const Vector2f& getOrigin() const;
    T& at(std::size_t row, std::size_t col);
    const T& at(std::size_t row, std::size_t col) const;
    Vector2f getPos(std::size_t row, std::size_t col) const;

    friend void to_json(nlohmann::json& json, const GridField& field) {
        json = {
            {"values", field._values},
            {"gridSize", field._gridSize},
            {"cellSize", field._cellSize},
            {"origin", field._origin}
        };
    }

    friend void from_json(const nlohmann::json& json, GridField& field) {
        json.at("values").get_to(field._values);
        json.at("gridSize").get_to(field._gridSize);
        json.at("cellSize").get_to(field._cellSize);
        json.at("origin").get_to(field._origin);
        assert(field._values.size() == field._gridSize.x * field._gridSize.y);
    }

private:
    std::vector<T> _values;
    Vector2s _gridSize;
    Vector2f _cellSize;
    Vector2f _origin;
};

template <typename T>
const Vector2s& GridField<T>::getGridSize() const {
    return _gridSize;
}

template <typename T>
const Vector2f& GridField<T>::getCellSize() const {
    return _cellSize;
}

template <typename T>
const Vector2f& GridField<T>::getOrigin() const {
    return _origin;
}

template <typename T>
T& GridField<T>::at(std::size_t row, std::size_t col) {
    return _values[row * _gridSize.y + col];
}

template <typename T>
const T& GridField<T>::at(std::size_t row, std::size_t col) const {
    return _values[row * _gridSize.y + col];
}

template <typename T>
Vector2f GridField<T>::getPos(std::size_t row, std::size_t col) const {
    return Vector2f(static_cast<float>(row) * _cellSize.x, static_cast<float>(col) * _cellSize.y) - _origin;
}

#endif // GRIDFIELD_HPP
