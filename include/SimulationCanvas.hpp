#ifndef SIMULATION_CANVAS_HPP
#define SIMULATION_CANVAS_HPP

#include <memory>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <PhysicalModel.hpp>
#include <DebugInfo.hpp>
#include <vector.hpp>

class SimulationCanvas : public tgui::CanvasSFML {
public:
    typedef std::shared_ptr<SimulationCanvas> Ptr;
    typedef std::shared_ptr<const SimulationCanvas> ConstPtr;

    SimulationCanvas(const char* typeName = "SimulationCanvas", bool initRenderer = true);
    static SimulationCanvas::Ptr create(tgui::Layout2d size = {"100%", "100%"});
    static SimulationCanvas::Ptr copy(SimulationCanvas::ConstPtr widget);
    void update(const sf::Time& elapsedTime, const PhysicalModel& model);
    bool handleEvent(const sf::Event& event);
    // Override display to show the debug info on top
    void display();

protected:
    tgui::Widget::Ptr clone() const override;

private:
    Vector2f _position{0, 0}; // In meters in the scene
    float _zoom{1}; // In pixel / meter
    float _panSpeed{300}; // In meter/second
    float _zoomSpeed{1.5}; // In pixel / meter / second
    bool _debugView{false};
    std::vector<DebugInfo> _debugInfos;

    void updateView();
};

#endif
