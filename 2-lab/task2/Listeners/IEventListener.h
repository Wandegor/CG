#pragma once

enum class EventType;

class IEventListener {
public:
    virtual void Update(EventType eventType, const std::optional<sf::Event> &event = std::nullopt) = 0;

protected:
    ~IEventListener() = default;
};
