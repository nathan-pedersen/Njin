#pragma once

#include "Component.h"
#include "Vec2.h"

class CTransform : public Component
{
    public:
        Vec2<f32> pos      = {0, 0};
        Vec2<f32> velocity = {0, 0};
        f32  angle    = 0;
        CTransform()  = default;
        CTransform(const Vec2<f32>& p, const Vec2<f32>& v, f32 a) : pos(p), velocity(v), angle(a)
        {
        }
};
