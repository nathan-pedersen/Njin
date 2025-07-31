#pragma once

#include "Component.h"

class CLifeSpan : public Component
{
    public:
        int totalLifeSpan     = 0;
        int remainingLifeSpan = 0;
        CLifeSpan()
        {
        }
        CLifeSpan(int totalLifeSpan)
            : totalLifeSpan(totalLifeSpan), remainingLifeSpan(totalLifeSpan)
        {
        }
};
