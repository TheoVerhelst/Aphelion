#include <Scene.hpp>

EntityId Scene::createEntity() {
    EntityId id;
    if (not _freeIds.empty()) {
        id = _freeIds.top();
        _freeIds.pop();
    } else {
        id = _maxEntityId++;
    }
    return id;
}

void Scene::removeEntity(EntityId id) {
    for (auto& [type, array] : _arrays) {
        array->erase(id);
    }
    _freeIds.push(id);
}
