#ifndef OBSERVER_HPP
#define OBSERVER_HPP

#include <vector>
#include <utility>

template <typename... Args>
class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(Args&&... args) = 0;
};

template <typename... Args>
class EventSource {
public:
    void registerObserver(Observer<Args...>& observer) {
        _observers.push_back(&observer);
    }

    void notifyObservers(Args&&... args) {
        for (auto observer : _observers) {
            observer->update(std::forward<Args>(args)...);
        }
    }

private:
    std::vector<Observer<Args...>*> _observers;
};

#endif // OBSERVER_HPP
