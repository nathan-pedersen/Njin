#pragma once

#include "Component.h"
#include "utils.h"
#include <SFML/Graphics/CircleShape.hpp>

class CShape : public Component
{
    public:
        sf::CircleShape shape;
        CShape() = default;
        CShape(float radius, size_t points, const sf::Color& fillColor,
               const sf::Color& outlineColor, f32 outlineThickness)
        {
            shape.setRadius(radius);
            shape.setFillColor(fillColor);
            shape.setPointCount(points);
            shape.setOutlineColor(outlineColor);
            shape.setOutlineThickness(outlineThickness);
            shape.setOrigin(radius, radius);
        }
};
