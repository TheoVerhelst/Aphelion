#ifndef TEMPERATURE_HPP
#define TEMPERATURE_HPP

#include <json.hpp>
#include <GridField.hpp>
#include <PolarField.hpp>
#include <TemperatureGraphics.hpp>

struct Temperature {
    float conductivity;
    float specificCapacity;
    float diffusivity;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Temperature, conductivity, specificCapacity)

struct CircleTemperature {
    PolarField<float> field;
    CircleTemperatureGraphics graphics;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CircleTemperature, field)

struct PolygonTemperature {
    GridField<float> field;
    PolygonTemperatureGraphics graphics;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PolygonTemperature, field)

#endif // TEMPERATURE_HPP
