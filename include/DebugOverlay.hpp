#ifndef DEBUGOVERLAY_HPP
#define DEBUGOVERLAY_HPP

#include <SFML/Graphics/Drawable.hpp>
#include <Observer.hpp>

// Forward declarations
namespace sf {
    class Time;
    class Texture;
    class Event;
}
namespace tgui {
    class BackendGui;
}
template <typename T>
class ResourceManager;
class PhysicsSystem;
class Scene;


class DebugOverlay : public sf::Drawable, public Observer<const sf::Time&> {
public:
    DebugOverlay(tgui::BackendGui& gui, PhysicsSystem& physicsSystem, Scene& scene,
        ResourceManager<sf::Texture>& textureManager);
    virtual void update(const sf::Time& dt) override;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    bool handleEvent(const sf::Event& event);
    void buildGui();

private:
    tgui::BackendGui& _gui;
    PhysicsSystem& _physicsSystem;
    Scene& _scene;
    ResourceManager<sf::Texture>& _textureManager;
    bool _debugView{false};
    bool _paused{false};

    void pauseTime();
    void resumeTime();
};

#endif // DEBUGOVERLAY_HPP
