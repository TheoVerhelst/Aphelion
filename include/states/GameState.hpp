#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

#include <TGUI/Backend/Renderer/SFML-Graphics/CanvasSFML.hpp>
#include <TGUI/Widgets/Picture.hpp>
#include <states/AbstractState.hpp>
#include <systems/CollisionSystem.hpp>
#include <systems/GameplaySystem.hpp>
#include <systems/LightSystem.hpp>
#include <systems/PhysicsSystem.hpp>
#include <systems/RenderSystem.hpp>
#include <Scene.hpp>

// Forward declarations
namespace sf {
    class Time;
    class Font;
    class Texture;
    class Shader;
    class SoundBuffer;
}
namespace tgui {
    class Texture;
}
template <typename T>
class ResourceManager;
class StateStack;

class GameState : public AbstractState {
public:
    GameState(StateStack& stack,
        const ResourceManager<sf::Font>& fontManager,
        const ResourceManager<sf::Texture>& textureManager,
        const ResourceManager<tgui::Texture>& tguiTextureManager,
        ResourceManager<sf::Shader>& shaderManager,
        const ResourceManager<sf::SoundBuffer>& soundBufferManager);

    virtual tgui::Widget::Ptr buildGui() override;
    virtual void update(sf::Time dt) override;
    virtual bool handleTriggerAction(const TriggerAction& action) override;
    virtual bool handleContinuousAction(const Action& action, sf::Time dt) override;

private:
    Scene _scene;
    tgui::CanvasSFML::Ptr _canvas;
    const ResourceManager<sf::Shader>& _shaderManager;
    tgui::Picture::Ptr _background;
    const std::string _saveFile{"save.json"};
    CollisionSystem _collisionSystem;
    GameplaySystem _gameplaySystem;
    LightSystem _lightSystem;
    PhysicsSystem _physicsSystem;
    RenderSystem _renderSystem;
    const float _rotationSpeed{3};
    const float _zoomSpeed{15.f};
    const Vector2f _minViewSize{130.f, 70.f};
    const Vector2f _maxViewSize{6400.f, 4500.f};

    void registerComponents();
    void updateView(float zoom, bool rotate, sf::Time dt);
};

#endif // GAMESTATE_HPP
