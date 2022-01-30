#ifndef LOADGAMESTATE_HPP
#define LOADGAMESTATE_HPP

#include <string>
#include <vector>
#include <filesystem>
#include <states/AbstractState.hpp>

class LoadGameState : public AbstractState {
public:
    LoadGameState(StateStack& stack);
    virtual tgui::Widget::Ptr buildGui() override;
    virtual bool update(sf::Time dt) override;
    virtual bool handleTriggerAction(const TriggerAction& action) override;
    virtual bool handleContinuousAction(const Action& action, sf::Time dt) override;

private:
    const std::filesystem::path _saveDirectory{"saves/"};

    std::vector<std::filesystem::path> getSaveFilenames() const;
};

#endif // LOADGAME_HPP
