#include <TGUI/Widgets/ChildWindow.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/VerticalLayout.hpp>
#include <TGUI/Widgets/HorizontalLayout.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/Slider.hpp>
#include <TGUI/Widgets/Grid.hpp>
#include <states/SettingsState.hpp>
#include <states/StateStack.hpp>
#include <Action.hpp>


SettingsState::SettingsState(StateStack& stack, SoundSettings& soundSettings):
    AbstractState{stack},
    _soundSettings{soundSettings},
    _initialSoundSettings{soundSettings} {
}

tgui::Widget::Ptr SettingsState::buildGui() {
    tgui::ChildWindow::Ptr window{tgui::ChildWindow::create("Settings")};
    window->setClientSize({350.f, 400.f});
    window->setPosition("50%", "50%");
    window->setOrigin(0.5f, 0.5f);
    window->onClose([this] {
        _stack.popStatesUntil(*this);
        // TODO when the PauseState window is closed, we should also pop this state
    });

    tgui::Grid::Ptr grid{tgui::Grid::create()};
    grid->setPosition("5%", "5%");
    window->add(grid);

    tgui::Label::Ptr mainVolumeLabel{tgui::Label::create("Main volume")};
    grid->addWidget(mainVolumeLabel, 0, 0, tgui::Grid::Alignment::Left);

    tgui::Slider::Ptr mainVolumeSlider{tgui::Slider::create(0, 100)};
    mainVolumeSlider->setValue(_soundSettings.mainVolume);
    mainVolumeSlider->onValueChange([this](float value) noexcept {
        _soundSettings.mainVolume = value;
    });
    grid->addWidget(mainVolumeSlider, 0, 1, tgui::Grid::Alignment::Right, {10, 0});

    tgui::Label::Ptr effectsVolumeLabel{tgui::Label::create("Effects volume")};
    grid->addWidget(effectsVolumeLabel, 1, 0, tgui::Grid::Alignment::Left);

    tgui::Slider::Ptr effectsVolumeSlider{tgui::Slider::create(0, 100)};
    effectsVolumeSlider->setValue(_soundSettings.effectsVolume);
    effectsVolumeSlider->onValueChange([this](float value) noexcept {
        _soundSettings.effectsVolume = value;
    });
    grid->addWidget(effectsVolumeSlider, 1, 1, tgui::Grid::Alignment::Right, {10, 0});

    tgui::Label::Ptr musicVolumeLabel{tgui::Label::create("Music volume")};
    grid->addWidget(musicVolumeLabel, 2, 0, tgui::Grid::Alignment::Left);

    tgui::Slider::Ptr musicVolumeSlider{tgui::Slider::create(0, 100)};
    musicVolumeSlider->setValue(_soundSettings.musicVolume);
    musicVolumeSlider->onValueChange([this](float value) noexcept {
        _soundSettings.musicVolume = value;
    });
    grid->addWidget(musicVolumeSlider, 2, 1, tgui::Grid::Alignment::Right, {10, 0});

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
    okButton->onPress([this] {
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

bool SettingsState::handleTriggerAction(const TriggerAction& actionPair) {
    auto& [action, start] = actionPair;
    if (action == Action::Exit and start) {
        _stack.popStatesUntil(*this);
    }
    return true;
}

bool SettingsState::handleContinuousAction(const Action&, sf::Time) {
    return true;
}
