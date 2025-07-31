#pragma once

#include "Component.h"
#include "Vec2.h"

class CTransform : public Component
{
    public:
        Vec2 pos      = {0, 0};
        Vec2 velocity = {0, 0};
        f32  angle    = 0;
        CTransform()  = default;
        CTransform(const Vec2& p, const Vec2& v, f32 a) : pos(p), velocity(v), angle(a)
        {
        }
};
