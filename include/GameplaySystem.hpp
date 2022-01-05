#ifndef GAMEPLAYSYSTEM_HPP
#define GAMEPLAYSYSTEM_HPP

#include <set>
#include <SFML/System/Time.hpp>
#include <Action.hpp>
#include <Scene.hpp>
#include <components.hpp>

class GameplaySystem {
public:
    GameplaySystem(const SceneView<Body, Player>& scene);
    void handleActions(const sf::Time& dt, const std::set<Action>& actions);

private:
    SceneView<Body, Player> _scene;
};

#endif // GAMEPLAYSYSTEM_HPP
