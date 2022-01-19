#ifndef SCENE_HPP
#define SCENE_HPP

#include <cstddef>
#include <vector>
#include <queue>
#include <memory>
#include <map>
#include <cassert>
#include <typeindex>
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

    template <typename T>
    void registerComponent() {
        _arrays[typeid(T)] = std::make_unique<ArrayModel<T>>();
    }

    template <typename T, typename... Args>
    T& assignComponent(EntityId id, Args&&... args) {
        return getArray<T>().assign(id, std::forward<Args>(args)...);
    }

    template <typename T>
    const T& getComponent(EntityId id) const {
        return getArray<T>().get(id);
    }

    template <typename T>
    T& getComponent(EntityId id) {
        return getArray<T>().get(id);
    }

    template <typename T>
    void eraseComponent(EntityId id) {
        getArray<T>().erase(id);
    }

    template <typename... Types>
    SceneView<Types...> view() {
        return SceneView<Types...>(*this);
    }

private:
    // Type erasure idiom to store component arrays of any type. The base class
    // is called the concept, and the derived class the model. The base class
    // just defines the functions we need to call without knowing the underlying
    // type. In this case, it's only erase, since we need to call it when we
    // delete an entity (and we don't know the component types in that context).
    class ArrayConcept {
    public:
        virtual ~ArrayConcept() = default;
        virtual void erase(EntityId id) = 0;
    };

    // The derived class is template, so it can store the underlying array. It
    // also implements the virtual functions.
    template <typename T>
    class ArrayModel : public ArrayConcept {
    public:
        template <typename... Args>
        T& assign(EntityId id, Args&&... args) {
            assert(not mapping.contains(id));
            mapping.emplace(id, T(std::forward<Args>(args)...));
            return mapping.at(id);
        }

        const T& get(EntityId id) const {
            assert(contains(id));
            return mapping.at(id);
        }

        T& get(EntityId id) {
            return const_cast<T&>(const_cast<const ArrayModel<T>&>(*this).get(id));
        }

        virtual void erase(EntityId id) override {
            assert(contains(id));
            mapping.erase(id);
        }

        bool contains(EntityId id) const {
            return mapping.contains(id);
        }

    private:
        // TODO We should use a vector of component in order to store them
        // close in memory, but somehow this code breaks if we use a vector...
        // std::vector<T> components;
        std::map<EntityId, T> mapping;
    };

    // Maximum entity ID, which is also the size of the component arrays.
    EntityId _maxEntityId{0};
    // List of entity IDs that can be recycled for new entities. Gives the
    // smallest id first.
    std::priority_queue<EntityId, std::vector<EntityId>, std::greater<EntityId>> _freeIds;
    // Storage of component arrays, indexed by type_index
    std::unordered_map<std::type_index, std::unique_ptr<ArrayConcept>> _arrays;

    template <typename T>
    inline const ArrayModel<T>& getArray() const {
        assert(_arrays.contains(typeid(T)));
        return dynamic_cast<const ArrayModel<T>&>(*_arrays.at(typeid(T)));
    }

    template <typename T>
    inline ArrayModel<T>& getArray() {
        return const_cast<ArrayModel<T>&>(const_cast<const Scene&>(*this).getArray<T>());
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
        advanceUntilValid();
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
        advanceUntilValid();
        return *this;
    }
private:
    EntityId _id;
    const Scene& _scene;

    void advanceUntilValid() {
        while (_id < _scene._maxEntityId and not componentsMatch()) {
            ++_id;
        }
    }

    bool componentsMatch() const {
        // Fold expression, it's equivalent to calling [...].contains<T>(_id)
        // for every type T in Types and joining with and operators
        return (_scene.getArray<Types>().contains(_id) and ...);
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
        return SceneIterator<Types...>(_scene._maxEntityId, _scene);
    }

    template <typename T>
    T& getComponent(EntityId id) {
        return const_cast<T&>(const_cast<const SceneView<Types...>&>(*this).getComponent<T>(id));
    }

    template <typename T>
    const T& getComponent(EntityId id) const {
        static_assert((std::is_same_v<T, Types> or ...), "Component T is not in Types...");
        return _scene.getComponent<T>(id);
    }

private:
    Scene& _scene;
};

#endif // SCENE_HPP
