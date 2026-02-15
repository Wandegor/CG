#pragma once
#include <string>

class IEventListener {
public:
    virtual void Update(const std::string& eventType, void* data) = 0;

protected:
    ~IEventListener() = default;
};
