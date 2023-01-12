#ifndef POLARFIELD_HPP
#define POLARFIELD_HPP

#include <cmath>
#include <vector>
#include <cassert>
#include <vector.hpp>
#include <serializers.hpp>

template <typename T>
class PolarField {
public:
    std::size_t getRhoSteps() const;
    std::size_t getThetaSteps() const;
    float getRadius() const;
    void setRadius(float newRadius);
    float getRho(std::size_t rho_i) const;
    float getTheta(std::size_t theta_i) const;
    Vector2f getCartesian(std::size_t rho_i, std::size_t theta_i) const;
    T& at(std::size_t rho_i, std::size_t theta_i);
    const T& at(std::size_t rho_i, std::size_t theta_i) const;

    friend void to_json(nlohmann::json& json, const PolarField& field) {
        json = {
            {"values", field._values},
            {"rhoSteps", field._rhoSteps},
            {"thetaSteps", field._thetaSteps},
            {"centerValue", field._centerValue}
        };
    }

    friend void from_json(const nlohmann::json& json, PolarField& field) {
        json.at("rhoSteps").get_to(field._rhoSteps);
        json.at("thetaSteps").get_to(field._thetaSteps);
        json.at("centerValue").get_to(field._centerValue);
        if (json.contains("values")) {
            json.at("values").get_to(field._values);
            assert(field._values.size() == (field._rhoSteps - 1) * field._thetaSteps);
        } else {
            // Insert zeros if there is nothing in the json
            field._values.resize((field._rhoSteps - 1) * field._thetaSteps, static_cast<T>(0));
        }
    }

private:
    std::vector<T> _values;
    std::size_t _rhoSteps;
    std::size_t _thetaSteps;
    T _centerValue;
    float _radius;
};

template <typename T>
std::size_t PolarField<T>::getRhoSteps() const {
    return _rhoSteps;
}

template <typename T>
std::size_t PolarField<T>::getThetaSteps() const {
    return _thetaSteps;
}

template <typename T>
float PolarField<T>::getRadius() const {
    return _radius;
}

template <typename T>
void PolarField<T>::setRadius(float newRadius) {
    _radius = newRadius;
}

template <typename T>
float PolarField<T>::getRho(std::size_t rho_i) const {
    return (_radius * static_cast<float>(rho_i)) / static_cast<float>(getRhoSteps() - 1);
}

template <typename T>
float PolarField<T>::getTheta(std::size_t theta_i) const {
    return (2 * pi * static_cast<float>(theta_i)) / static_cast<float>(getThetaSteps());
}

template <typename T>
Vector2f PolarField<T>::getCartesian(std::size_t rho_i, std::size_t theta_i) const {
    return {
        getRho(rho_i) * std::cos(getTheta(theta_i)),
        getRho(rho_i) * std::sin(getTheta(theta_i))
    };
}

template <typename T>
T& PolarField<T>::at(std::size_t rho_i, std::size_t theta_i) {
    if (rho_i == 0) {
        return _centerValue;
    } else {
        return _values[(rho_i - 1) * _thetaSteps + theta_i];
    }
}

template <typename T>
const T& PolarField<T>::at(std::size_t rho_i, std::size_t theta_i) const {
    return const_cast<PolarField<T>&>(*this).at(rho_i, theta_i);
}

#endif // POLARFIELD_HPP
