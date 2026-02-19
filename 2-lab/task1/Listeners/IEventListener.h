#pragma once

enum class EventType;

class IEventListener {
public:
    virtual void Update(EventType eventType, const std::optional<sf::Event> &event) = 0;

protected:
    ~IEventListener() = default;
};
