#ifndef APLICATION_HPP
#define APLICATION_HPP

#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <states/StateStack.hpp>
#include <Settings.hpp>
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

    // Resource managers
    ResourceManager<sf::Texture> _textureManager;
    ResourceManager<tgui::Texture> _tguiTextureManager;
    ResourceManager<sf::Shader> _shaderManager;
    ResourceManager<sf::SoundBuffer> _soundBufferManager;
    Settings _settings;
    MusicManager _musicManager;

    void registerResources();
    void registerStateBuilders();
};

#endif // APPLICATION_HPP
