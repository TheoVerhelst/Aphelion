#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

#include <string>
#include <filesystem>
#include <queue>
#include <TGUI/Backend/Renderer/SFML-Graphics/CanvasSFML.hpp>
#include <TGUI/Widgets/Picture.hpp>
#include <states/AbstractState.hpp>
#include <systems/AnimationSystem.hpp>
#include <systems/AutoPilotSystem.hpp>
#include <systems/CollisionSystem.hpp>
#include <systems/GameplaySystem.hpp>
#include <systems/LightSystem.hpp>
#include <systems/PhysicsSystem.hpp>
#include <systems/RenderSystem.hpp>
#include <systems/SoundEffectsSystem.hpp>
#include <systems/ThermodynamicsSystem.hpp>
#include <InputManager.hpp>
#include <Scene.hpp>
#include <SceneSerializer.hpp>
#include <Event.hpp>
#include <Input.hpp>

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
struct Settings;

class GameState : public AbstractState {
public:
    GameState(StateStack& stack,
        ResourceManager<sf::Texture>& textureManager,
        ResourceManager<tgui::Texture>& tguiTextureManager,
        ResourceManager<sf::Shader>& shaderManager,
        ResourceManager<sf::SoundBuffer>& soundBufferManager,
        const Settings& settings,
        const std::filesystem::path& savePath);

    virtual tgui::Widget::Ptr buildGui() override;
    virtual bool update(sf::Time dt) override;
    virtual bool handleEvent(const sf::Event& event) override;
    virtual bool handleContinuousInputs(sf::Time dt) override;

private:
    Scene _scene;
    tgui::CanvasSFML::Ptr _canvas;
    ResourceManager<sf::Shader>& _shaderManager;
    tgui::Picture::Ptr _background;
    InputManager<GameInput> _inputManager;
    const std::map<GameInput, Event> _eventMapping;
    std::queue<Event> _eventQueue;
    AnimationSystem _animationSystem;
    AutoPilotSystem _autoPilotSystem;
    CollisionSystem _collisionSystem;
    GameplaySystem _gameplaySystem;
    LightSystem _lightSystem;
    PhysicsSystem _physicsSystem;
    RenderSystem _renderSystem;
    SoundEffectsSystem _soundEffectsSystem;
    ThermodynamicsSystem _thermodynamicsSystem;
    SceneSerializer _serializer;
    const float _rotationSpeed{3};
    const float _zoomSpeed{15.f};
    const Vector2f _minViewSize{130.f, 70.f};
    const Vector2f _maxViewSize{6400.f, 4500.f};
    bool _playerAutoRcsActivated{false};
    bool _playerAutoRcsClockwise{false};

    void registerComponents();
    void updateView(float zoom, bool rotate, sf::Time dt);
    void processtriggerEventsQueue();
};

#endif // GAMESTATE_HPP
