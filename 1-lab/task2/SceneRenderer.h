#pragma once
#include "Common/Position.h"

class SceneRenderer
{
public:
    static void Draw(sf::RenderTarget& window,
                     const std::vector<std::unique_ptr<IShape>>& shapes,
                     const Position& offset)
    {
        window.clear(sf::Color(10, 10, 40));

        sf::View view = window.getView();

        view.move({offset.x, offset.y});
        window.setView(view);

        for (const auto& shape : shapes)
        {
            shape->Draw(window);
        }
        // Восстановление вида (иначе арифметическая прогрессия смещения)
        view.move({-offset.x, -offset.y});
        window.setView(view);
    }
};
