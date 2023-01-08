#ifndef POLARFIELD_HPP
#define POLARFIELD_HPP

#include <cmath>
#include <vector.hpp>
#include <Matrix.hpp>
#include <serializers.hpp>

template <typename T>
class PolarField {
public:
    PolarField(std::size_t rhoSteps, std::size_t thetaSteps, float radius);
    T& at(std::size_t rho, std::size_t theta);
    const T& at(std::size_t rho, std::size_t theta) const;
    T interpolate(Vector2f pos) const;

    friend void to_json(nlohmann::json& json, const PolarField& field) {
        json = {
            {"values", field._values},
            {"radius", field._radius}
        };
    }

    friend void from_json(const nlohmann::json& json, PolarField& field) {
        json.at("values").get_to(field._values);
        json.at("radius").get_to(field._radius);
    }

private:
    Matrix<T> _values;
    float _radius;
};

template <typename T>
PolarField<T>::PolarField(std::size_t rhoSteps, std::size_t thetaSteps, float radius):
    _values{rhoSteps, thetaSteps},
    _radius{radius} {
}

template <typename T>
T& PolarField<T>::at(std::size_t rho, std::size_t theta) {
    return _values(rho, theta);
}

template <typename T>
const T& PolarField<T>::at(std::size_t rho, std::size_t theta) const {
    return _values(rho, theta);
}

template <typename T>
T PolarField<T>::interpolate(Vector2f pos) const {

}

#endif // POLARFIELD_HPP
