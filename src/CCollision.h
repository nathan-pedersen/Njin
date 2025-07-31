#pragma once

#include "Component.h"

class CCollision : public Component
{
    public:
        f32 radius   = 0;
        CCollision() = default;
        CCollision(f32 r) : radius(r)
        {
        }
};