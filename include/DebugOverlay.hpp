#ifndef DEBUGOVERLAY_HPP
#define DEBUGOVERLAY_HPP

#include <SFML/Graphics/Drawable.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <systems/PhysicsSystem.hpp>
#include <Scene.hpp>
#include <ResourceManager.hpp>
#include <DebugInfo.hpp>
#include <components.hpp>

class DebugOverlay : public sf::Drawable {
public:
    DebugOverlay(tgui::Gui& gui, PhysicsSystem& physicsSystem,
        const SceneView<Body, DebugInfo>& scene,
        ResourceManager<sf::Texture>& textureManager);
    void update();
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    bool handleEvent(const sf::Event& event);
    void buildGui();

private:
    tgui::Gui& _gui;
    PhysicsSystem& _physicsSystem;
    SceneView<Body, DebugInfo> _scene;
    ResourceManager<sf::Texture>& _textureManager;
    bool _debugView{false};
    bool _paused{false};

    void pauseTime();
    void resumeTime();
};

#endif // DEBUGOVERLAY_HPP
