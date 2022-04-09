#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

#include <string>
#include <filesystem>
#include <TGUI/Backend/Renderer/SFML-Graphics/CanvasSFML.hpp>
#include <TGUI/Widgets/Picture.hpp>
#include <states/AbstractState.hpp>
#include <systems/AnimationSystem.hpp>
#include <systems/CollisionSystem.hpp>
#include <systems/GameplaySystem.hpp>
#include <systems/LightSystem.hpp>
#include <systems/PhysicsSystem.hpp>
#include <systems/RenderSystem.hpp>
#include <InputManager.hpp>
#include <Scene.hpp>
#include <SceneSerializer.hpp>
#include <Action.hpp>

// Forward declarations
namespace sf {
    class Texture;
    class Shader;
    class SoundBuffer;
}
namespace tgui {
    class Texture;
}
template <typename T>
class ResourceManager;
struct SoundSettings;

class GameState : public AbstractState {
public:
    GameState(StateStack& stack,
        ResourceManager<sf::Texture>& textureManager,
        ResourceManager<tgui::Texture>& tguiTextureManager,
        ResourceManager<sf::Shader>& shaderManager,
        ResourceManager<sf::SoundBuffer>& soundBufferManager,
        SoundSettings& soundSettings,
        const std::filesystem::path& savePath);

    virtual tgui::Widget::Ptr buildGui() override;
    virtual bool update(sf::Time dt) override;
    virtual bool handleEvent(const sf::Event& event) override;
    void handleContinuousActions(sf::Time dt);

private:
    Scene _scene;
    tgui::CanvasSFML::Ptr _canvas;
    ResourceManager<sf::Shader>& _shaderManager;
    tgui::Picture::Ptr _background;
    InputManager<GameAction> _inputManager;
    AnimationSystem _animationSystem;
    CollisionSystem _collisionSystem;
    GameplaySystem _gameplaySystem;
    LightSystem _lightSystem;
    PhysicsSystem _physicsSystem;
    RenderSystem _renderSystem;
    SceneSerializer _serializer;
    const float _rotationSpeed{3};
    const float _zoomSpeed{15.f};
    const Vector2f _minViewSize{130.f, 70.f};
    const Vector2f _maxViewSize{6400.f, 4500.f};

    void registerComponents();
    void updateView(float zoom, bool rotate, sf::Time dt);
};

#endif // GAMESTATE_HPP
