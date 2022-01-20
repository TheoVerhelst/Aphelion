#ifndef DEBUGOVERLAY_HPP
#define DEBUGOVERLAY_HPP

#include <SFML/Graphics/Drawable.hpp>
#include <TGUI/TGUI.hpp>
#include <systems/PhysicsSystem.hpp>
#include <Scene.hpp>
#include <ResourceManager.hpp>
#include <DebugInfo.hpp>
#include <components.hpp>

class DebugOverlay : public sf::Drawable {
public:
    DebugOverlay(tgui::BackendGui& gui, PhysicsSystem& physicsSystem, Scene& scene,
        ResourceManager<sf::Texture>& textureManager);
    void update();
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
