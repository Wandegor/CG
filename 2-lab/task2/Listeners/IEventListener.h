#pragma once
#include <string>

class IEventListener {
public:
    virtual void Update(const std::string& eventType, const sf::Event& event) = 0;
    // virtual void Update(const std::string& eventType) = 0;

protected:
    ~IEventListener() = default;
};
