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

Vec2 Vec2::randomPointInBounds(const Vec2& topLeft, const Vec2& bottomRight)
{
    i32 x = rand_in_range_i32(topLeft.x, bottomRight.x);
    i32 y = rand_in_range_i32(topLeft.y, bottomRight.y);
    return Vec2(x, y);
}

void Vec2::print() const
{
    std::println("[{}, {}]", x, y);
}