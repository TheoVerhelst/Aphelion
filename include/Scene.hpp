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

typedef std::uint32_t EntityId;

class Scene {
public:
    // Scene is non-copyable
    Scene() = default;
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

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
    std::vector<EntityId> view() const {
        // TODO Replace by STL algorithm
        std::vector<EntityId> res;
        for (EntityId id{0}; id < _maxEntityId; ++id) {
            // Fold expression, it's equivalent to calling
            // getArray<T>.contains(id) for every type T in Types and joining
            // them with "and" operators
            if ((getArray<Types>().contains(id) and ...)) {
                res.push_back(id);
            }
        }
        return res;
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
            mapping.emplace(id, components.size());
            return components.emplace_back(std::forward<Args>(args)...);
        }

        const T& get(EntityId id) const {
            assert(contains(id));
            return components[mapping.at(id)];
        }

        T& get(EntityId id) {
            assert(contains(id));
            return components[mapping.at(id)];
        }

        virtual void erase(EntityId id) override {
            assert(contains(id));
            std::size_t index{mapping.at(id)};
            // Update all subsequent index mappings
            for (auto& [otherId, otherIndex] : mapping) {
                if (otherIndex > index) {
                    otherIndex--;
                }
            }
            components.erase(components.begin() + index);
            mapping.erase(id);
        }

        bool contains(EntityId id) const {
            return mapping.contains(id);
        }

    private:
        std::vector<T> components;
        std::map<EntityId, std::size_t> mapping;
    };

    // Maximum entity ID we have assigned so far.
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
};

#endif // SCENE_HPP
