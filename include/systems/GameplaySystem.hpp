#ifndef GAMEPLAYSYSTEM_HPP
#define GAMEPLAYSYSTEM_HPP

#include <SFML/Graphics/RenderTarget.hpp>
#include <Action.hpp>
#include <Scene.hpp>
#include <Observer.hpp>

class GameplaySystem : public Observer<const TriggerAction&>, public Observer<const ContinuousAction&> {
public:
    GameplaySystem(Scene& scene);
    virtual void update(const TriggerAction& action) override;
    virtual void update(const ContinuousAction& action) override;

private:
    Scene& _scene;
};

#endif // GAMEPLAYSYSTEM_HPP
