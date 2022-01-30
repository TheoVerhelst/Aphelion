#include <Scene.hpp>

EntityId Scene::createEntity() {
    EntityId id;
    if (not _freeIds.empty()) {
        id = _freeIds.extract(0).value();
    } else {
        id = _maxEntityId++;
    }
    return id;
}

void Scene::removeEntity(EntityId id) {
    for (auto& [type, array] : _arrays) {
        array->erase(id);
    }
    _freeIds.insert(id);
}
