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
    GameplaySystem(const SceneView<Body, Player>& scene);
    void handleActions(const sf::Time& dt, const std::set<Action>& actions);
    void setRenderTarget(sf::RenderTarget& renderTarget);

private:
    SceneView<Body, Player> _scene;
    sf::RenderTarget* _renderTarget{nullptr};
};

#endif // GAMEPLAYSYSTEM_HPP
