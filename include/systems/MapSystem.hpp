#ifndef MAPSYSTEM_HPP
#define MAPSYSTEM_HPP

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Time.hpp>
#include <Observer.hpp>
#include <Action.hpp>
#include <Scene.hpp>

class MapSystem : public Observer<const TriggerAction&>, public Observer<const sf::Time&> {
public:
    MapSystem(Scene& scene);
    virtual void update(const TriggerAction& action) override;
    virtual void update(const sf::Time& dt) override;
    void setRenderTarget(sf::RenderTarget& renderTarget);

private:
    Scene& _scene;
    sf::RenderTarget* _renderTarget;
};

#endif // MAPSYSTEM_HPP
