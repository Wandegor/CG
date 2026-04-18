#pragma once

class IView
{
public:
    virtual ~IView() = default;
    virtual void Redraw() = 0;
};