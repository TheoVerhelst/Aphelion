#ifndef GAMEPLAYSYSTEM_HPP
#define GAMEPLAYSYSTEM_HPP

#include <set>
#include <map>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <Action.hpp>
#include <vector.hpp>
#include <Scene.hpp>

class GameplaySystem {
public:
    GameplaySystem(Scene& scene);
    void handleTriggerActions(const std::map<Action, bool>& actions);
    void handleContinuousActions(const sf::Time& dt, const std::set<Action>& actions);
    void setRenderTarget(sf::RenderTarget& renderTarget);

private:
    Scene& _scene;
    sf::RenderTarget* _renderTarget{nullptr};
    const Vector2f maxViewSize{320000, 200000};
    const Vector2f minViewSize{130, 70};
};

#endif // GAMEPLAYSYSTEM_HPP
