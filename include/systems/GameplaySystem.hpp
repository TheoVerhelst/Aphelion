#ifndef GAMEPLAYSYSTEM_HPP
#define GAMEPLAYSYSTEM_HPP

#include <set>
#include <map>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <Action.hpp>
#include <vector.hpp>
#include <Scene.hpp>
#include <Observer.hpp>

class GameplaySystem : public Observer<const TriggerAction&>, public Observer<const ContinuousAction&>, public Observer<const sf::Time&> {
public:
    GameplaySystem(Scene& scene);
    virtual void update(const TriggerAction& action) override;
    virtual void update(const ContinuousAction& action) override;
    virtual void update(const sf::Time& action) override;
    void setRenderTarget(sf::RenderTarget& renderTarget);

private:
    Scene& _scene;
    sf::RenderTarget* _renderTarget{nullptr};
    const Vector2f maxViewSize{320000, 200000};
    const Vector2f minViewSize{130, 70};
};

#endif // GAMEPLAYSYSTEM_HPP
