#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Mouse.hpp>

struct MouseData
{
    sf::Vector2i pos;
    sf::Mouse::Button button;
};
