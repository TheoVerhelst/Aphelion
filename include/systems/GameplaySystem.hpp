#ifndef GAMEPLAYSYSTEM_HPP
#define GAMEPLAYSYSTEM_HPP

// Forward declarations
namespace sf {
    class Time;
}
class Scene;


class GameplaySystem {
public:
    GameplaySystem(Scene& scene);
    void update(sf::Time dt);

private:
    Scene& _scene;
};

#endif // GAMEPLAYSYSTEM_HPP
