#ifndef DEBUGOVERLAY_HPP
#define DEBUGOVERLAY_HPP

#include <SFML/Graphics/Drawable.hpp>
#include <PhysicsSystem.hpp>
#include <Scene.hpp>
#include <DebugInfo.hpp>
#include <components.hpp>

class DebugOverlay : public sf::Drawable {
public:
    DebugOverlay(PhysicsSystem& physicsSystem, const SceneView<Body, DebugInfo>& scene);
    void update();
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    bool handleEvent(const sf::Event& event);

private:
    PhysicsSystem& _physicsSystem;
    SceneView<Body, DebugInfo> _scene;
    bool _debugView{false};
};

#endif // DEBUGOVERLAY_HPP
