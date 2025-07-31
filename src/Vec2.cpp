#include "Vec2.h"

bool Vec2::operator==(const Vec2& rhs) const
{
    if (x == rhs.x && y == rhs.y)
        return true;
    return false;
}

Vec2 Vec2::operator+(const Vec2& rhs) const
{
    return {x + rhs.x, y + rhs.y};
}

Vec2 Vec2::operator-(const Vec2& rhs) const
{
    return {x - rhs.x, y - rhs.y};
}

Vec2 Vec2::operator*(const f32& scalar) const
{
    return {x * scalar, y * scalar};
}

Vec2 Vec2::operator/(const Vec2& rhs) const
{
    return {x / rhs.x, y / rhs.y};
}

void Vec2::operator+=(const Vec2& rhs)
{
    x += rhs.x;
    y += rhs.y;
}

void Vec2::operator-=(const Vec2& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
}

void Vec2::operator*=(const Vec2& rhs)
{
    x *= rhs.x;
    y *= rhs.y;
}

void Vec2::operator/=(const Vec2& rhs)
{
    x /= rhs.x;
    y /= rhs.y;
}

Vec2::operator sf::Vector2f()
{
    return sf::Vector2f(x, y);
}

f32 Vec2::length()
{
    return std::sqrt(x * x + y * y);
}

void Vec2::normalize()
{
    f32 l = this->length();
    x     = x / l;
    y     = y / l;
}

Vec2 Vec2::normalized() const
{
    Vec2 tmp = *this;
    tmp.normalize();
    return tmp;
}

void Vec2::print() const
{
    std::println("[{}, {}]", x, y);
}