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
    virtual bool handleEvent(const sf::Event& event) override;

private:
    const std::filesystem::path _saveDirectory{"saves/"};

    std::vector<std::filesystem::path> getSaveFilenames() const;
};

#endif // LOADGAME_HPP
