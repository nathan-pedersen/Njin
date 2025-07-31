#pragma once

#include "Component.h"

class CScore : public Component
{
    public:
        i32 score = 0;
        CScore()  = default;
        CScore(int s) : score(s)
        {
        }
};