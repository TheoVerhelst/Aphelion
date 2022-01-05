#ifndef GAMEPLAYSYSTEM_HPP
#define GAMEPLAYSYSTEM_HPP

#include <set>
#include <memory>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <Action.hpp>
#include <Scene.hpp>
#include <components.hpp>

class GameplaySystem {
public:
    GameplaySystem(Scene& scene);
    void handleTriggerActions(const std::map<Action, bool>& actions);
    void handleContinuousActions(const sf::Time& dt, const std::set<Action>& actions);
    void setRenderTarget(sf::RenderTarget& renderTarget);

private:
    Scene& _scene;
    sf::RenderTarget* _renderTarget{nullptr};
};

#endif // GAMEPLAYSYSTEM_HPP
