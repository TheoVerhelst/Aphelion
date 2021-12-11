#ifndef SIMULATION_CANVAS_HPP
#define SIMULATION_CANVAS_HPP

#include <TGUI/TGUI.hpp>
#include <PhysicalModel.hpp>

class SimulationCanvas : public tgui::CanvasSFML {
public:
    typedef std::shared_ptr<SimulationCanvas> Ptr;
    typedef std::shared_ptr<const SimulationCanvas> ConstPtr;

    SimulationCanvas(PhysicalModel& model, tgui::Layout2d size);
    void update(const sf::Time& elapsedTime);
    static SimulationCanvas::Ptr create(PhysicalModel& model, tgui::Layout2d size = {"100%", "100%"});

private:
    PhysicalModel& _model;
    Vector2f _position{0, 0}; // In meters in the scene
    float _zoom{1}; // In pixel / meter
    float _panSpeed{300}; // In meter/second
    float _zoomSpeed{1.5}; // In pixel / meter / second

    void updateView();
};

#endif
