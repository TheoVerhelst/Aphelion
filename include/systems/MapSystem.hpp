#ifndef MAPSYSTEM_HPP
#define MAPSYSTEM_HPP

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Time.hpp>
#include <Observer.hpp>
#include <Action.hpp>
#include <Scene.hpp>
#include <vector.hpp>

class MapSystem : public Observer<const TriggerAction&>, public Observer<const sf::Time&> {
public:
    MapSystem(Scene& scene, sf::RenderTarget& renderTarget);
    virtual void update(const TriggerAction& action) override;
    virtual void update(const sf::Time& dt) override;

private:
    Scene& _scene;
    sf::RenderTarget& _renderTarget;
    Vector2f _normalViewSize{1366.f, 768.f};
    Vector2f _mapViewSize{68300.f, 38400.f};
    bool _mapView{false};
};

#endif // MAPSYSTEM_HPP
