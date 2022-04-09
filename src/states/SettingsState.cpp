#include <SFML/Window/Event.hpp>
#include <TGUI/Widgets/ChildWindow.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/ComboBox.hpp>
#include <TGUI/Widgets/VerticalLayout.hpp>
#include <TGUI/Widgets/HorizontalLayout.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/Slider.hpp>
#include <TGUI/Widgets/Grid.hpp>
#include <SFML/Window/Window.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <states/SettingsState.hpp>
#include <states/StateStack.hpp>
#include <Action.hpp>


SettingsState::SettingsState(StateStack& stack, SoundSettings& soundSettings, sf::Window& window):
    AbstractState{stack},
    _soundSettings{soundSettings},
    _initialSoundSettings{soundSettings},
    _window{window},
    _initalScreenSize{window.getSize()} {
    // Find all fullscreen video modes, with the highest bits per pixel rating
    for (const sf::VideoMode& mode : sf::VideoMode::getFullscreenModes()) {
        std::string modeName{std::to_string(mode.width) + "x" + std::to_string(mode.height)};
        if (not _videoModes.contains(modeName) or _videoModes.at(modeName).bitsPerPixel < mode.bitsPerPixel) {
            _videoModes[modeName] = mode;
            // Maintain the order of video modes
            // TODO there is probably a cleaner way
            auto it = std::find(_videoModeOrder.begin(), _videoModeOrder.end(), modeName);
            if (it != _videoModeOrder.end()) {
                _videoModeOrder.erase(it);
            }
            _videoModeOrder.push_back(modeName);
        }
    }
}

tgui::Widget::Ptr SettingsState::buildGui() {
    tgui::ChildWindow::Ptr window{tgui::ChildWindow::create("Settings")};
    window->setClientSize({350.f, 400.f});
    window->setPosition("50%", "50%");
    window->setOrigin(0.5f, 0.5f);
    window->onClose([this] {
        _stack.popStatesUntil(*this);
    });

    tgui::Grid::Ptr grid{tgui::Grid::create()};
    grid->setPosition("5%", "5%");
    window->add(grid);
    std::size_t row{0};

    float labelHeight{35};
    auto labelWidth = tgui::bindInnerWidth(window) * 0.35;
    float widgetHeight{20};
    auto widgetWidth = tgui::bindInnerWidth(window) * 0.5;

    tgui::Label::Ptr resolutionLabel{tgui::Label::create("Resolution")};
    resolutionLabel->setSize(labelWidth, labelHeight);
    grid->addWidget(resolutionLabel, row, 0, tgui::Grid::Alignment::Left);

    tgui::ComboBox::Ptr resolutionListBox{tgui::ComboBox::create()};
    resolutionListBox->setSize(widgetWidth, widgetHeight);
    for (auto& modeName : _videoModeOrder) {
        resolutionListBox->addItem(modeName);
    }
    std::string currentModeName{std::to_string(_initalScreenSize.x) + "x" + std::to_string(_initalScreenSize.y)};
    if (_videoModes.contains(currentModeName)) {
        resolutionListBox->setSelectedItem(currentModeName);
    }
    grid->addWidget(resolutionListBox, row++, 1, tgui::Grid::Alignment::Left);

    tgui::Label::Ptr mainVolumeLabel{tgui::Label::create("Main volume")};
    mainVolumeLabel->setSize(labelWidth, labelHeight);
    grid->addWidget(mainVolumeLabel, row, 0, tgui::Grid::Alignment::Left);

    tgui::Slider::Ptr mainVolumeSlider{tgui::Slider::create(0, 100)};
    mainVolumeSlider->setSize(widgetWidth, widgetHeight);
    mainVolumeSlider->setValue(_soundSettings.mainVolume);
    mainVolumeSlider->onValueChange([this](float value) noexcept {
        _soundSettings.mainVolume = value;
    });
    grid->addWidget(mainVolumeSlider, row++, 1, tgui::Grid::Alignment::Right);

    tgui::Label::Ptr effectsVolumeLabel{tgui::Label::create("Effects volume")};
    effectsVolumeLabel->setSize(labelWidth, labelHeight);
    grid->addWidget(effectsVolumeLabel, row, 0, tgui::Grid::Alignment::Left);

    tgui::Slider::Ptr effectsVolumeSlider{tgui::Slider::create(0, 100)};
    effectsVolumeSlider->setSize(widgetWidth, widgetHeight);
    effectsVolumeSlider->setValue(_soundSettings.effectsVolume);
    effectsVolumeSlider->onValueChange([this](float value) noexcept {
        _soundSettings.effectsVolume = value;
    });
    grid->addWidget(effectsVolumeSlider, row++, 1, tgui::Grid::Alignment::Right);

    tgui::Label::Ptr musicVolumeLabel{tgui::Label::create("Music volume")};
    musicVolumeLabel->setSize(labelWidth, labelHeight);
    grid->addWidget(musicVolumeLabel, row, 0, tgui::Grid::Alignment::Left);

    tgui::Slider::Ptr musicVolumeSlider{tgui::Slider::create(0, 100)};
    musicVolumeSlider->setSize(widgetWidth, widgetHeight);
    musicVolumeSlider->setValue(_soundSettings.musicVolume);
    musicVolumeSlider->onValueChange([this](float value) noexcept {
        _soundSettings.musicVolume = value;
    });
    grid->addWidget(musicVolumeSlider, row++, 1, tgui::Grid::Alignment::Right);

    tgui::HorizontalLayout::Ptr bottomLayout{tgui::HorizontalLayout::create()};
    bottomLayout->setSize("80%", "8%");
    bottomLayout->setPosition("10%", "90%");
    window->add(bottomLayout);

    tgui::Button::Ptr cancelButton{tgui::Button::create("Cancel")};
    cancelButton->onPress([this] {
        _soundSettings = _initialSoundSettings;
        _stack.popStatesUntil(*this);
    });
    cancelButton->setTextSize(18);
    bottomLayout->add(cancelButton);
    bottomLayout->addSpace(0.1f);

    tgui::Button::Ptr okButton{tgui::Button::create("OK")};
    okButton->onPress([this, resolutionListBox] {
        std::string modeName{resolutionListBox->getSelectedItem()};
        if (not modeName.empty() and _videoModes.contains(modeName)) {
            const sf::VideoMode& mode{_videoModes.at(modeName)};
            if (mode.width != _initalScreenSize.x and mode.height != _initalScreenSize.y) {
                _window.create(mode, "Aphelion", sf::Style::Fullscreen);
            }
        }
        _stack.popStatesUntil(*this);
    });
    okButton->setTextSize(18);
    bottomLayout->add(okButton);
    bottomLayout->addSpace(0.1f);

    return window;
}

bool SettingsState::update(sf::Time) {
    return true;
}

bool SettingsState::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Escape) {
        _stack.popStatesUntil(*this);
        return true;
    }
    return false;
}
