#ifndef SCENE_HPP
#define SCENE_HPP

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <list>
#include <map>
#include <functional>
#include <type_traits>
#include <iterator>

typedef std::uint32_t EntityId;

template <typename... Types>
class SceneView;

class Scene {
public:
    EntityId createEntity();
    void removeEntity(EntityId id);

    template <typename T, typename... Args>
    T& assignComponent(EntityId id, Args&&... args) {
        // Register the action of cleaning up this component when the entity
        // will be destroyed
        _destructors[id].push_back(std::bind(&Scene::removeComponent<T>, this, id));
        // Add a new default-constructed component to the list
        T& component{getComponentList<T>().emplace_back(std::forward<Args>(args)...)};
        // Map the added element to the entity ID
        getComponentMap<T>().emplace(id, --getComponentList<T>().end());
        return component;
    }

    template <typename T>
    const T& getComponent(EntityId id) const {
        return *getComponentMap<T>().at(id);
    }

    template <typename T>
    T& getComponent(EntityId id) {
        return const_cast<T&>(const_cast<const Scene&>(*this).getComponent<T>(id));
    }

    template <typename T>
    void removeComponent(EntityId id) {
        getComponentList<T>().erase(getComponentMap<T>().at(id));
        getComponentMap<T>().erase(id);
    }

    template <typename... Types>
    SceneView<Types...> view() {
        return SceneView<Types...>(*this);
    }

private:
    template <typename T>
    using ComponentList = std::list<T>;
    template <typename T>
    using ComponentMap = std::map<EntityId, typename std::list<T>::iterator>;

    EntityId _entityCounter{0};
    std::vector<EntityId> _freeIds;
    std::map<EntityId, std::vector<std::function<void()>>> _destructors;

    template <typename T>
    const ComponentList<T>& getComponentList() const {
        static ComponentList<T> list;
        return list;
    }

    template <typename T>
    ComponentList<T>& getComponentList() {
        return const_cast<ComponentList<T>&>(const_cast<const Scene&>(*this).getComponentList<T>());
    }

    template <typename T>
    const ComponentMap<T>& getComponentMap() const {
        static ComponentMap<T> map;
        return map;
    }

    template <typename T>
    ComponentMap<T>& getComponentMap() {
        return const_cast<ComponentMap<T>&>(const_cast<const Scene&>(*this).getComponentMap<T>());
    }

    template <typename... Types>
    friend class SceneView;

    template <typename... Types>
    friend class SceneIterator;
};

template <typename... Types>
class SceneIterator {
public:
    typedef EntityId difference_type;
    typedef EntityId value_type;
    typedef EntityId* pointer;
    typedef EntityId& reference;
    typedef std::forward_iterator_tag iterator_category;

    SceneIterator(EntityId id, const Scene& scene):
        _id{id},
        _scene{scene} {
        advanceValid();
    }

    EntityId operator*() const {
        return _id;
    }

    bool operator==(const SceneIterator& other) {
        return _id == other._id;
    }

    bool operator!=(const SceneIterator& other) {
        return not (*this == other);
    }

    SceneIterator<Types...>& operator++() {
        ++_id;
        advanceValid();
        return *this;
    }
private:
    EntityId _id;
    const Scene& _scene;

    void advanceValid() {
        while (_id < _scene._entityCounter and not componentsMatch<Types...>()) {
            ++_id;
        }
    }

    template <typename T, typename... Rest>
    bool componentsMatch() const requires (sizeof...(Rest) > 0) {
        return _scene.getComponentMap<T>().contains(_id) and componentsMatch<Rest...>();
    }

    template <typename T>
    bool componentsMatch() const {
        return _scene.getComponentMap<T>().contains(_id);
    }
};



template <typename... Types>
class SceneView {
public:
    SceneView(Scene& scene):
        _scene{scene} {
    }

    SceneIterator<Types...> begin() const {
        return SceneIterator<Types...>(0, _scene);
    }

    SceneIterator<Types...> end() const {
        return SceneIterator<Types...>(_scene._entityCounter, _scene);
    }

    template <typename T>
    T& getComponent(EntityId id) {
        return const_cast<T&>(const_cast<const SceneView<Types...>&>(*this).getComponent<T>(id));
    }

    template <typename T>
    const T& getComponent(EntityId id) const {
        static_assert(std::disjunction_v<std::is_same<T, Types>...>,
            "Cannot get component outside of Types...");
        return _scene.getComponent<T>(id);
    }

private:
    Scene& _scene;
};

#endif // SCENE_HPP
