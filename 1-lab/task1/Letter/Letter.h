#pragma once
#include <memory>
#include <memory>

#include "Color.h"
#include "Position.h"
#include "DrawStrategy/IDrawStrategy.h"
#include "DrawStrategy/DrawT.h"
#include "JumpStrategy/IJumpStrategy.h"

class Letter
{
    Color m_color;
    Position m_position;
    std::unique_ptr<IDrawStrategy> m_drawStrategy;
    std::unique_ptr<IJumpStrategy> m_jumpStrategy;

public:
    Letter(
        Color color,
        Position position,
        std::unique_ptr<IDrawStrategy> drawStrategy,
        std::unique_ptr<IJumpStrategy> jumpStrategy
    )
        : m_color(color), m_position(position),
    m_drawStrategy(std::move(drawStrategy)),
    m_jumpStrategy(std::move(jumpStrategy))
    {
        if (m_jumpStrategy) {
            m_jumpStrategy->StartJumping(this->m_position.m_y);
        }
    }

    void SetColor(Color color)
    {
        m_color = color;
    }

    void SetPosition(Position position)
    {
        m_position = position;
    }

    void Draw(sf::RenderWindow &window)
    {
        m_drawStrategy->Draw(window, m_color, m_position);
    }

    void Update(float deltaTime)
    {
        m_jumpStrategy->Update(deltaTime, this->m_position.m_y);
    }
};
