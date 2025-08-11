#include "Vec2.h"

template <typename T>
bool Vec2<T>::operator==(const Vec2<T>& rhs) const
{
    if (x == rhs.x && y == rhs.y)
        return true;
    return false;
}

template <typename T>
Vec2<T> Vec2<T>::operator+(const Vec2<T>& rhs) const
{
    return {x + rhs.x, y + rhs.y};
}

template <typename T>
Vec2<T> Vec2<T>::operator-(const Vec2<T>& rhs) const
{
    return {x - rhs.x, y - rhs.y};
}

template <typename T>
Vec2<T> Vec2<T>::operator*(const T& scalar) const
{
    return {x * scalar, y * scalar};
}

template <typename T>
Vec2<T> Vec2<T>::operator/(const Vec2<T>& rhs) const
{
    return {x / rhs.x, y / rhs.y};
}

template <typename T>
void Vec2<T>::operator+=(const Vec2<T>& rhs)
{
    x += rhs.x;
    y += rhs.y;
}

template <typename T>
void Vec2<T>::operator-=(const Vec2<T>& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
}

template <typename T>
void Vec2<T>::operator*=(const Vec2<T>& rhs)
{
    x *= rhs.x;
    y *= rhs.y;
}

template <typename T>
void Vec2<T>::operator/=(const Vec2<T>& rhs)
{
    x /= rhs.x;
    y /= rhs.y;
}

template <typename T>
Vec2<T>::operator sf::Vector2f()
{
    return sf::Vector2f(x, y);
}

template <typename T>
f32 Vec2<T>::length()
{
    return std::sqrt(x * x + y * y);
}

template <typename T>
void Vec2<T>::normalize()
{
    f32 l = this->length();
    x     = x / l;
    y     = y / l;
}

template <typename T>
Vec2<T> Vec2<T>::normalized() const
{
    Vec2<T> tmp = *this;
    tmp.normalize();
    return tmp;
}

template <typename T>
Vec2<T> Vec2<T>::randomPointInBounds(const Vec2<T>& topLeft, const Vec2<T>& bottomRight)
{
    i32 x = rand_in_range_i32(topLeft.x, bottomRight.x);
    i32 y = rand_in_range_i32(topLeft.y, bottomRight.y);
    return Vec2<T>(x, y);
}

Vec2<f32> Vec2<f32>::rotate(f32 degrees) const
{
    f32 radians = degrees * (M_PI / 180.0f);
    f32 cosDeg  = cosf(radians);
    f32 sinDeg  = sinf(radians);
    f32 bx      = (cosDeg * this->x) + (-sinDeg * this->y);
    f32 by      = (sinDeg * this->x) + (cosDeg * this->y);
    return Vec2<f32>{bx, by};
}

template <typename T>
void Vec2<T>::print() const
{
    std::println("[{}, {}]", x, y);
}

// Explicit instantiations for the types you use, e.g. float, int
template class Vec2<float>;
template class Vec2<int>;