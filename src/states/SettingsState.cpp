#include <SFML/Window/Event.hpp>
#include <TGUI/Widgets/Panel.hpp>
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
#include <Input.hpp>


SettingsState::SettingsState(StateStack& stack, Settings& settings, sf::Window& window):
    AbstractState{stack},
    _settings{settings},
    _initialSettings{settings},
    _window{window} {
    // Find all fullscreen video modes, with the highest bits per pixel rating
    for (const sf::VideoMode& mode : sf::VideoMode::getFullscreenModes()) {
        std::string modeName{videoModeToName(mode)};
        if (not _videoModes.contains(modeName)
            or _videoModes.at(modeName).bitsPerPixel < mode.bitsPerPixel) {
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

    /// Main layout ///
    tgui::Panel::Ptr panel{tgui::Panel::create({"50%", "100%"})};
    panel->setPosition("25%-1px", "0%");

    tgui::Grid::Ptr grid{tgui::Grid::create()};
    grid->setPosition("10%", "10%");
    panel->add(grid);
    std::size_t row{0};
    
    /// Parameters ///
    float labelHeight{34};
    float widgetHeight{32};
    unsigned int labelTextSize{14};
    unsigned int buttonTextSize{16};
    auto labelWidth = tgui::bindInnerWidth(panel) * 0.35;
    auto widgetWidth = tgui::bindInnerWidth(panel) * 0.45;

    /// Resolution combo box ///
    tgui::Label::Ptr resolutionLabel{tgui::Label::create("Resolution")};
    resolutionLabel->setSize(labelWidth, labelHeight);
    resolutionLabel->setTextSize(labelTextSize);
    resolutionLabel->setVerticalAlignment(tgui::VerticalAlignment::Center);
    grid->addWidget(resolutionLabel, row, 0, tgui::Grid::Alignment::Left);

    tgui::ComboBox::Ptr resolutionListBox{tgui::ComboBox::create()};
    resolutionListBox->setSize(widgetWidth, widgetHeight);
    resolutionListBox->onItemSelect([this](const tgui::String& tguiModeName) noexcept {
        const std::string modeName{tguiModeName.toStdString()};
        if (not modeName.empty() and _videoModes.contains(modeName)) {
            _settings.videoMode = _videoModes.at(modeName);
        }
    });
    for (auto& modeName : _videoModeOrder) {
        resolutionListBox->addItem(modeName);
    }
    std::string currentModeName{videoModeToName(_initialSettings.videoMode)};
    if (_videoModes.contains(currentModeName)) {
        resolutionListBox->setSelectedItem(currentModeName);
    }
    grid->addWidget(resolutionListBox, row++, 1, tgui::Grid::Alignment::Left);

    /// Sound settings ///
    std::vector<std::pair<std::string, float&>> sliders{
        {"Main volume", _settings.soundSettings.mainVolume},
        {"Effects volume", _settings.soundSettings.effectsVolume},
        {"Music volume", _settings.soundSettings.musicVolume}
    };

    for (auto& [text, settingReference] : sliders) {
        tgui::Label::Ptr label{tgui::Label::create(text)};
        label->setSize(labelWidth, labelHeight);
        label->setVerticalAlignment(tgui::VerticalAlignment::Center);
        label->setTextSize(labelTextSize);
        grid->addWidget(label, row, 0, tgui::Grid::Alignment::Left);

        tgui::Slider::Ptr slider{tgui::Slider::create(0, 100)};
        slider->setSize(widgetWidth, widgetHeight);
        slider->setValue(settingReference);
        slider->onValueChange([this, &settingReference](float value) noexcept {
            settingReference = value;
        });
        grid->addWidget(slider, row, 1, tgui::Grid::Alignment::Left);
        ++row;
    }

    /// Buttons at the bottom ///
    tgui::HorizontalLayout::Ptr bottomLayout{tgui::HorizontalLayout::create()};
    bottomLayout->setSize("80%", "8%");
    bottomLayout->setPosition("10%", "90%");
    panel->add(bottomLayout);

    tgui::Button::Ptr cancelButton{tgui::Button::create("Cancel")};
    cancelButton->onPress([this] {
        _settings = _initialSettings;
        _stack.popStatesUntil(*this);
    });
    cancelButton->setTextSize(buttonTextSize);
    bottomLayout->add(cancelButton);
    bottomLayout->addSpace(0.1f);

    tgui::Button::Ptr okButton{tgui::Button::create("OK")};
    okButton->onPress([this, resolutionListBox] {
        if (_settings.videoMode != _initialSettings.videoMode) {
            _window.create(_settings.videoMode, "Aphelion", sf::Style::Fullscreen);
        }
        Settings::saveSettings(_settings);
        _stack.popStatesUntil(*this);
    });
    okButton->setTextSize(buttonTextSize);
    bottomLayout->add(okButton);
    bottomLayout->addSpace(0.1f);

    return panel;
}

bool SettingsState::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Escape) {
        _stack.popStatesUntil(*this);
        return true;
    }
    return false;
}

std::string SettingsState::videoModeToName(const sf::VideoMode& mode) {
    return std::to_string(mode.width) + "x" + std::to_string(mode.height);
}
