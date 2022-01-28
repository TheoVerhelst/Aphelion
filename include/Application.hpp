#ifndef APLICATION_HPP
#define APLICATION_HPP

#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <states/StateStack.hpp>
#include <InputManager.hpp>
#include <MusicManager.hpp>
#include <ResourceManager.hpp>

class Application {
public:
    Application();
    void run();

private:
    // Main objects
    sf::RenderWindow _window;
    tgui::Gui _gui;
    StateStack _stack;
    InputManager _inputManager;

    // Resource managers
    ResourceManager<sf::Font> _fontManager;
    ResourceManager<sf::Texture> _textureManager;
    ResourceManager<tgui::Texture> _tguiTextureManager;
    ResourceManager<sf::Shader> _shaderManager;
    ResourceManager<sf::SoundBuffer> _soundBufferManager;
    MusicManager _musicManager;

    void loadResources();
};

#endif // APPLICATION_HPP
