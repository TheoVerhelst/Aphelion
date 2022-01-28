#ifndef DEBUGOVERLAYSTATE_HPP
#define DEBUGOVERLAYSTATE_HPP

class DebugOverlayState {
public:
    DebugOverlay(PhysicsSystem& physicsSystem, Scene& scene,
        ResourceManager<sf::Texture>& textureManager);

};

#endif // DEBUGOVERLAYSTATE_HPP
