#ifndef MAPSYSTEM_HPP
#define MAPSYSTEM_HPP

#include <Observer.hpp>
#include <vector.hpp>

// Forward declarations
namespace sf {
    class Time;
    class RenderTarget;
}
enum class Action;
typedef std::pair<Action, bool> TriggerAction;
typedef std::pair<Action, sf::Time> ContinuousAction;
class Scene;


class MapSystem : public Observer<const TriggerAction&>, public Observer<const ContinuousAction&>, public Observer<const sf::Time&> {
public:
    MapSystem(Scene& scene, sf::RenderTarget& renderTarget);
    virtual void update(const TriggerAction& action) override;
    virtual void update(const ContinuousAction& action) override;
    virtual void update(const sf::Time& dt) override;

private:
    Scene& _scene;
    sf::RenderTarget& _renderTarget;
    const Vector2f _minNormalViewSize{130.f, 70.f};
    const Vector2f _maxNormalViewSize{6400.f, 4500.f};
    const Vector2f _minMapViewSize{32000.f, 18000.f};
    const Vector2f _maxMapViewSize{320000.f, 180000.f};
    const float _zoomSpeed{15};
    const float _rotationSpeed{3};
    Vector2f _normalViewSize{1366.f, 768.f};
    Vector2f _mapViewSize{68300.f, 38400.f};
    bool _mapView{false};

    Vector2f clampVector(Vector2f v, const Vector2f& min, const Vector2f& max);
};

#endif // MAPSYSTEM_HPP
