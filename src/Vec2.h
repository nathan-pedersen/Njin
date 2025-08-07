#pragma once

#include "utils.h"
#include <cmath>
#include <print>
#include <SFML/Graphics.hpp>

class Vec2
{
    public:
        f32 x, y;

        Vec2() : x(0.0f), y(0.0f)
        {
        }
        Vec2(f32 x, f32 y) : x(x), y(y)
        {
        }
        Vec2(float a) : x(a), y(a)
        {
        }

        bool operator==(const Vec2& rhs) const;
        Vec2 operator+(const Vec2& rhs) const;
        Vec2 operator-(const Vec2& rhs) const;
        Vec2 operator*(const f32& scalar) const;
        Vec2 operator/(const Vec2& rhs) const;
        void operator+=(const Vec2& rhs);
        void operator-=(const Vec2& rhs);
        void operator*=(const Vec2& rhs);
        void operator/=(const Vec2& rhs);
        operator sf::Vector2f();

        f32  length();
        void normalize();
        Vec2 normalized() const;
        static Vec2 randomPointInBounds(const Vec2& topLeft, const Vec2& bottomRight);
        void print() const;
};
