#pragma once
#define _USE_MATH_DEFINES

#include "utils.h"
#include <cmath>
#include <print>
#include <SFML/Graphics.hpp>

template <typename T>
class Vec2
{
    public:
        T x, y;

        Vec2() : x(0.0f), y(0.0f)
        {
        }
        Vec2(T x, T y) : x(x), y(y)
        {
        }
        Vec2(T a) : x(a), y(a)
        {
        }

        bool operator==(const Vec2& rhs) const;
        Vec2 operator+(const Vec2& rhs) const;
        Vec2 operator-(const Vec2& rhs) const;
        Vec2 operator*(const T& scalar) const;
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
        Vec2<f32>        rotate(f32 degrees) const;
        void print() const;
};
