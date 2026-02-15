#pragma once
#include <string>

class IDocumentListener {
public:
    virtual void Update(const std::string& eventType, void* data) = 0;

protected:
    ~IDocumentListener() = default;
};
