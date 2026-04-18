#pragma once

class IModelListener {
public:
    virtual ~IModelListener() = default;
    virtual void OnModelChanged() = 0;
};