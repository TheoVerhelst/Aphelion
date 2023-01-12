#ifndef THERMODYNAMICSSYSTEM_HPP
#define THERMODYNAMICSSYSTEM_HPP

// Forward declarations
namespace sf {
    class Time;
}
class Scene;
struct Event;

class ThermodynamicsSystem {
public:
    ThermodynamicsSystem(Scene& scene);
    void update(sf::Time dt);

private:
    Scene& _scene;
};

#endif // THERMODYNAMICSSYSTEM_HPP
