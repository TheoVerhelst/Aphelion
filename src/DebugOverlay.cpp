#include <DebugOverlay.hpp>
#include <format.hpp>

DebugOverlay::DebugOverlay(tgui::BackendGui& gui, PhysicsSystem& physicsSystem,
    Scene& scene, ResourceManager<sf::Texture>& textureManager):
    _gui{gui},
    _physicsSystem{physicsSystem},
    _scene{scene},
    _textureManager{textureManager} {
}

void DebugOverlay::update() {
    if (_debugView) {
        for (EntityId id : _scene.view<Body, DebugInfo>()) {
            Body& body{_scene.getComponent<Body>(id)};
            DebugInfo& debugInfo{_scene.getComponent<DebugInfo>(id)};
            debugInfo.update(body);
        }
        _gui.get<tgui::Label>("timeSecondsDisplay")->setText(formatTime(_physicsSystem.getElapsedTime()));
        _gui.get<tgui::Label>("timeStepsDisplay")->setText(std::to_string(_physicsSystem.getStepCounter()));
    }
}

void DebugOverlay::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (_debugView) {
        for (EntityId id : _scene.view<Body, DebugInfo>()) {
            target.draw(_scene.getComponent<DebugInfo>(id), states);
        }
    }
}

bool DebugOverlay::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::F3) {
        _debugView = not _debugView;
        _gui.get<tgui::ChildWindow>("controlsPanel")->setVisible(_debugView);
        return true;
    }
    return false;
}

void DebugOverlay::buildGui() {
    _gui.get<tgui::ChildWindow>("controlsPanel")->setVisible(_debugView);
    _gui.get<tgui::ChildWindow>("controlsPanel")->setFocusable(false);

    // Spin controls. Do not work with GUI text file import for some reason.
    auto timeSpeedControl = tgui::SpinControl::create(-100, 100, 1, 2, 0.1f);
    timeSpeedControl->setPosition("timeSpeedLabel.right", "timeSpeedLabel.top");
    timeSpeedControl->setSize(124, 16);
    timeSpeedControl->onValueChange([this] (float value) {
        if(not _paused) {
            _physicsSystem.setTimeScale(value);
        }
    });
    _gui.get<tgui::ChildWindow>("controlsPanel")->add(timeSpeedControl, "timeSpeedControl");

    // Other bindings
    _gui.get<tgui::Button>("stepBackButton")->onPress([this] () {
        pauseTime();
        _physicsSystem.updateSteps(-1);
    });
    _gui.get<tgui::Button>("stepForwardButton")->onPress([this] () {
        pauseTime();
        _physicsSystem.updateSteps(1);
    });
    auto pauseButton = _gui.get<tgui::Button>("pauseButton");
    _gui.get<tgui::Button>("pauseButton")->onPress([this] () {
        if (_paused) {
            resumeTime();
        } else {
            pauseTime();
        }
    });
}

void DebugOverlay::pauseTime() {
    auto renderer = _gui.get<tgui::Button>("pauseButton")->getRenderer();
    renderer->setTexture(_textureManager.get("playButton"));
    renderer->setTextureHover(_textureManager.get("playHoverButton"));
    _physicsSystem.setTimeScale(0);
    _paused = true;
}

void DebugOverlay::resumeTime() {
    auto renderer = _gui.get<tgui::Button>("pauseButton")->getRenderer();
    renderer->setTexture(_textureManager.get("pauseButton"));
    renderer->setTextureHover(_textureManager.get("pauseHoverButton"));
    _paused = false;
    _physicsSystem.setTimeScale(_gui.get<tgui::SpinControl>("timeSpeedControl")->getValue());
}
