#include <Scene.hpp>

EntityId Scene::createEntity() {
    EntityId id;
    if (not _freeIds.empty()) {
        id = _freeIds.back();
        _freeIds.pop_back();
    } else {
        id = _entityCounter++;
    }
    return id;
}

void Scene::removeEntity(EntityId id) {
    for (auto& destructor : _destructors[id]) {
        destructor();
    }
    _destructors.erase(id);
    _freeIds.push_back(id);
}
