#pragma once
#include "Common/Position.h"

class SceneRenderer
{
public:
    static void Draw(sf::RenderWindow& window,
                     const std::vector<std::unique_ptr<IShape>>& shapes,
                     const Position& offset)
    {
        window.clear(sf::Color(10, 10, 40));

        sf::View view = window.getView();

        view.move({offset.m_x, offset.m_y});
        window.setView(view);

        for (const auto& shape : shapes)
        {
            shape->Draw(window);
        }
        // Восстановление вида (иначе арифметическая прогрессия смещения)
        view.move({-offset.m_x, -offset.m_y});
        window.setView(view);
    }
};
